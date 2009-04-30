#pragma once
#include <fstream>
#include <string>
#include "TypeDef.h"

class CLogHelper
{
public:
    CLogHelper(void);
    ~CLogHelper(void);
private:
    static bool m_bLogEnable;
    static std::ofstream m_LogFile;
public:
    static void EnableLogging(const bool bEnable);
    static void SetLogFile(string_t sFileName);
    static void WriteLine(const char* c, bool addTime = true);
    static void WriteFormatLine(LPCSTR strMsg, ...);
};
