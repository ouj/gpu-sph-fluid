#pragma once
#include "IGraphicObject.h"

class CSmoothedParticles;
class CFluidSource :
    public IGraphicObject
{
public:
    CFluidSource(void);

    ~CFluidSource(void);

    HRESULT Create();

public:
    void virtual OnLostDevice();
    void virtual OnResetDevice();

    void virtual OnDestroyDevice(); // Clean up;

    HRESULT Render();
    const D3DXVECTOR3& GetPosition() const;
    void SetPosition(const D3DXVECTOR3 &val);

    float GetRate() const;
    void SetRate(const float val);

    D3DXVECTOR3 GetInitVelocity() const;
    void SetInitVelocity(const D3DXVECTOR3& vel);


    bool IsEnable() const;
    void Enable(const bool bEnable);

    bool OutletFluid(CSmoothedParticles* pSPar, float dt);
protected:
    HRESULT CreateModel();
    void DestroyBuffer();
    HRESULT LoadEffect();
    void SetEyePt(const D3DVECTOR &vEyePos);
    void SetLightDir(const D3DVECTOR &vLightDir);
    void SetWorldViewProj(const D3DXMATRIX &worldViewProj);
    void SetWorld(const D3DXMATRIX &world);
    void SetTexture(const LPDIRECT3DCUBETEXTURE9 pTex);
private:
    float                           m_Rate;
    bool                            m_bEnabled;

    D3DXVECTOR3                     m_Position;
    LPD3DXMESH                      m_pMesh;
    LPD3DXEFFECT                    m_pEffect;
    D3DXHANDLE                      m_hTechnique;
    D3DXHANDLE                      m_hCubeTexture;
    D3DXHANDLE                      m_hViewInverse;

    D3DXHANDLE                      m_hWorldViewProj;
    D3DXHANDLE                      m_hWorld;
    D3DXHANDLE                      m_hEyePt;
    D3DXHANDLE                      m_hLightDir;

    LPDIRECT3DCUBETEXTURE9          m_pCubeTexture;

    float                           m_Count;
    float                           m_Eval;
    D3DXVECTOR3                     m_InitVelocity;

    D3DXMATRIX                      m_ObjMatrix;
};
