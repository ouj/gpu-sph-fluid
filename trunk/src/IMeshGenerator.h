#pragma once
#include "SmoothedParticles.h"

class IMeshConstructor
{
protected:
    IMeshConstructor(void)
    {
    }

    virtual ~IMeshConstructor(void)
    {
    }
public:
    virtual void ConstructSurface(const CSmoothedParticles &sParticles) = 0;
};
