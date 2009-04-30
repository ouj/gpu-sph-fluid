#include "stdafx.h"
#include "Point3DHash.h"

CPoint3DHash::CPoint3DHash(void)
{
}

CPoint3DHash::~CPoint3DHash(void)
{
}

void CPoint3DHash::Clear()
{
    for(int i = 0; i < HASH_SIZE; i++)
    {
        m_pointHash[i].clear();
    }
}

void CPoint3DHash::ConstructHash(D3DXVECTOR3 *positions, unsigned int nSize, float h)
{
    Clear();
    for (unsigned int i = 0; i < nSize; i++)
    {
        int idx = HashFunction(positions[i].x, positions[i].y, positions[i].z, h);
        m_pointHash[idx].push_back(i);
    }
}

int CPoint3DHash::HashFunction( float fx, float fy, float fz, float h )
{
    int x = (int)(fx / h);
    int y = (int)(fy / h);
    int z = (int)(fz / h);

    int idx = (x + y + z) % HASH_SIZE;

    if (idx < 0)
    {
        idx += 2 * HASH_SIZE;
    }

    return idx;
}

void CPoint3DHash::FindNeighbor(D3DXVECTOR3 *positions, unsigned int idx, float h, vector<int>& npIndices)
{
    D3DXVECTOR3 &pos = positions[idx];
    int hashKey;
    hashKey = HashFunction(pos.x, pos.y, pos.z, h);
    AddPointsOfKey(hashKey, idx, positions, h, npIndices);
    
    hashKey = HashFunction(pos.x - h, pos.y, pos.z, h);
    AddPointsOfKey(hashKey, idx, positions, h, npIndices);

    hashKey = HashFunction(pos.x + h, pos.y, pos.z, h);
    AddPointsOfKey(hashKey, idx, positions, h, npIndices);

    hashKey = HashFunction(pos.x, pos.y - h, pos.z, h);
    AddPointsOfKey(hashKey, idx, positions, h, npIndices);

    hashKey = HashFunction(pos.x, pos.y + h, pos.z, h);
    AddPointsOfKey(hashKey, idx, positions, h, npIndices);

    hashKey = HashFunction(pos.x, pos.y, pos.z - h, h);
    AddPointsOfKey(hashKey, idx, positions, h, npIndices);

    hashKey = HashFunction(pos.x, pos.y, pos.z + h, h);
    AddPointsOfKey(hashKey, idx, positions, h, npIndices);

    hashKey = HashFunction(pos.x - h, pos.y - h, pos.z, h);
    AddPointsOfKey(hashKey, idx, positions, h, npIndices);

    hashKey = HashFunction(pos.x + h, pos.y + h, pos.z, h);
    AddPointsOfKey(hashKey, idx, positions, h, npIndices);

    hashKey = HashFunction(pos.x, pos.y - h, pos.z - h, h);
    AddPointsOfKey(hashKey, idx, positions, h, npIndices);

    hashKey = HashFunction(pos.x, pos.y + h, pos.z + h, h);
    AddPointsOfKey(hashKey, idx, positions, h, npIndices);

    hashKey = HashFunction(pos.x - h, pos.y, pos.z - h, h);
    AddPointsOfKey(hashKey, idx, positions, h, npIndices);

    hashKey = HashFunction(pos.x + h, pos.y, pos.z + h, h);
    AddPointsOfKey(hashKey, idx, positions, h, npIndices);

    hashKey = HashFunction(pos.x - h, pos.y - h, pos.z - h, h);
    AddPointsOfKey(hashKey, idx, positions, h, npIndices);

    hashKey = HashFunction(pos.x + h, pos.y + h, pos.z + h, h);
    AddPointsOfKey(hashKey, idx, positions, h, npIndices);
}

void CPoint3DHash::AddPointsOfKey( int hashKey, unsigned int idx, D3DXVECTOR3 * positions, 
                     float h, vector<int> &npIndices )
{
    D3DXVECTOR3 &pos = positions[idx];
    vector<int>::iterator it = m_pointHash[hashKey].begin();
    while (it != m_pointHash[hashKey].end())
    {
        int nIdx = *it;
        if (idx != nIdx)
        {
            D3DXVECTOR3 &nPos = positions[nIdx];
            if (fabs(nPos.x - pos.x) < h && 
                fabs(nPos.y - pos.y) < h &&
                fabs(nPos.z - pos.z) < h)
            {
                npIndices.push_back(*it);
            }
        }
        it++;
    }
}