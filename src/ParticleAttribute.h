#pragma once
#include "DXUT.h"
#pragma warning( push )
#pragma warning(disable:4995)
#include <vector>
#pragma warning( pop )

struct CParticleAttribute
{
    D3DXVECTOR3   position;
    D3DXVECTOR3   oposition;

    D3DXVECTOR3   velocity;
    D3DXVECTOR3   force;
    D3DXVECTOR3   pressureForce;


    FLOAT       mass;
    FLOAT       density;
    FLOAT       volumn;

    FLOAT       pressure;
    D3DCOLOR    color;

    std::vector<UINT>   m_NPointIndices;
};
