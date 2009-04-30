#pragma once
#include "stdafx.h"
using namespace std;

class CPoint3DHash
{
public:
    CPoint3DHash(void);
    ~CPoint3DHash(void);
    
public:
    void Clear();
    void ConstructHash(D3DXVECTOR3 *positions, unsigned int nSize, float h);
    void FindNeighbor(D3DXVECTOR3 *positions, unsigned int idx, float h, vector<int>& npIndices);

    void AddPointsOfKey( int hashKey, unsigned int idx, D3DXVECTOR3 * positions, 
        float h, vector<int> &npIndices );
 
private:
    int HashFunction( float fx, float fy, float fz, float h );
    static const int HASH_SIZE = 1000;
    vector<int> m_pointHash[HASH_SIZE * 2];
};
