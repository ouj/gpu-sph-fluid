#pragma once
#include "stdafx.h"
#include "brook/brook.h"

class CMathHelper
{
private:
    CMathHelper(void){}
public:
    inline static void VectorCeil(D3DXVECTOR3 &vec)
    {
        vec.x = ceil(vec.x);
        vec.y = ceil(vec.y);
        vec.z = ceil(vec.z);
    }

    inline static void VectorFloor(D3DXVECTOR3 &vec)
    {
        vec.x = floor(vec.x);
        vec.y = floor(vec.y);
        vec.z = floor(vec.z);
    }

    inline static float3 DXVec3ToFloat3(const D3DXVECTOR3 &vec)
    {
        float3 f;
        f.x = vec.x;
        f.y = vec.y;
        f.z = vec.z;
        return f;
    }

    inline static float SquaredDistance(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2)
    {
        float dx = vec2.x - vec1.x;
        float dy = vec2.y - vec1.y;
        float dz = vec2.z - vec1.z;
        return dx * dx + dy * dy + dz * dz;
    }

    inline static float Distance(const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2)
    {
        return sqrt(SquaredDistance(vec1, vec2));
    }
};
