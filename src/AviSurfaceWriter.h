#pragma once
#include "Vfw.h"
#include <string>
#include "TypeDef.h"

class CAviSurfaceWriter
{
public:
    CAviSurfaceWriter();
    CAviSurfaceWriter(LPCTSTR lpszFileName, UINT Rate);
    virtual ~CAviSurfaceWriter(void);
    HRESULT InitializeVfw(LPCTSTR lpszFileName, UINT Rate);
    void    InitVariables();

    virtual HRESULT AddFrame(LPDIRECT3DSURFACE9 pSurface);
    //TCHAR* GetFName() const;
    virtual bool IsOK() const;
private:
    static const UINT kTextHeight = 20;
    static const UINT kAviFileKeyFrame = 0x00000010L;
    static const UINT kBufSize = 8192;
   
    bool            m_bOk;

    string_t        m_AviFileName;

    int             m_Width;
    int             m_Pitch;
    int             m_Height;
    UINT            m_nFrame;
    UINT            m_Rate;

    LPBYTE          m_pData;

    AVISTREAMINFO   m_StreamHdr;
    PAVIFILE        m_pFile;
    PAVISTREAM      m_pStream;
    PAVISTREAM      m_pStreamCompressed;

    AVICOMPRESSOPTIONS m_CompOpts;
    AVICOMPRESSOPTIONS FAR * m_aCompOpts[1];
};
