#include "stdafx.h"
#include "GraphicEngine.h"
#include "RandomHelper.h"
#include "LogHelper.h"
#include "StartupDlg.h"
#include "CmdLineArgs.h"
//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3
#define IDC_FILLMODEFRAME       4
#define IDC_GRAVITY_STATIC      5
#define IDC_GRAVITY             6
#define IDC_SAMPLELENGTH_STATIC 71
#define IDC_SAMPLELENGTH        7
#define IDC_USEBROOK            8
#define IDC_SURFACERENDER       9

std::vector<IGraphicObject*>    CGraphicEngine::st_GraphicObjects;
CFirstPersonCamera              CGraphicEngine::m_Camera;

CGraphicEngine::CGraphicEngine(void)
{
    SetupDXUTCallBacks();
}

CGraphicEngine::~CGraphicEngine(void)
{
    DXUTShutdown(); 
    //Shutdown DXUT manually to force the destroy process to do before the class is destroyed.
}

void CGraphicEngine::SetupDXUTCallBacks()
{
    // Set DXUT callbacks
    DXUTSetCallbackMsgProc( MsgProcCB, this);
    DXUTSetCallbackKeyboard( OnKeyboardCB, this);
    DXUTSetCallbackFrameMove( OnFrameMoveCB, this );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettingsCB, this );

    DXUTSetCallbackD3D9DeviceAcceptable( IsD3D9DeviceAcceptableCB, this );
    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDeviceCB, this );
    DXUTSetCallbackD3D9DeviceReset( OnD3D9ResetDeviceCB, this );
    DXUTSetCallbackD3D9DeviceLost( OnD3D9LostDeviceCB, this );
    DXUTSetCallbackD3D9DeviceDestroyed( OnD3D9DestroyDeviceCB, this );
    DXUTSetCallbackD3D9FrameRender( OnD3D9FrameRenderCB, this );
}


//////////////////////////////////////////////////////////////////////////
// Call Back functions, redirect the call
#if TRUE //Just use this to collapse the code in editor
LRESULT CALLBACK CGraphicEngine::MsgProcCB( HWND hWnd, UINT uMsg, WPARAM wParam, 
                                           LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
    CGraphicEngine* pEngine = reinterpret_cast<CGraphicEngine*>(pUserContext);
    return pEngine->MsgProc(hWnd, uMsg, wParam, lParam, pbNoFurtherProcessing);
}
void CALLBACK CGraphicEngine::OnKeyboardCB( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
    CGraphicEngine* pEngine = reinterpret_cast<CGraphicEngine*>(pUserContext);
    return pEngine->OnKeyboard(nChar, bKeyDown, bAltDown);
}
void CALLBACK CGraphicEngine::OnGUIEventCB( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    CGraphicEngine* pEngine = reinterpret_cast<CGraphicEngine*>(pUserContext);
    return pEngine->OnGUIEvent(nEvent, nControlID, pControl);
}

bool CALLBACK CGraphicEngine::IsD3D9DeviceAcceptableCB( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                                     D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    CGraphicEngine* pEngine = reinterpret_cast<CGraphicEngine*>(pUserContext);
    return pEngine->IsD3D9DeviceAcceptable(pCaps, AdapterFormat, BackBufferFormat, bWindowed);
}
bool CALLBACK CGraphicEngine::ModifyDeviceSettingsCB( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    CGraphicEngine* pEngine = reinterpret_cast<CGraphicEngine*>(pUserContext);
    return pEngine->ModifyDeviceSettings(pDeviceSettings);
}
HRESULT CALLBACK CGraphicEngine::OnD3D9CreateDeviceCB( IDirect3DDevice9* pd3dDevice, 
                                                    const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    CGraphicEngine* pEngine = reinterpret_cast<CGraphicEngine*>(pUserContext);
    return pEngine->OnD3D9CreateDevice(pd3dDevice, pBackBufferSurfaceDesc);
}
HRESULT CALLBACK CGraphicEngine::OnD3D9ResetDeviceCB( IDirect3DDevice9* pd3dDevice, 
                                                   const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    CGraphicEngine* pEngine = reinterpret_cast<CGraphicEngine*>(pUserContext);
    return pEngine->OnD3D9ResetDevice(pd3dDevice, pBackBufferSurfaceDesc);
}
void CALLBACK CGraphicEngine::OnFrameMoveCB( double fTime, float fElapsedTime, void* pUserContext )
{
    CGraphicEngine* pEngine = reinterpret_cast<CGraphicEngine*>(pUserContext);
    return pEngine->OnFrameMove(fTime, fElapsedTime);
}
void CALLBACK CGraphicEngine::OnD3D9FrameRenderCB( IDirect3DDevice9* pd3dDevice, double fTime, 
                                                float fElapsedTime, void* pUserContext )
{
    CGraphicEngine* pEngine = reinterpret_cast<CGraphicEngine*>(pUserContext);
    return pEngine->OnD3D9FrameRender(pd3dDevice, fTime, fElapsedTime);
}
void CALLBACK CGraphicEngine::OnD3D9LostDeviceCB( void* pUserContext )
{
    CGraphicEngine* pEngine = reinterpret_cast<CGraphicEngine*>(pUserContext);
    return pEngine->OnD3D9LostDevice();
}
void CALLBACK CGraphicEngine::OnD3D9DestroyDeviceCB( void* pUserContext )
{
    CGraphicEngine* pEngine = reinterpret_cast<CGraphicEngine*>(pUserContext);
    return pEngine->OnD3D9DestroyDevice();
}
#endif

void CGraphicEngine::ParseCmdLine(void)
{
    CmdLineArgs args;
    for (unsigned int x = 1; x < args.size(); ++x)
    {
        if (_tcscmp(args[x], TEXT("-cpu")) == 0)
        {
            CSetting::SetIsUseBrook(false);
        }
        else if (_tcscmp(args[x], TEXT("-sr")) == 0)
        {
            CSetting::SetIsSurfaceRendering(true);
        }
        else if (_tcscmp(args[x], TEXT("-mv")) == 0)
        {
            if (++x < args.size())
            {
                CSetting::SetIsOffline(true);
                CSetting::SetAviFilename(args[x]);
            }
        }
        else if (_tcscmp(args[x], TEXT("-lg")) == 0)
        {
            if (++x < args.size())
            {
                CSetting::SetIsLog(true);
                CSetting::SetLogFileName(args[x]);
            }
        }
        else if (_tcscmp(args[x], TEXT("-sd")) == 0)
        {
            CSetting::SetStartUpDialog(true);
        }
    }
}

void CGraphicEngine::Initialize(void)
{
    ParseCmdLine();

    CLogHelper::EnableLogging(CSetting::IsLog());
    CLogHelper::SetLogFile(CSetting::GetLogFileName());
    CLogHelper::WriteLine("\n------------------------------------------------------------------", false);
    CLogHelper::WriteLine("Begin Run");

    if (CSetting::IsStartUpDialog())
    {
        CStartupDlg dlg;
        dlg.Show();
    }

    InitApp();
    DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true );
    DXUTCreateWindow( L"SPH Fluid Simulation" );
    DXUTCreateDevice( true, 640, 480 );

    CRandomHelper::Initialize();
}

int CGraphicEngine::StartLoop(void)
{
    DXUTMainLoop(); // Enter into the DXUT render loop
    return DXUTGetExitCode(); 
}

void CGraphicEngine::Destroy(void)
{
    m_OffEnv.Destroy();
    CLogHelper::WriteLine("Normal End Run");
    CLogHelper::WriteLine("------------------------------------------------------------------\n\n", false);
}

// Add the graphic object to the engine

void CGraphicEngine::AddGraphicObject(IGraphicObject* pGraphicObject)
{
    st_GraphicObjects.push_back(pGraphicObject);
}

void CGraphicEngine::RemoveGraphicObejct(IGraphicObject* pGraphicObject)
{
    std::vector<IGraphicObject*>::iterator it = st_GraphicObjects.begin();
    while (it != st_GraphicObjects.end())
    {
        if (*it == pGraphicObject)
        {
            st_GraphicObjects.erase(it);
            break;
        }
        it++;
    }
}

CBaseCamera* CGraphicEngine::GetCamera()
{
    return &m_Camera;
}


//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void CGraphicEngine::InitApp()
{
    m_SettingsDlg.Init( &m_DialogResourceManager );
    m_HUD.Init( &m_DialogResourceManager );
    m_StartupUI.Init( &m_DialogResourceManager );

    m_HUD.SetCallback( OnGUIEventCB, this); 
    int iY = 10; 
    m_HUD.AddButton( IDC_TOGGLEFULLSCREEN, TEXT("Toggle full screen"), 35, iY, 125, 22 );
    m_HUD.AddButton( IDC_TOGGLEREF, TEXT("Toggle REF (F3)"), 35, iY += 24, 125, 22, VK_F3 );
    m_HUD.AddButton( IDC_CHANGEDEVICE, TEXT("Change device (F2)"), 35, iY += 24, 125, 22, VK_F2 );
    m_HUD.AddButton( IDC_FILLMODEFRAME, TEXT("Wire Frame"), 35, iY += 24, 125, 22);

    m_HUD.AddButton( IDC_SURFACERENDER, TEXT("Toggle Surface Render"), 35, iY += 24, 125, 22);

    TCHAR sz[100];
    iY += 24;
    StringCchPrintf( sz, 100, TEXT("Gravity: %f"), CSetting::GetGravity().y );
    m_HUD.AddStatic( IDC_GRAVITY_STATIC, sz, 35, iY += 24, 125, 22);
    m_HUD.AddSlider( IDC_GRAVITY, 35, iY += 24, 125, 22, 0, 300, 98, false);

    iY += 24;
    StringCchPrintf( sz, 100, TEXT("Sample Length: %.2f"), CSetting::GetSamplingLength());
    m_HUD.AddStatic( IDC_SAMPLELENGTH_STATIC, sz, 35, iY += 24, 125, 22);
    m_HUD.AddSlider( IDC_SAMPLELENGTH, 35, iY += 24, 125, 22, 50, 300, 
        (int)(CSetting::GetSamplingLength() * 100), false);


	// Title font for comboboxes
	m_StartupUI.SetFont( 1, L"Arial", 24, FW_BOLD );
	CDXUTElement* pElement = m_StartupUI.GetDefaultElement( DXUT_CONTROL_STATIC, 0 );
	if( pElement )
	{
		pElement->iFont = 1;
		pElement->dwTextFormat = DT_CENTER | DT_BOTTOM;
	}

    m_StartupUI.SetCallback( OnGUIEventCB, this); iY = 10; 

    CRandomHelper::Initialize();
    m_IsWireFrame = false;
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void CGraphicEngine::RenderText()
{
    m_pTxtHelper->Begin();
    m_pTxtHelper->SetInsertionPos( 5, 5 );
    m_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    m_pTxtHelper->DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );  
    m_pTxtHelper->DrawTextLine( DXUTGetDeviceStats() );

    TCHAR sOut[256];
    StringCchPrintf(sOut, 256, TEXT("Particle Number: %d"), m_Sim.GetParticleNum()); 

    m_pTxtHelper->DrawTextLine(sOut);
    
    m_pTxtHelper->End();
}


//--------------------------------------------------------------------------------------
// Rejects any D3D9 devices that aren't acceptable to the app by returning false
//--------------------------------------------------------------------------------------
bool CGraphicEngine::IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                     D3DFORMAT BackBufferFormat, bool bWindowed)
{
    // Skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3D9Object(); 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
        AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
        D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    // No fallback defined by this app, so reject any device that 
    // doesn't support at least ps2.0
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(2,0) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D10 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CGraphicEngine::ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings)
{
    if( pDeviceSettings->ver == DXUT_D3D9_DEVICE )
    {
        IDirect3D9 *pD3D = DXUTGetD3D9Object();
        D3DCAPS9 Caps;
        pD3D->GetDeviceCaps( pDeviceSettings->d3d9.AdapterOrdinal, pDeviceSettings->d3d9.DeviceType, &Caps );

        // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
        // then switch to SWVP.
        if( (Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
            Caps.VertexShaderVersion < D3DVS_VERSION(1,1) )
        {
            pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }

        // Debugging vertex shaders requires either REF or software vertex processing 
        // and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
        if( pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF )
        {
            pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
            pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;                            
            pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }
#endif
#ifdef DEBUG_PS
        pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif
    }

    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( (DXUT_D3D9_DEVICE == pDeviceSettings->ver && pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF) ||
            (DXUT_D3D10_DEVICE == pDeviceSettings->ver && pDeviceSettings->d3d10.DriverType == D3D10_DRIVER_TYPE_REFERENCE) )
            DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
    }

    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
// and aren't tied to the back buffer size
//--------------------------------------------------------------------------------------
HRESULT CGraphicEngine::OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, 
                                                    const D3DSURFACE_DESC* pBackBufferSurfaceDesc)
{
    HRESULT hr;

    V_RETURN( m_DialogResourceManager.OnD3D9CreateDevice( pd3dDevice ) );
    V_RETURN( m_SettingsDlg.OnD3D9CreateDevice( pd3dDevice ) );

    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
        OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
        L"Arial", &m_pFont9 ) );


    if (CSetting::IsOffline())
    {
        m_OffEnv.Initialize(1024, 768);
    }

    V(m_Sim.Initialize());

    if (FAILED(hr))
    {
        DXUT_ERR_MSGBOX(TEXT("Failed to create graphic objects for the simulation"), hr);
    }

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
// or that are tied to the back buffer size 
//--------------------------------------------------------------------------------------
HRESULT CGraphicEngine::OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc)
{
    HRESULT hr;

    V_RETURN( m_DialogResourceManager.OnD3D9ResetDevice() );
    V_RETURN( m_SettingsDlg.OnD3D9ResetDevice() );

    if( m_pFont9 ) V_RETURN( m_pFont9->OnResetDevice() );

    V_RETURN( D3DXCreateSprite( pd3dDevice, &m_pSprite9 ) );
    m_pTxtHelper = new CDXUTTextHelper( m_pFont9, m_pSprite9, NULL, NULL, 15 );

    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    m_Camera.SetProjParams( D3DX_PI/4, fAspectRatio, 0.1f, 1000.0f );
    // m_Camera.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );

    m_HUD.SetLocation( pBackBufferSurfaceDesc->Width-170, 0 );
    m_HUD.SetSize( 170, 170 );
	m_StartupUI.SetLocation( ( pBackBufferSurfaceDesc->Width - 300 ) / 2,
		( pBackBufferSurfaceDesc->Height - 200 ) / 2 );
    m_StartupUI.SetSize( 300, 200 );

    std::vector<IGraphicObject*>::iterator it = st_GraphicObjects.begin();
    while (it != st_GraphicObjects.end())
    {
        (*it)->OnResetDevice();
        it++;
    }
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CGraphicEngine::OnFrameMove( double fTime, float fElapsedTime)
{
    // Update the camera's position based on user input 
    if (CSetting::IsOffline())
    {
        m_OffEnv.OfflineMove();
    }
    else
    {
        m_Camera.FrameMove( fElapsedTime );
    }

    m_Sim.Step(CSetting::GetTimeStep());

    DXUTTRACE(TEXT("Update the simulator state< time: %lf, elapse: %f>"), fTime, fElapsedTime);
    //g_Sim.Update(fTime, fElapsedTime);
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void CGraphicEngine::OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime)
{
    HRESULT hr;

    // If the settings dialog is being shown, then render it instead of rendering the app's scene
    if( m_SettingsDlg.IsActive() )
    {
        m_SettingsDlg.OnRender( fElapsedTime );
        return;
    }

    if (CSetting::IsOffline())
    {
        m_OffEnv.BeginRender();
    }

    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 128, 128, 128), 1.0f, 0) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        DXUTGetD3D9Device()->SetRenderState(D3DRS_FILLMODE, m_IsWireFrame ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

        m_Sim.Render();

        DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" ); 
        // These events are to help PIX identify what the code is doing
        RenderText();
        if (!CSetting::IsOffline())
        {
            RenderFrameRate(fElapsedTime);
            V( m_HUD.OnRender( fElapsedTime ) );
            V( m_StartupUI.OnRender( fElapsedTime ) );
        }
        DXUT_EndPerfEvent();

        V( pd3dDevice->EndScene() );
    }

    if (CSetting::IsOffline())
    {
        V(m_OffEnv.EndRender());
    }

}
//////////////////////////////////////////////////////////////////////////
// Render the frame rate to the screen
void CGraphicEngine::RenderFrameRate(float fElapsedTime)
{
    m_pTxtHelper->Begin();
    m_pTxtHelper->SetInsertionPos(5, DXUTGetWindowHeight() - 30);
    m_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );

    TCHAR sOut[256];

    StringCchPrintf(sOut, 256, TEXT("%.1f FPS"), 1 / fElapsedTime);
    m_pTxtHelper->DrawTextLine(sOut);

    m_pTxtHelper->End();
    
}

//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CGraphicEngine::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing)
{
    // Pass messages to dialog resource manager calls so GUI state is updated correctly
    *pbNoFurtherProcessing = m_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass messages to settings dialog if its active
    if( m_SettingsDlg.IsActive() )
    {
        m_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

	*pbNoFurtherProcessing = m_StartupUI.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return 0;

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = m_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;


    // Pass all remaining windows messages to camera so it can respond to user input
    m_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

    return 0;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CGraphicEngine::OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown)
{
    if ((nChar == 'O' || nChar == 'o') && bKeyDown)
    {
        m_Sim.ResetParticles();
    }

	if ((nChar == 'L' || nChar == 'l') && bKeyDown)
	{
		CSetting::SetIsSurfaceRendering(!CSetting::IsSurfaceRendering());
	}

    if (nChar == '1' && bKeyDown)
    {
        m_Sim.SwitchFluidSource(0);
    }
    if (nChar == '2' && bKeyDown)
    {
        m_Sim.SwitchFluidSource(1);
    }
    if (nChar == '3' && bKeyDown)
    {
        m_Sim.SwitchFluidSource(2);
    }
    if (nChar == '4' && bKeyDown)
    {
        m_Sim.SwitchFluidSource(3);
    }

    if ((nChar == 'P' || nChar == 'p') && bKeyDown)
    {
        m_Sim.MakeFluidCube();
    }

    if ((nChar == 'N' || nChar == 'n') && bKeyDown)
    {
        m_Sim.Pause(!m_Sim.isPause());
    }
}

//////////////////////////////////////////////////////////////////////////
//Toggle Wire Frame Mode
void CGraphicEngine::ToggleWireFrameMode()
{
    m_IsWireFrame = !m_IsWireFrame;
}

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CGraphicEngine::OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl)
{
    switch( nControlID )
    {
    case IDC_TOGGLEFULLSCREEN:  DXUTToggleFullScreen(); break;
    case IDC_TOGGLEREF:         DXUTToggleREF(); break;
    case IDC_CHANGEDEVICE:      m_SettingsDlg.SetActive( !m_SettingsDlg.IsActive() ); break;
    case IDC_FILLMODEFRAME:     ToggleWireFrameMode(); break;
    case IDC_SURFACERENDER:     CSetting::SetIsSurfaceRendering(!CSetting::IsSurfaceRendering()); break;
    case IDC_GRAVITY:
        {
            CDXUTSlider* pGravitySlider = m_HUD.GetSlider(IDC_GRAVITY);
            CSetting::SetGravity(D3DXVECTOR3(0, -(float)pGravitySlider->GetValue() / 10, 0));
            TCHAR sz[100];
            StringCchPrintf( sz, 100, TEXT("Gravity: %f"), CSetting::GetGravity().y );
            m_HUD.GetStatic(IDC_GRAVITY_STATIC)->SetText(sz);
        }
        break;
    case IDC_SAMPLELENGTH:
        {
            CDXUTSlider* pSampleSlider = m_HUD.GetSlider(IDC_SAMPLELENGTH);
            CSetting::SetSamplingLength((float)pSampleSlider->GetValue() / 100);
            TCHAR sz[100];
            StringCchPrintf( sz, 100, TEXT("Sample Length: %.2f"), CSetting::GetSamplingLength());
            m_HUD.GetStatic(IDC_SAMPLELENGTH_STATIC)->SetText(sz);
        }
        break;
    }
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9ResetDevice callback 
//--------------------------------------------------------------------------------------
void CGraphicEngine::OnD3D9LostDevice(void)
{
    m_DialogResourceManager.OnD3D9LostDevice();
    m_SettingsDlg.OnD3D9LostDevice();
    if( m_pFont9 ) m_pFont9->OnLostDevice();

    std::vector<IGraphicObject*>::iterator it = st_GraphicObjects.begin();
    while (it != st_GraphicObjects.end())
    {
        (*it)->OnLostDevice();
        it++;
    }

    SAFE_RELEASE( m_pSprite9 );
    SAFE_DELETE( m_pTxtHelper );
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9CreateDevice callback 
//--------------------------------------------------------------------------------------
void CGraphicEngine::OnD3D9DestroyDevice()
{
    m_DialogResourceManager.OnD3D9DestroyDevice();
    m_SettingsDlg.OnD3D9DestroyDevice();
    SAFE_RELEASE( m_pFont9 );
    std::vector<IGraphicObject*>::iterator it = st_GraphicObjects.begin();
    while (it != st_GraphicObjects.end())
    {
        (*it)->OnDestroyDevice();
        it++;
    }
}

