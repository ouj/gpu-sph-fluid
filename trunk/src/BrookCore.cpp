#include "stdafx.h"
#include "Setting.h"
#include "SmoothedParticles.h"
#include "MathHelper.h"
#include "BrookCore.h"
#include "GPUCore\NeighborFinding.h"
#include "LogHelper.h"
#include "TimeHelper.h"
#include <algorithm>

#ifdef _DEBUG
#define VERIFY_STREAM(SNAME) {\
    if((SNAME).error()){CLogHelper::WriteLine((SNAME).errorLog());}}
#else
#define VERIFY_STREAM(SNAME) __noop;
#endif

BrookCore::BrookCore(void) :
m_CellRanges(NULL), m_ParticleHash(NULL)
{
    m_HashSize = 1;
}

BrookCore::~BrookCore(void)
{
}

class Int2Greater
{
public:
    bool operator()(int2 a, int2 b){return (a.x > b.x);}
};

void BrookCore::NeighborFinding(::brook::Stream<float3> &PositionStream, int posStreamSize)
{
	long nfEval;
	CTimeHelper::BeginTimer(100);
	CLogHelper::WriteFormatLine("----------------Inside Neighbor Finding----------------");

	CTimeHelper::BeginTimer(110);
    if (posStreamSize > m_HashSize)
    {
        while (posStreamSize > m_HashSize)
        {
            m_HashSize <<= 1;
        }

        unsigned int hashNum_Dim[] = { m_HashSize };
        m_ParticleHash = brook::Stream<int2>(sizeof(hashNum_Dim) / sizeof(unsigned int), hashNum_Dim); 

        m_Bucket_Num = m_HashSize >> 2;
        unsigned int cellRange_Dim[] = { m_HashSize };
        m_CellRanges = brook::Stream<int2>(sizeof(cellRange_Dim) / sizeof(unsigned int), cellRange_Dim);

        VERIFY_STREAM(m_ParticleHash);
    }
	nfEval = CTimeHelper::EndTimer(110);
	CLogHelper::WriteFormatLine("alloc data: %d", nfEval);

	CTimeHelper::BeginTimer(110);
    CalcGridHash(m_ParticleHash, PositionStream, posStreamSize, 
        CSetting::GetSmoothingLength(), m_Bucket_Num - 1);
    VERIFY_STREAM(m_ParticleHash);
    VERIFY_STREAM(PositionStream);

	nfEval = CTimeHelper::EndTimer(110);
	CLogHelper::WriteFormatLine("calc hash: %d", nfEval);



    CTimeHelper::BeginTimer(110);
    vector<int2> hash(m_HashSize);
    m_ParticleHash.write(&hash[0]);
    sort(hash.begin(), hash.end(), Int2Greater());
    m_ParticleHash.read(&hash[0]);
    VERIFY_STREAM(m_ParticleHash);
	nfEval = CTimeHelper::EndTimer(110);
	CLogHelper::WriteFormatLine("sort hash: %d", nfEval);

	CTimeHelper::BeginTimer(110);
    //kFindCellRange.domainOffset(uint4(0, 0, 0, 0));
    //kFindCellRange.domainSize(uint4(posStreamSize, 1, 1, 1));
    kFindCellRange(m_ParticleHash, m_CellRanges);
    VERIFY_STREAM(m_CellRanges);


	nfEval = CTimeHelper::EndTimer(110);
	CLogHelper::WriteFormatLine("find range: %d", nfEval);

	nfEval = CTimeHelper::EndTimer(100);
	CLogHelper::WriteFormatLine("neighboring over all: %d", nfEval);
}

void BrookCore::Step(CSmoothedParticles *pSP, float dt)
{    
	long nfEval;
	CTimeHelper::BeginTimer(11);
	CLogHelper::WriteFormatLine("-------------------Inside Brook----------------");
	CTimeHelper::BeginTimer(10);
    float rho0 = (float)CSetting::GetFluidDensity();
    float k = (float)CSetting::GetIncompressibility();
    float h = (float)CSetting::GetSmoothingLength();

    float3 gravity = CMathHelper::DXVec3ToFloat3(CSetting::GetGravity());
    float3 sceneSize = CMathHelper::DXVec3ToFloat3(CSetting::GetSceneSize());;

    float mu = (float)CSetting::GetViscosity();
	nfEval = CTimeHelper::EndTimer(10);
	CLogHelper::WriteFormatLine("Get Parameter: %d", nfEval);

	CTimeHelper::BeginTimer(10);

    unsigned int particleNum_Dim[] = { pSP->positions.size() };
    ::brook::Stream< float3 > PositionStream(sizeof(particleNum_Dim) / sizeof(unsigned int), particleNum_Dim);
    ::brook::Stream< float3 > PositionStreamOut(sizeof(particleNum_Dim) / sizeof(unsigned int), particleNum_Dim);
    ::brook::Stream< float3 > VelocityStream(sizeof(particleNum_Dim) / sizeof(unsigned int), particleNum_Dim);
    ::brook::Stream< float3 > VelocityStreamOut(sizeof(particleNum_Dim) / sizeof(unsigned int), particleNum_Dim);
    ::brook::Stream< float3 > ForceStream(sizeof(particleNum_Dim) / sizeof(unsigned int), particleNum_Dim);
    ::brook::Stream< float > DensityStream(sizeof(particleNum_Dim) / sizeof(unsigned int), particleNum_Dim);
    ::brook::Stream< float > PressureStream(sizeof(particleNum_Dim) / sizeof(unsigned int), particleNum_Dim);

    PositionStream.read(&(pSP->positions[0]));
    VelocityStream.read(&(pSP->velocities[0]));

	nfEval = CTimeHelper::EndTimer(10);
	CLogHelper::WriteFormatLine("Init & read data: %d", nfEval);

    for (int i = 0; i < 4; i++)
    {
        CTimeHelper::BeginTimer(10);
        NeighborFinding(PositionStream, pSP->positions.size());
        nfEval = CTimeHelper::EndTimer(10);
        CLogHelper::WriteFormatLine("Neighbor Finding: %d", nfEval);

        CTimeHelper::BeginTimer(10);

        nComputeDensities(DensityStream, PositionStream, m_ParticleHash, m_CellRanges, h, m_Bucket_Num - 1);
        nComputePressures(DensityStream, PressureStream, rho0, k);
        nComputeForces(ForceStream, PressureStream, PositionStream, DensityStream, VelocityStream, m_ParticleHash, m_CellRanges, gravity, h, mu, m_Bucket_Num - 1);
        nUpdateParticles(PositionStreamOut, VelocityStreamOut, PositionStream, VelocityStream, ForceStream, dt / 4, sceneSize);
        
        nfEval = CTimeHelper::EndTimer(10);
        CLogHelper::WriteFormatLine("Solving Equation: %d", nfEval);

        ::brook::Stream<float3> tmpPos = PositionStreamOut;
        PositionStreamOut = PositionStream;
        PositionStream = tmpPos;

        ::brook::Stream<float3> tmpVel = VelocityStreamOut;
        VelocityStreamOut = VelocityStream;
        VelocityStream = tmpVel;
    }

    //Timer time;
    //CLogHelper::EnableLogging(true);
    //time.Reset();
    //time.Start();
    //PositionStream.write(&pSP->positions[0]);
    //time.Stop();
    //CLogHelper::WriteFormatLine("Write %d Data: %lf", pSP->GetParticleNum(), time.GetElapsedTime());
    //CLogHelper::EnableLogging(false);

    //////////////////////////////////////////////////////////////////////////
    // Write Data
	CTimeHelper::BeginTimer(10);
    VelocityStream.write(&pSP->velocities[0]);
    PositionStream.write(&pSP->positions[0]);
    DensityStream.write(&pSP->densities[0]);
	nfEval = CTimeHelper::EndTimer(10);
	CLogHelper::WriteFormatLine("Write Data: %d", nfEval);


	nfEval = CTimeHelper::EndTimer(11);
	CLogHelper::WriteFormatLine("Overall: %d", nfEval);
}