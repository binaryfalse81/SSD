// Copyright.2024.binaryfalse81@gmail.com
#include <string>
#include <iostream>
#include "SSDCommand.h"

using namespace std;

string GetConcatenateArgv(int argc, char* argv[])
{
    string result{ "" };
    for (int i = 1; i < argc; i++)
    {
        result = result + " " + string(argv[i]);
    }
    return result;
}

int main(int argc, char* argv[])
{
    Parser parser;
    SSD ssd;
    CmdStatus status;
    SSDCommand cmd{ &ssd, &parser, &status };
    string arg{ argv[0] };

    arg += GetConcatenateArgv(argc, argv);
    try
    {
        cmd.Run(arg);
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
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (size_needed <= 0)
    {
        cerr << "Error calculating buffer size: " << GetLastError() << '\n';
        return "";
    }

    string strTo(size_needed - 1, 0);  // 널 문자 공간 제외
    int result = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &strTo[0], size_needed, NULL, NULL);
    if (result == 0)
    {
        cerr << "Error converting wide string: " << GetLastError() << '\n';
        return "";
    }

    return strTo;
}

extern "C" __declspec(dllexport) int ExecuteCommand(const wchar_t* cmdLine)
{
    Parser parser;
    SSD ssd;
    CmdStatus status;
    SSDCommand cmd{ &ssd, &parser, &status };
    string arg = " " + WideCharToString(cmdLine);
    cmd.Run(arg);
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
    return TRUE;
}
#endif
