#pragma once
#include "stdafx.h"
#include <time.h>

class CRandomHelper
{
private:
    CRandomHelper(void){};
    ~CRandomHelper(void){};
public:
    inline static void Initialize()
    {
        srand(static_cast<unsigned int>(time(NULL))); // seed the rand number
    }

    inline static D3DCOLOR RandomColor()
    {
        return D3DCOLOR_ARGB(rand()%256, rand()%256, rand()%256, rand()%256);
    }

    inline static D3DCOLOR RandomColorNoA()
    {
        return D3DCOLOR_ARGB(255, rand()%256, rand()%256, rand()%256);
    }

    inline static FLOAT RandomFloat(FLOAT minNum, FLOAT maxNum)
    {
        if (minNum > maxNum)
        {
            float t = minNum;
            minNum = maxNum;
            maxNum = t;
        }

        return (FLOAT)RandomDouble() * (maxNum - minNum) + minNum;
    }

    inline static D3DXVECTOR3 RandomVector3(FLOAT minNum, FLOAT maxNum)
    {
        return D3DXVECTOR3(
            RandomFloat(minNum, maxNum),
            RandomFloat(minNum, maxNum),
            RandomFloat(minNum, maxNum)
            );
    }

    inline static DOUBLE RandomDouble()
    {
        return rand() / (DOUBLE)RAND_MAX;
    }
};
