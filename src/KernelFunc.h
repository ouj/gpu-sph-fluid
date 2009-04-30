#pragma once
#include "assert.h"
#define PI 3.141592654f

class CKernelFunc
{
protected:
    CKernelFunc(void) {}
    ~CKernelFunc(void) {}
public:
    static inline FLOAT SmoothingKernel(const float r, const float h)
    {
        const float q = r/h;
        if (q>1.0f) 
            return 0.0f;
        else
            return 1.0f-q*q;
    }

    static inline FLOAT SquaredStandardKernel(const float rr, const float h)
    {
        const float hh = h * h;  // h^2
        const float qq = rr / hh;  // r^2/h^2

        if (qq > 1)
            return 0;
        else
        {
            const float dd = 1 - qq;    // (h^2 - r^2) / h^2

            const float d6 = dd * dd * dd;    // (h^2 - r^2)^3 / h^6

            return 315.0f /(64.0f * PI * hh * h) * d6; // 315[(h^2 - r^2)^3] / 64 * PI * h^9;
        }
    }

    static inline FLOAT StandardKernel(const float r, const float h)
    {
        const float hh = h * h;  // h^2
        const float qq = r * r/ hh;  // r^2/h^2

        if (qq > 1)
            return 0;
        else
        {
            const float dd = 1 - qq;    // (h^2 - r^2) / h^2

            const float d6 = dd * dd * dd;    // (h^2 - r^2)^3 / h^6

            return 315.0f /(64.0f * PI * hh * h) * d6; // 315[(h^2 - r^2)^3] / 64 * PI * h^9;
        }
    }

    static inline FLOAT SpikeKernel(const float r, const float h)
    {
        assert(r >= 0);
        assert(h >= 0);
        const float q = r / h;
        if (q > 1.0f) 
            return 0.0f;
        else
        {
            const float d = 1.0f - q; // (h - r) / h
            const float hh = h * h;   // h^2

            const float ddd = d * d * d; // (h - r)^3 / h^3
            return 15.0f / (PI * hh * h) * ddd; // [15 * (h - r)^3] / Pi * h^6
        }
    }

    static inline float SpikeGradient(const float r, const float h)
    {
        float grad = 0;
        const float q = r / h;
        if (q > 1.0f)
            return grad;
        else
        {
            const float d = 1.0f - q;
            const float hh = h * h;

            grad = -45.0f / (PI * hh * hh) * d * d;
        }
        return grad;
    }

    static inline float StandardGradient(const float r, const float h) 
    {
        FLOAT grad = 0.0f;
        const float q = r / h;
        if (q > 1.0f)
            return grad;
        else
        {
            const float hh = h * h;
            const float dd = 1 - q * q;

            grad = -945.0f / (32.0f * PI * hh * hh * h) * dd * dd;
        }
        return grad;
    }

    static inline D3DXVECTOR3 SmoothedGradient(const D3DXVECTOR3 &vec, const float h) 
    {
        FLOAT grad = 0.0f;
        D3DXVECTOR3 gradVec;
        float r = D3DXVec3Length(&vec);
        const float q = r / h;
        if (q > 1.0f)
            return D3DXVECTOR3(0, 0, 0);
        else
        {
            const float hh = h*h;

            grad = 2 * r / hh;

            D3DXVec3Normalize(&gradVec, &vec);
            gradVec *= grad;
        }
        return gradVec;
    }

    static inline D3DXVECTOR3 SpikeLapalacian(const D3DXVECTOR3 &vec, const float r, const float h) 
    {
        FLOAT lap = 0.0f;
        D3DXVECTOR3 lapVec;
        const float q = r / h;
        if (q > 1.0f)
            return D3DXVECTOR3(0, 0, 0);
        else
        {
            const float hh = h * h;
            const float d = 1.0f-q;

            lap = 90.0f/(PI * hh * hh * h) * d;

            D3DXVec3Normalize(&lapVec, &vec);
            lapVec *= lap;
        }
        return lapVec;
    }

    static inline D3DXVECTOR3 SmoothedLapalacian(const D3DXVECTOR3 &vec, const float r, const float h) 
    {
        FLOAT lap = 0.0f;
        D3DXVECTOR3 lapVec;
        const float q = r / h;
        if (q > 1.0f)
            return D3DXVECTOR3(0, 0, 0);
        else
        {
            const float hh = h * h;

            lap = 1.0f - (1.0f / hh);

            D3DXVec3Normalize(&lapVec, &vec);
            lapVec *= lap;
        }
        return lapVec;
    }

    static inline float StandardLapalacian(const float r, const float h) 
    {
        FLOAT lap = 0.0f;
        const float q = r / h;
        if (q > 1.0f)
            return lap;
        else
        {
            const float dd = 1 - q * q;
            const float rr = r * r;
            const float hh = h * h;
            const float h5 = hh * hh * h;

            lap += 945.0f / (8.0f * PI * h5 * hh) * rr * dd;
            lap += -945.0f / (32.0f * PI * h5) * dd * dd;
        }
        return lap;
    }

    static inline float ViscosityLapalacian(const float r, const float h) 
    {
        FLOAT lap = 0.0f;
        const float q = r / h;
        if (q > 1.0f)
            return 0.0f;
        else
        {
            const float hh = h*h;
            const float d = 1.0f-q;

            lap = 45.0f/(PI * hh * hh * h) * d;
        }
        return lap;
    }
};
