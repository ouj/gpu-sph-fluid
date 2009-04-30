#include "stdafx.h"
#include "FluidSource.h"
#include "GraphicEngine.h"
#include "SmoothedParticles.h"
#include "RandomHelper.h"

CFluidSource::CFluidSource()
{
    m_pMesh = NULL;
    m_pEffect = NULL;
    m_pCubeTexture = NULL;
    m_bEnabled = false;
    m_Rate = 0.18f;

    SetInitVelocity(D3DXVECTOR3(0.0f, -9.0f, 0.0f));
    m_Count = 0.0f;
}

CFluidSource::~CFluidSource(void)
{
    DestroyBuffer();
    SAFE_RELEASE(m_pEffect);
}

//////////////////////////////////////////////////////////////////////////
// IGraphicObject Methods
void CFluidSource::OnLostDevice()
{
    
}

void CFluidSource::OnResetDevice()
{
	HRESULT hr = S_OK;
    V(CreateModel());
	if (!m_pEffect)
	{
		V(LoadEffect());
	}
}

void CFluidSource::OnDestroyDevice()
{
    DestroyBuffer();
    SAFE_RELEASE(m_pEffect);
}// Clean up;

HRESULT CFluidSource::Create()
{
    HRESULT hr = S_OK;
    V_RETURN(CreateModel());
    V_RETURN(LoadEffect());

    CGraphicEngine::AddGraphicObject(this);
    return hr;
}

void CFluidSource::DestroyBuffer()
{
    SAFE_RELEASE(m_pMesh);
    SAFE_RELEASE(m_pCubeTexture);
}

HRESULT CFluidSource::CreateModel()
{
    DestroyBuffer();
    WCHAR str[MAX_PATH];
    HRESULT hr = S_OK;
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, TEXT("Resource\\FluidSource.x")));

    V_RETURN(D3DXLoadMeshFromX(str, D3DXMESH_MANAGED, DXUTGetD3D9Device(), NULL,
        NULL, NULL, NULL, &m_pMesh));

    const TCHAR textureFileName[] = TEXT("Resource\\cubemap_inclouds.dds");    
    // Create the mesh texture from a file
    V_RETURN( DXUTFindDXSDKMediaFileCch(str, MAX_PATH, textureFileName ));

    V_RETURN( D3DXCreateCubeTextureFromFile( DXUTGetD3D9Device(), str, &m_pCubeTexture ));

    return hr;
}

void CFluidSource::SetEyePt(const D3DVECTOR &vEyePt)
{
    if (m_pEffect)
    {
        D3DXVECTOR4 eyePt = D3DXVECTOR4(vEyePt, 1);
        m_pEffect->SetVector(m_hEyePt, &eyePt);
    }
}

void CFluidSource::SetLightDir(const D3DVECTOR &vLightDir)
{
    if (m_pEffect)
    {
        D3DXVECTOR4 lightDir(vLightDir, 0);
        m_pEffect->SetVector(m_hLightDir, &lightDir);
    }
}

void CFluidSource::SetWorldViewProj(const D3DXMATRIX &worldViewProj)
{
    if (m_pEffect)
    {
        m_pEffect->SetMatrix(m_hWorldViewProj, &worldViewProj);
    }
}

void CFluidSource::SetWorld(const D3DXMATRIX &world)
{
    if (m_pEffect)
    {
        m_pEffect->SetMatrix(m_hWorld, &world);
    }
}


void CFluidSource::SetTexture(const LPDIRECT3DCUBETEXTURE9 pTex)
{
    if (m_pEffect)
    {
        m_pEffect->SetTexture(m_hCubeTexture, pTex);
    }
}

const D3DXVECTOR3& CFluidSource::GetPosition() const
{ 
    return m_Position; 
}

void CFluidSource::SetPosition(const D3DXVECTOR3 &val) 
{ 
    m_Position = val;

    D3DXVECTOR3 sceneSize = CSetting::GetSceneSize();
    sceneSize /= 2;

    const float kPushOutFactor = 10.0f;// Push out

    if (m_Position.y <= -(sceneSize.y))
    {
        m_Position.y = -(sceneSize.y) + kPushOutFactor;
    }
    if(m_Position.y >= (sceneSize.y))
    {
        m_Position.y = (sceneSize.y) - kPushOutFactor;
    }
    if (m_Position.x <= -(sceneSize.x))
    {
        m_Position.x = -(sceneSize.x) + kPushOutFactor;
    }
    if (m_Position.x >= (sceneSize.x))
    {
        m_Position.x = (sceneSize.x) - kPushOutFactor;
    }
    if (m_Position.z <= -(sceneSize.z))
    {
        m_Position.z = -(sceneSize.z) + kPushOutFactor;
    }
    if( m_Position.z >= (sceneSize.z))
    {
        m_Position.z = (sceneSize.z) - kPushOutFactor;
    }
}

float CFluidSource::GetRate() const
{
    return m_Rate;
}

void CFluidSource::SetRate(const float val)
{
    if (val >= 0)
    {
        m_Rate = val;
    }
}


D3DXVECTOR3 CFluidSource::GetInitVelocity() const
{
    return m_InitVelocity;
}

void CFluidSource::SetInitVelocity(const D3DXVECTOR3& vel)
{
    

    m_InitVelocity = vel;
    float rr = D3DXVec3Dot(&m_InitVelocity, &m_InitVelocity);
    float r = sqrt(rr);
    m_Eval = CSetting::GetSmoothingLength() / r;
}


bool CFluidSource::OutletFluid(CSmoothedParticles* pSPar, float dt)
{
    if (!m_bEnabled)
    {
        return true;
    }

    m_Count += dt;
    if (m_Count > m_Eval)
    {
        m_Count = 0.0f;
        int parNum = 4;

        for (int i = -4; i < parNum; i+= 2)
        {
            for (int j = -2; j < parNum; j+= 2)
            {
                D3DXVECTOR3 position = 
                    D3DXVECTOR3(m_Position.x + static_cast<float>(i) + CRandomHelper::RandomFloat(-0.2f, 0.2f), 
                    m_Position.y + i + j, 
                    m_Position.z + static_cast<float>(j) + CRandomHelper::RandomFloat(-0.2f, 0.2f));

                pSPar->AddParticle(position, m_InitVelocity);
            }
        }
    }
    return true;
}

bool CFluidSource::IsEnable() const
{
    return m_bEnabled;
}
void CFluidSource::Enable(const bool bEnable)
{
    m_bEnabled = bEnable;
}

HRESULT CFluidSource::Render()
{
    HRESULT hr = S_OK;

    UINT cPasses;

    D3DXMATRIX mWorld;
    //D3DXMatrixIdentity(&mWorld);
    D3DXMatrixTranslation(&mWorld, m_Position.x, m_Position.y, m_Position.z);

    const D3DXMATRIX *pView;
    const D3DXMATRIX *pProj;
    D3DXMATRIX mWorldViewProjection;

    // Get the projection & view matrix from the camera class
    pProj = CGraphicEngine::GetCamera()->GetProjMatrix();
    pView = CGraphicEngine::GetCamera()->GetViewMatrix();

    const D3DVECTOR *pEyePt = CGraphicEngine::GetCamera()->GetEyePt();
    SetEyePt(*pEyePt);

    mWorldViewProjection = mWorld * (*pView) * (*pProj); // World matrix is identical
    SetWorldViewProj(mWorldViewProjection);
    SetWorld(mWorld);

    SetTexture(m_pCubeTexture);

    // Render the scene with this technique
    // as defined in the .fx file
    V( m_pEffect->SetTechnique( m_hTechnique ) );

    V( m_pEffect->Begin( &cPasses, 0 ) );
    for( UINT iPass = 0; iPass < cPasses; iPass++ )
    {
        V_RETURN( m_pEffect->BeginPass( iPass ) );
        V_RETURN(m_pMesh->DrawSubset(0));
        V_RETURN( m_pEffect->EndPass() );
    }
    V( m_pEffect->End() );


    return hr;
}

HRESULT CFluidSource::LoadEffect()
{
    HRESULT hr = S_OK;
    const TCHAR effectFileName[] = TEXT("Shaders\\SurfaceShader.fx");

    TCHAR filePath[MAX_PATH];

    // Create the mesh texture from a file
    V_RETURN( DXUTFindDXSDKMediaFileCch(filePath, MAX_PATH, effectFileName ));

    // Create the effect
    DWORD dwShaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
    dwShaderFlags |= D3DXSHADER_DEBUG;
#endif

    LPD3DXBUFFER pBuffer = NULL;
    V(D3DXCreateEffectFromFile( DXUTGetD3D9Device(), filePath, NULL, NULL, 
        dwShaderFlags, NULL, &m_pEffect, &pBuffer));
    if (FAILED(hr))
    {
        //char *p = (char*)pBuffer->GetBufferPointer();
        return hr;
    }
    SAFE_RELEASE(pBuffer);

    // Obtain the technique
    m_hTechnique = m_pEffect->GetTechniqueByName( "TeapotShading" );
    m_hWorldViewProj = m_pEffect->GetParameterByName(NULL, "WvpXf");
    m_hWorld = m_pEffect->GetParameterByName(NULL, "WXf");

    m_hEyePt = m_pEffect->GetParameterByName(NULL, "eyePt");
    m_hLightDir = m_pEffect->GetParameterByName(NULL, "lightDir");

    m_hCubeTexture = m_pEffect->GetParameterByName(NULL, "cubeTexture");

    //SetLightDir(D3DXVECTOR3(10.0f, 10.0f, 0.0f));

    return S_OK;
}