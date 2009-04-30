#pragma once
#include "stdafx.h"
#include "Simulator.h"
#include "IGraphicObject.h"

class CGraphicEngine
{
public:
    CGraphicEngine(void);
    ~CGraphicEngine(void);

public:
    void            Initialize(void);
    int             StartLoop(void);
    void            Destroy(void);

    static void     AddGraphicObject(IGraphicObject* pGraphicObject);
    static void     RemoveGraphicObejct(IGraphicObject* pGraphicObject);
    static CBaseCamera*    GetCamera();
    void            ToggleWireFrameMode();
protected:
    void            SetupDXUTCallBacks();

    //CallBack Functions
    static LRESULT CALLBACK MsgProcCB( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );
    static void    CALLBACK OnKeyboardCB( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
    static void    CALLBACK OnGUIEventCB( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
    static void    CALLBACK OnFrameMoveCB( double fTime, float fElapsedTime, void* pUserContext );
    static bool    CALLBACK ModifyDeviceSettingsCB( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
    static bool    CALLBACK IsD3D9DeviceAcceptableCB( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
    static HRESULT CALLBACK OnD3D9CreateDeviceCB( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
    static HRESULT CALLBACK OnD3D9ResetDeviceCB( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
    static void    CALLBACK OnD3D9FrameRenderCB( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
    static void    CALLBACK OnD3D9LostDeviceCB( void* pUserContext );
    static void    CALLBACK OnD3D9DestroyDeviceCB( void* pUserContext );

    LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing);
    void    OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown);
    void    OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl);
    void    OnFrameMove( double fTime, float fElapsedTime);
    bool    ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings);
    bool    IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed);
    HRESULT OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc);
    HRESULT OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc);
    void    OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime);
    void    OnD3D9LostDevice();
    void    OnD3D9DestroyDevice();
    void    InitApp();
    void    RenderText();
    void    RenderFrameRate(float);

    void    ParseCmdLine(void);
protected:
    static std::vector<IGraphicObject*>    st_GraphicObjects;

    // Variables
    static CFirstPersonCamera             m_Camera;               // A model viewing camera
    CDXUTDialogResourceManager            m_DialogResourceManager; // manager for shared resources of dialogs
    CD3DSettingsDlg                       m_SettingsDlg;          // Device settings dialog
    CDXUTTextHelper*                      m_pTxtHelper;
    CDXUTDialog                           m_HUD;                  // dialog for standard controls
    CDXUTDialog                           m_StartupUI;             // dialog for sample specific controls

    // Global Simulator Instance
    CSimulator                            m_Sim;
    COfflineRender                        m_OffEnv;

    // Direct3D 9 resources
    ID3DXFont*                            m_pFont9;
    ID3DXSprite*                          m_pSprite9;
    bool                                  m_IsWireFrame;
	bool								  m_bInitialized;
};
