#include "stdafx.h"
#include "ParticleRenderer.h"
#include "GraphicEngine.h"
#include "RandomHelper.h"



CParticleRenderer::CParticleRenderer(void)
{
    m_pEffect = NULL;
    m_hTechnique = NULL;
    m_pDiffuseTexture = NULL;
    m_VBSize = 2000;
    m_VBBatchSize = 500;
    m_VBOffSet = 0;
}

CParticleRenderer::~CParticleRenderer(void)
{
}


// A structure for our custom vertex type. We added texture coordinates
struct ParticleVertex
{
    D3DVECTOR    position; // The position
    D3DCOLOR    color;
    FLOAT        tu, tv;   // The texture coordinates
};

static const D3DVERTEXELEMENT9 g_Decl[] = 
{
    { 0,  0,                 D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0,  sizeof(D3DVECTOR), D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
    { 0,  sizeof(D3DVECTOR) + sizeof(D3DCOLOR), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    D3DDECL_END()
};


HRESULT CParticleRenderer::Create()
{
    HRESULT hr = LoadEffect();
    assert(SUCCEEDED(hr));
    if(FAILED(hr))
    {
        DXUT_ERR_MSGBOX(TEXT("Failed to load effect!"), hr);
        return hr;
    }

    hr = CreateVB();
    assert(SUCCEEDED(hr));
    if(FAILED(hr))
        return hr;

    CGraphicEngine::AddGraphicObject(this);

    return S_OK;
}


HRESULT CParticleRenderer::LoadEffect()
{
    HRESULT hr = S_OK;
    const TCHAR effectFileName[] = TEXT("Shaders\\Particle.fx");

    TCHAR filePath[MAX_PATH];

    // Create the mesh texture from a file
    V_RETURN( DXUTFindDXSDKMediaFileCch(filePath, MAX_PATH, effectFileName ));

    // Create the effect
    DWORD dwShaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
    dwShaderFlags |= D3DXSHADER_DEBUG;
#endif

    V_RETURN(D3DXCreateEffectFromFile( DXUTGetD3D9Device(), filePath, NULL, NULL, 
        dwShaderFlags, NULL, &m_pEffect, NULL));

    // Obtain the technique
    m_hTechnique = m_pEffect->GetTechniqueByName( "ScreenNoZ" );

    m_hDiffuseTexture = m_pEffect->GetParameterByName(NULL, "ColorTexture");
    m_hWorldViewProj = m_pEffect->GetParameterByName(NULL, "WvpXf");

    const TCHAR textureFileName[] = TEXT("Resource\\Particle.dds");    
    // Create the mesh texture from a file
    V_RETURN( DXUTFindDXSDKMediaFileCch(filePath, MAX_PATH, textureFileName ));

    V_RETURN( D3DXCreateTextureFromFile( DXUTGetD3D9Device(), filePath, &m_pDiffuseTexture ) );

    return S_OK;
}

HRESULT CParticleRenderer::CreateVB()
{
    HRESULT hr = S_OK;

    V_RETURN(DXUTGetD3D9Device()->CreateVertexDeclaration(g_Decl, &m_pDecl));

    V_RETURN(DXUTGetD3D9Device()->CreateVertexBuffer(m_VBSize * sizeof(ParticleVertex), 
        D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY, 
        0, D3DPOOL_MANAGED,
        &m_pVB, NULL));

    return S_OK;
}

// IGraphicObject methods
void CParticleRenderer::OnLostDevice()
{
}
void CParticleRenderer::OnResetDevice()
{

}

void CParticleRenderer::OnDestroyDevice()
{
    SAFE_RELEASE(m_pEffect);
    SAFE_RELEASE(m_pVB);
    SAFE_RELEASE(m_pDiffuseTexture);
    SAFE_RELEASE(m_pDecl);
}// Clean up;

#define F2D(f) (*((DWORD*)&f))

HRESULT CParticleRenderer::Render(const CSmoothedParticles &sParticles)
{
    HRESULT hr = S_OK;
    
    if (!m_pEffect || !m_pVB)
    {
        return E_FAIL;
    }

    float pointSize = 100.0f;

    const D3DXMATRIX *pView;
    const D3DXMATRIX *pProj;
    D3DXMATRIX mWorldViewProjection;

    // Get the projection & view matrix from the camera class
    pProj = CGraphicEngine::GetCamera()->GetProjMatrix();
    pView = CGraphicEngine::GetCamera()->GetViewMatrix();

    mWorldViewProjection = (*pView) * (*pProj); // World matrix is identical

    V_RETURN(m_pEffect->SetMatrix(m_hWorldViewProj, &mWorldViewProjection));
    V_RETURN(m_pEffect->SetTexture(m_hDiffuseTexture, m_pDiffuseTexture));

    // Point size uses a value in FLOAT FORMAT, but it have be passed via DWORD. 
    V_RETURN(DXUTGetD3D9Device()->SetRenderState(D3DRS_POINTSPRITEENABLE,true));
    V_RETURN(DXUTGetD3D9Device()->SetRenderState(D3DRS_POINTSIZE,F2D(pointSize)));

    V_RETURN(m_pEffect->ValidateTechnique(m_hTechnique));
    V_RETURN(m_pEffect->SetTechnique(m_hTechnique));

    V_RETURN(DXUTGetD3D9Device()->SetStreamSource(0, m_pVB, 0, sizeof(ParticleVertex)));
    V_RETURN(DXUTGetD3D9Device()->SetVertexDeclaration(m_pDecl));
    

    if (m_VBOffSet >= m_VBSize)
        m_VBOffSet = 0;

    ParticleVertex* vertices;
    V_RETURN(m_pVB->Lock(m_VBOffSet * sizeof(ParticleVertex),
        m_VBBatchSize * sizeof(ParticleVertex), 
        (void**)&vertices, m_VBOffSet ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD));

    DWORD numParInBatch = 0;
    for (unsigned int i = 0; i < sParticles.GetParticleNum(); i++)
    {
        vertices->position = sParticles.positions[i];
        vertices->color = D3DCOLOR_RGBA(64, 128, 128, 128);
        vertices++;
        numParInBatch++;

        // It is full
        if (numParInBatch == m_VBBatchSize)
        {
            V_RETURN(m_pVB->Unlock());

            unsigned int cPasses = 0;
            V( m_pEffect->Begin( &cPasses, 0 ) );
            for( unsigned int iPass = 0; iPass < cPasses; iPass++ )
            {
                V( m_pEffect->BeginPass( iPass ) );
                V_RETURN(DXUTGetD3D9Device()->DrawPrimitive(D3DPT_POINTLIST, m_VBOffSet, numParInBatch));
                V( m_pEffect->EndPass() );
            }
            V( m_pEffect->End() );

            m_VBOffSet += m_VBBatchSize;

            if(m_VBOffSet >= m_VBSize)
                m_VBOffSet = 0;

            V_RETURN(m_pVB->Lock(m_VBOffSet * sizeof(ParticleVertex),
                m_VBBatchSize * sizeof(ParticleVertex), 
                (void**)&vertices, m_VBOffSet ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD));

            numParInBatch = 0;
        }
    }

    V_RETURN(m_pVB->Unlock());

    if (numParInBatch)
    {
        unsigned int cPasses = 0;
        V( m_pEffect->Begin( &cPasses, 0 ) );
        for( unsigned int iPass = 0; iPass < cPasses; iPass++ )
        {
            V( m_pEffect->BeginPass( iPass ) );
            V_RETURN(DXUTGetD3D9Device()->DrawPrimitive(D3DPT_POINTLIST, m_VBOffSet, numParInBatch));
            V( m_pEffect->EndPass() );
        }
        V( m_pEffect->End() );
    }

    m_VBOffSet += m_VBBatchSize;
    
    DXUTGetD3D9Device()->SetRenderState(D3DRS_POINTSPRITEENABLE,FALSE);

    return hr;
}