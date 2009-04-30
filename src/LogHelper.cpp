#include "stdafx.h"
#include "LogHelper.h"
#include <string.h>
#include "TimeHelper.h"


bool CLogHelper::m_bLogEnable = false;
std::ofstream CLogHelper::m_LogFile;

CLogHelper::CLogHelper(void)
{
}

CLogHelper::~CLogHelper(void)
{
}

void CLogHelper::EnableLogging(bool bEnable)
{
    m_bLogEnable = bEnable;
}

void CLogHelper::SetLogFile(string_t sFileName)
{
    if (m_LogFile.is_open())
    {
        m_LogFile.close();
    }
    m_LogFile.open(sFileName.c_str(), std::ios_base::app);
}

void CLogHelper::WriteLine(const char* c, bool addTime)
{
    if (!m_bLogEnable)
        return;

    if (addTime == false)
    {
        m_LogFile << c << std::endl;
        return;
    }

    char timeBuf[26];
    CTimeHelper::GetCurTime(timeBuf, 26);

    m_LogFile << "[" << timeBuf << "]: " << c << std::endl;
}

void CLogHelper::WriteFormatLine( LPCSTR strMsg, ...)
{
    if (!m_bLogEnable)
        return;

    CHAR strBuffer[512];

    va_list args;
    va_start(args, strMsg);
    StringCchVPrintfA( strBuffer, 512, strMsg, args );
    strBuffer[511] = '\0';
    va_end(args);

    char timeBuf[26];
    CTimeHelper::GetCurTime(timeBuf, 26);

    m_LogFile << "[" << timeBuf << "]: " << strBuffer << std::endl;   
}
