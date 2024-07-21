// Copyright.2024.binaryfalse81@gmail.com
#include "Header.h"
#include "SSDCmd.h"

string GetConcatenateArgv(INT32 argc, CHAR* argv[])
{
    string strResult{ "" };
    for (INT32 i = 1; i < argc; i++)
    {
        strResult = strResult + " " + string(argv[i]);
    }
    return strResult;
}

INT32 main(INT32 argc, CHAR* argv[])
{
    Parser parser;
    SSD ssd;
    PARSING_RESULT stParsingResult;
    SSDCmd cmd{ &ssd, &parser, &stParsingResult };
    string strInputCmd{ argv[0] };

    strInputCmd += GetConcatenateArgv(argc, argv);
    try
    {
        cmd.Run(strInputCmd);
    }
    catch (...)
    {
        return -1;
    }

    return 0;
}

#define USING_DLL           (true)

#if (USING_DLL)
#include <windows.h>
#include <iostream>

string WideCharToString(const wchar_t* wstr)
{
    INT32 size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (size_needed <= 0)
    {
        cerr << "Error calculating buffer size: " << GetLastError() << '\n';
        return "";
    }

    string strTo(size_needed - 1, 0);  // 널 문자 공간 제외
    INT32 result = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &strTo[0], size_needed, NULL, NULL);
    if (result == 0)
    {
        cerr << "Error converting wide string: " << GetLastError() << '\n';
        return "";
    }

    return strTo;
}

extern "C" __declspec(dllexport) INT32 ExecuteCommand(const wchar_t* cmdLine)
{
    Parser parser;
    SSD ssd;
    PARSING_RESULT stParsingResult;
    SSDCmd cmd{ &ssd, &parser, &stParsingResult };
    string strInputCmd = " " + WideCharToString(cmdLine);
    cmd.Run(strInputCmd);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
{
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return true;
}
#endif
