#pragma once
#include "IMeshGenerator.h"
#include <vector>
#include <map>

using namespace std;

class CMarchingCubeMeshConstructor : IMeshConstructor
{
public:
    CMarchingCubeMeshConstructor(void);
    ~CMarchingCubeMeshConstructor(void);
public:
    void ConstructSurface(const CSmoothedParticles &sParticles);
    D3DXVECTOR3 GetMaxBound(void);
    D3DXVECTOR3 GetMinBound(void);

    vector<D3DXVECTOR3> m_Vertices;
    vector<unsigned int>        m_Indices;
    vector<D3DXVECTOR3> m_Normals;

    vector<D3DXVECTOR3> m_GridPoints;
protected:
    void ConstructDistanceField(const CSmoothedParticles &sParticles);
    void DestroyBuffers(void);
    void Rasterize(const CSmoothedParticles &sParticles, unsigned int index);
    void GenerateSurface(void);
    D3DXVECTOR3 IntersectPoint(unsigned int x, unsigned int y, unsigned int z, unsigned int nEdgeNo);
    D3DXVECTOR3 Interpolate(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2, float val1, float val2);
    void UpdateIndices();
    void UpdateNormals();
    
    int GetEdgeID(unsigned int i, unsigned int j, unsigned int k, unsigned int nEdgeNo);
    unsigned int GetVertexID(unsigned int i, unsigned int j, unsigned int k);
private:
    void ComputeBoundingBox(const CSmoothedParticles &sParticles);
    void InitializeDistanceField(void);

    D3DXVECTOR3 m_MaxBound;
    D3DXVECTOR3 m_MinBound;
    float m_IsoLevel;

    D3DXVECTOR3 m_CellH;

    float *m_pScalarValues;


    map<unsigned int, unsigned int> m_EdgeID2Indices;

    static const unsigned int m_EdgeTable[256];
    static const int m_Faces[256][16];
    int m_Cx; // Cube number in x direction
    int m_Cy; // Cube number in y direction
    int m_Cz; // Cube nunber in z direction
};
