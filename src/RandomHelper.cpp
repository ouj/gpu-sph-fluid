#include "dxut.h"
#include "RandomHelper.h"
#include <time.h>

void CRandomHelper::Initialize()
{
    srand(static_cast<UINT>(time(NULL))); // seed the rand number
}

D3DCOLOR CRandomHelper::RandomColor()
{
    return D3DCOLOR_ARGB(rand()%256, rand()%256, rand()%256, rand()%256);
}

D3DCOLOR CRandomHelper::RandomColorNoA()
{
    return D3DCOLOR_ARGB(255, rand()%256, rand()%256, rand()%256);
}

FLOAT CRandomHelper::RandomFloat(FLOAT minNum, FLOAT maxNum)
{
    if (minNum > maxNum)
    {
        float t = minNum;
        minNum = maxNum;
        maxNum = t;
    }
    
    return (FLOAT)RandomDouble() * (maxNum - minNum) + minNum;
}

DOUBLE CRandomHelper::RandomDouble()
{
    return rand() / (DOUBLE)RAND_MAX;
}

D3DXVECTOR3 CRandomHelper::RandomVector3(FLOAT minNum, FLOAT maxNum)
{
    return D3DXVECTOR3(
        RandomFloat(minNum, maxNum),
        RandomFloat(minNum, maxNum),
        RandomFloat(minNum, maxNum)
        );
}
