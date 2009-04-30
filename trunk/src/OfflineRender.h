#pragma once
#include "Setting.h"
#include "AviSurfaceWriter.h"
#include "SDKmisc.h"

class CPushSourceSurface;
class COfflineRender
{
public:
    COfflineRender(void);
    ~COfflineRender(void);

    void Destroy(void);

    HRESULT Initialize(const UINT Width, const UINT Height);
    HRESULT BeginRender(void);
    HRESULT OfflineMove(void);
    HRESULT EndRender(void);
private:
    LPDIRECT3DSURFACE9      m_pRenderTargetSurface;
    LPDIRECT3DSURFACE9      m_pOldRenderTargetSurface;
    LPDIRECT3DSURFACE9      m_pDepthStencilSurface;
    LPDIRECT3DSURFACE9      m_pOldDepthStencilSurface;

    UINT                    m_Height;
    UINT                    m_Width;
    CAviSurfaceWriter       m_SurfaceWriter;
    float                   m_totalTime;

    CDXUTTextHelper*                      m_pTxtHelper;
    // Direct3D 9 resources
    ID3DXFont*                            m_pFont9;
    ID3DXSprite*                          m_pSprite9;
};
