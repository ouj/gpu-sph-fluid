#include "stdafx.h"
#include "OfflineRender.h"
#include "GraphicEngine.h"

COfflineRender::COfflineRender(void)
{
    m_pRenderTargetSurface = NULL;
    m_pOldRenderTargetSurface = NULL;
    m_pOldDepthStencilSurface = NULL;
    m_pDepthStencilSurface = NULL;
}


COfflineRender::~COfflineRender(void)
{
    Destroy();
}

void COfflineRender::Destroy()
{
    SAFE_RELEASE(m_pRenderTargetSurface);
    SAFE_RELEASE(m_pOldRenderTargetSurface);

    SAFE_RELEASE(m_pDepthStencilSurface);
    SAFE_RELEASE(m_pOldDepthStencilSurface);

    SAFE_RELEASE(m_pFont9);
    SAFE_RELEASE(m_pSprite9);

    SAFE_DELETE(m_pTxtHelper);
}

HRESULT COfflineRender::Initialize(const UINT Width, const UINT Height)
{
    if (!CSetting::IsDisplayOffline())
    {
        return E_FAIL;
    }
    m_Height = Height;
    m_Width = Width;

    HRESULT hr = S_OK;

    V_RETURN(m_SurfaceWriter.InitializeVfw(CSetting::AviFilename().c_str(), (UINT)(1 / CSetting::GetTimeStep())));

    // Initialize Render Target Surface.
    SAFE_RELEASE(m_pOldRenderTargetSurface);
    V_RETURN(DXUTGetD3D9Device()->GetRenderTarget( 0, &m_pOldRenderTargetSurface ));
    assert(m_pOldRenderTargetSurface);
    if (m_pOldRenderTargetSurface)
    {
        D3DSURFACE_DESC desc;
        m_pOldRenderTargetSurface->GetDesc( &desc );
        SAFE_RELEASE(m_pRenderTargetSurface);
        V_RETURN(DXUTGetD3D9Device()->CreateRenderTarget(m_Width, m_Height, 
            D3DFMT_X8R8G8B8, 
            desc.MultiSampleType, desc.MultiSampleQuality,
            true, &m_pRenderTargetSurface, NULL ));
    }
    SAFE_RELEASE(m_pOldRenderTargetSurface);


    // Initialize Depth Stencil Surface
    SAFE_RELEASE(m_pOldDepthStencilSurface);
    V_RETURN(DXUTGetD3D9Device()->GetDepthStencilSurface(&m_pOldDepthStencilSurface));
    assert(m_pOldDepthStencilSurface);
    if (m_pOldDepthStencilSurface)
    {
        D3DSURFACE_DESC desc;
        m_pOldDepthStencilSurface->GetDesc( &desc );
        SAFE_RELEASE(m_pDepthStencilSurface);
        V_RETURN(DXUTGetD3D9Device()->CreateDepthStencilSurface(m_Width, m_Height, 
            desc.Format,
            desc.MultiSampleType, 
            desc.MultiSampleQuality,
            true, &m_pDepthStencilSurface, NULL));
    }
    SAFE_RELEASE(m_pOldDepthStencilSurface);

    m_totalTime = 0.0f;

    V_RETURN( D3DXCreateFont( DXUTGetD3D9Device(), 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
        OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
        L"Arial", &m_pFont9 ) );

    V_RETURN( D3DXCreateSprite( DXUTGetD3D9Device(), &m_pSprite9 ) );
    m_pTxtHelper = new CDXUTTextHelper( m_pFont9, m_pSprite9, NULL, NULL, 15 );

    return hr;
}


HRESULT COfflineRender::OfflineMove(void)
{
    if (!CSetting::IsDisplayOffline())
    {
        return E_FAIL;
    }
    HRESULT hr = S_OK;
    
    CGraphicEngine::GetCamera()->FrameMove(CSetting::GetTimeStep());
    

    return hr;
}


HRESULT COfflineRender::BeginRender()
{
    if (!CSetting::IsDisplayOffline())
    {
        return E_FAIL;
    }
    HRESULT hr = S_OK;

    SAFE_RELEASE(m_pOldRenderTargetSurface);
    V_RETURN(DXUTGetD3D9Device()->GetRenderTarget( 0, &m_pOldRenderTargetSurface ));
    assert(m_pOldRenderTargetSurface);
    if (m_pOldRenderTargetSurface)
    {
        V_RETURN(DXUTGetD3D9Device()->SetRenderTarget( 0, m_pRenderTargetSurface));
    }

    SAFE_RELEASE(m_pOldDepthStencilSurface);
    V_RETURN(DXUTGetD3D9Device()->GetDepthStencilSurface(&m_pOldDepthStencilSurface));
    assert(m_pOldDepthStencilSurface);
    if (m_pOldDepthStencilSurface)
    {
        V_RETURN(DXUTGetD3D9Device()->SetDepthStencilSurface(m_pDepthStencilSurface));
    }

    return hr;
}

HRESULT COfflineRender::EndRender()
{
    if (!CSetting::IsDisplayOffline())
    {
        return E_FAIL;
    }
    HRESULT hr = S_OK;

    assert(m_pOldRenderTargetSurface);
    if (m_pOldRenderTargetSurface)
    {
        V_RETURN(DXUTGetD3D9Device()->SetRenderTarget( 0, m_pOldRenderTargetSurface));        
    }

    assert(m_pOldDepthStencilSurface);
    if (m_pOldDepthStencilSurface)
    {
        V_RETURN(DXUTGetD3D9Device()->SetDepthStencilSurface(m_pOldDepthStencilSurface));
    }

    m_totalTime += CSetting::GetTimeStep();

    if (CSetting::IsDisplayOffline())
    {
        DXUTGetD3D9Device()->StretchRect( m_pRenderTargetSurface, NULL, 
            m_pOldRenderTargetSurface, NULL, D3DTEXF_LINEAR );
    }
    else
    {
        V( DXUTGetD3D9Device()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
            D3DCOLOR_ARGB(0, 128, 128, 128), 1.0f, 0) );

        // Render the scene
        if( SUCCEEDED( DXUTGetD3D9Device()->BeginScene() ) )
        {
            m_pTxtHelper->Begin();
            m_pTxtHelper->SetInsertionPos( 5, 5 );
            TCHAR sOut[256];
            StringCchPrintf(sOut, 256, TEXT("Time passed: %.4f"), m_totalTime); 
            m_pTxtHelper->DrawTextLine(sOut);
            m_pTxtHelper->End();
            DXUTGetD3D9Device()->EndScene();
        }
    }

    assert(m_pRenderTargetSurface);
    if (m_pRenderTargetSurface)
    {
        V_RETURN(m_SurfaceWriter.AddFrame(m_pRenderTargetSurface));
    }

    return hr;
}