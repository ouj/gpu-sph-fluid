#include "stdafx.h"
#include "AviSurfaceWriter.h"

CAviSurfaceWriter::CAviSurfaceWriter()
{
    InitVariables();
}

CAviSurfaceWriter::CAviSurfaceWriter(LPCTSTR lpszFileName, UINT Rate)
{
    InitVariables();
    HRESULT hr = S_OK;
    V(InitializeVfw(lpszFileName, Rate));
}

CAviSurfaceWriter::~CAviSurfaceWriter(void)
{
    SAFE_DELETE_ARRAY(m_pData);

    if (m_pStream)
        AVIStreamRelease(m_pStream);

    if (m_pStreamCompressed)
        AVIStreamRelease(m_pStreamCompressed);

    if (m_pFile)
        AVIFileRelease(m_pFile);

    WORD wVer = HIWORD(VideoForWindowsVersion());
    if (wVer >= 0x010A)
    {
        AVIFileExit();
    }
}

void CAviSurfaceWriter::InitVariables()
{
    m_pFile = NULL;
    m_pStream = NULL;
    m_pStreamCompressed = NULL;
    m_aCompOpts[0] = &m_CompOpts;
    m_pData = NULL;
}

HRESULT CAviSurfaceWriter::InitializeVfw(LPCTSTR lpszFileName, UINT Rate)
{

    HRESULT hr = S_OK;

    m_AviFileName = lpszFileName;

    m_nFrame = 0;
    m_Rate = Rate;

    WORD wVer = HIWORD(VideoForWindowsVersion());
    if (wVer < 0x010A)
    {
        m_bOk = false;
        return E_FAIL;
    }
    else
    {
        AVIFileInit();
        m_bOk = true;
        hr = S_OK;
    }

    return hr;
}

HRESULT CAviSurfaceWriter::AddFrame(LPDIRECT3DSURFACE9 pSurface)
{
    HRESULT hr = S_OK;

    if (!m_bOk)
        return E_FAIL;

    D3DLOCKED_RECT lockRect;
    V_RETURN(pSurface->LockRect(&lockRect, NULL, 
        D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_READONLY));

    if (m_nFrame == 0)
    {
        D3DSURFACE_DESC desc;
        pSurface->GetDesc( &desc );

        m_Height = desc.Height;
        m_Width = desc.Width;
        m_Pitch = lockRect.Pitch;

        BITMAPINFOHEADER bi;
        bi.biSize = sizeof(BITMAPINFOHEADER) ;
        bi.biWidth = m_Width ;
        bi.biHeight = m_Height ;
        bi.biPlanes = 1 ;
        bi.biBitCount = (WORD) 32 ;
        bi.biCompression = BI_RGB ;
        bi.biSizeImage = m_Height * m_Pitch;
        bi.biXPelsPerMeter = 0 ;
        bi.biYPelsPerMeter = 0 ;
        bi.biClrUsed = 0;
        bi.biClrImportant = 0 ;

        hr = AVIFileOpen(&m_pFile,		    // returned file pointer
            m_AviFileName.c_str(),							// file name
            OF_WRITE | OF_CREATE,		    // mode to open file with
            NULL);							// use handler determined
        // from file extension....
        if (hr != AVIERR_OK)
        {
            m_bOk = false;
            return E_FAIL;
        }

        memset(&m_StreamHdr, 0, sizeof(m_StreamHdr));
        m_StreamHdr.fccType                = streamtypeVIDEO;// stream type
        m_StreamHdr.fccHandler             = 0;
        m_StreamHdr.dwScale                = 1;
        m_StreamHdr.dwRate                 = m_Rate;
        m_StreamHdr.dwSuggestedBufferSize  = kBufSize;
        SetRect(&m_StreamHdr.rcFrame, 0, 0,		    // rectangle for stream
            (int) desc.Width,
            (int) desc.Height);

        // And create the stream;
        hr = AVIFileCreateStream(m_pFile,		    // file pointer
            &m_pStream,		    // returned stream pointer
            &m_StreamHdr);	    // stream header
        if (hr != AVIERR_OK)
        {
            m_bOk = false;
            return E_FAIL;
        }

        memset(&m_CompOpts, 0, sizeof(m_CompOpts));

        if (!AVISaveOptions(NULL, 0, 1, &m_pStream, (LPAVICOMPRESSOPTIONS FAR *) &m_aCompOpts))
        {
            m_bOk = false;
            return E_FAIL;
        }

        hr = AVIMakeCompressedStream(&m_pStreamCompressed, m_pStream, &m_CompOpts, NULL);
        if (hr != AVIERR_OK)
        {
            m_bOk = false;
            return E_FAIL;
        }

        hr = AVIStreamSetFormat(m_pStreamCompressed, 0, &bi, bi.biSize);
        if (hr != AVIERR_OK)
        {
            m_bOk = false;
            return E_FAIL;
        }

        m_pData = new BYTE[bi.biSizeImage];
    }

    //inverse the data
    for (int i = 0; i < m_Height; i++)
    {
        memcpy(m_pData + i * m_Pitch, 
            ((LPBYTE)lockRect.pBits) + (m_Height - i - 1) * m_Pitch, m_Pitch);
    }

    V_RETURN(pSurface->UnlockRect());

    hr = AVIStreamWrite(m_pStreamCompressed, m_nFrame, 1,
        (LPBYTE) m_pData, m_Height * m_Pitch, AVIIF_KEYFRAME,
        NULL, NULL);

    if (hr != AVIERR_OK)
    {
        m_bOk = false;
        return E_FAIL;
    }
    m_nFrame++;

    return S_OK;
}

bool CAviSurfaceWriter::IsOK() const
{
    return m_bOk;
}