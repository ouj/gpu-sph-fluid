#pragma once

#include "stdafx.h"
#include "GPUCore\EquationSolving.h"
class CSmoothedParticles;
using namespace std;

class BrookCore
{
public:
    BrookCore(void);
    ~BrookCore(void);

    void Step(CSmoothedParticles *pSP, const float dt);
private:
    void NeighborFinding(::brook::Stream<float3> &PositionStream, int posStreamSize);

    unsigned int            m_Bucket_Num;

    int                     m_HashSize;
    ::brook::Stream<int2>   m_CellRanges;
    ::brook::Stream<int2>   m_ParticleHash;
};
