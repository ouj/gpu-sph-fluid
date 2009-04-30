#pragma once
#include "stdafx.h"
#include "igraphicobject.h"
#include "SmoothedParticles.h"

class CParticleRenderer :
    public IGraphicObject
{
public:
    CParticleRenderer(void);
    virtual ~CParticleRenderer(void);

    HRESULT Create();
    HRESULT Render(const CSmoothedParticles &sParticles);

protected:
    HRESULT CreateVB();
    HRESULT LoadEffect();
public:
    // IGraphicObject methods
    void virtual OnLostDevice();
    void virtual OnResetDevice();

    void virtual OnDestroyDevice(); // Clean up;
protected:
    //Variables
    LPD3DXEFFECT                    m_pEffect;
    D3DXHANDLE                      m_hTechnique;
    D3DXHANDLE                      m_hDiffuseTexture;
    D3DXHANDLE                      m_hWorldViewProj;
    LPDIRECT3DVERTEXBUFFER9         m_pVB; // Buffer to hold vertices

    LPDIRECT3DVERTEXDECLARATION9    m_pDecl;
    LPDIRECT3DTEXTURE9              m_pDiffuseTexture;
    DWORD                           m_VBSize;
private:
    DWORD                           m_VBBatchSize;
    DWORD                           m_VBOffSet;
};
