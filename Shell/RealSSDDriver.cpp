// Copyright.2024.binaryfalse81@gmail.com
#include "Header.h"
#include "RealSSDDriver.h"
#include "../Logger/Logger.cpp"

RealSSDDriver::RealSSDDriver()
{
    for (INT32 i = 0; i < CONFIG_MAX_LBA; i++)
    {
        cmpBufMgr.SetCompareData(i, "0x00000000");
    }
}

string RealSSDDriver::Read(INT32 LBA)
{
    LOG_PRINT("Read from LBA");
    string cmdLine = "R " + to_string(LBA);
    SystemCall(cmdLine);
    string ReadFileName{ "result.txt" };
    ifstream resultFile(ReadFileName);
    string line;

    if (resultFile.is_open())
    {
        getline(resultFile, line);
        resultFile.close();
    }
    else
    {
            cerr << "result file open error " << ReadFileName << endl;
    }

    return line;
}

VOID RealSSDDriver::Write(INT32 LBA, string Data)
{
    LOG_PRINT("Write a data to LBA");
    string cmdLine = "W " + to_string(LBA) + " " + Data;
    cmpBufMgr.SetCompareData(LBA, Data);
    SystemCall(cmdLine);
}

VOID RealSSDDriver::Erase(INT32 startLBA, INT32 Size)
{
    LOG_PRINT("Erase data in specific area");
    INT32 LBA = startLBA;
    while (Size > 0)
    {
        INT32 EraseUnitSize = ((ERASE_LBA_UNIT < Size) ? (ERASE_LBA_UNIT) : (Size));
        string cmdLine = "E " + to_string(LBA);
        cmdLine += " " + to_string(EraseUnitSize);
        for (INT32 i = LBA; i < LBA + EraseUnitSize; i++)
        {
            cmpBufMgr.SetCompareData(i, "0x00000000");
        }
        SystemCall(cmdLine);
        Size -= EraseUnitSize;
        LBA += EraseUnitSize;
    }
}

VOID RealSSDDriver::Flush()
{
    LOG_PRINT("Execute commands in 'Command Buffer'");
    SystemCall("F");
}

UINT32 RealSSDDriver::Compare()
{
    return (UINT32)cmpBufMgr.CompareBuf();
}

#define SYSTEM_CALL_VER_1   (false)
#define SYSTEM_CALL_VER_2   (false)
#define USING_DLL           (true)

#if (SYSTEM_CALL_VER_1)
VOID RealSsdDriver::SystemCall(string cmdLine)
{
    LOG_PRINT("Execute SSD.exe with a command");
#ifdef _DEBUG
    string ssd_exe_path = "..\\x64\\Debug\\SSD.exe";
#else
    string ssd_exe_path = "SSD.exe";
#endif
    ssd_exe_path += " ";
    ssd_exe_path += cmdLine;
    INT32 result = system(ssd_exe_path.c_str());
    if (result)
    {
        cerr << "Failed to execute SSD.exe. Error code: " << result << '\n';
    }
}
#elif (SYSTEM_CALL_VER_2)
#include <windows.h>

VOID RealSsdDriver::SystemCall(string cmdLine)
{
    LOG_PRINT("Execute SSD.exe with a command");

    wstring ssd_exe_path;
#ifdef _DEBUG
    ssd_exe_path = L"..\\x64\\Debug\\SSD.exe"; // 유니코드 문자열 (L 접두사 사용)
#else
    ssd_exe_path = L"SSD.exe"; // 유니코드 문자열 (L 접두사 사용)
#endif

    ssd_exe_path += L" ";
    ssd_exe_path += wstring(cmdLine.begin(), cmdLine.end()); // string을 wstring으로 변환

    // 필요한 변수들 선언
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // CreateProcessW로 외부 프로그램 실행
    if (!CreateProcessW(NULL, // 실행할 프로그램의 경로. NULL이면 ssd_exe_path에서 전체 경로를 지정해야 함
        const_cast<LPWSTR>(ssd_exe_path.c_str()), // 실행할 프로그램 명령어 (const wchar_t* 타입으로 변환)
        NULL, // 보안 속성
        NULL, // 보안 속성
        false, // 핸들 상속 여부
        0, // 생성 플래그
        NULL, // 새로운 프로세스의 환경 변수
        NULL, // 현재 디렉토리
        &si, // STARTUPINFOW 구조체
        &pi // PROCESS_INFORMATION 구조체
    ))
    {
        cerr << "Failed to execute SSD.exe. Error code: " << GetLastError() << '\n';
        return;
    }

    // 프로세스가 종료될 때까지 기다림
    WaitForSingleObject(pi.hProcess, INFINITE);

    // 프로세스 생성 후 핸들 닫기
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
#elif (USING_DLL)
#include <windows.h>

typedef INT32(*ExecuteCommandFunc)(const wchar_t*);

VOID RealSSDDriver::SystemCall(string cmdLine)
{
    LOG_PRINT("Execute SSD.dll with a command");

    // DLL 핸들을 로드
#ifdef _DEBUG
    HMODULE hModule = LoadLibrary(L"..\\x64\\Debug\\SSD.dll");
#else
    HMODULE hModule = LoadLibrary(L"SSD.dll");
#endif

    if (hModule == NULL)
    {
        cerr << "Failed to load SSD.dll. Error code: " << GetLastError() << '\n';
        return;
    }

    // ExecuteCommand 함수의 주소를 가져옴
    ExecuteCommandFunc ExecuteCommand = (ExecuteCommandFunc)GetProcAddress(hModule, "ExecuteCommand");
    if (ExecuteCommand == NULL)
    {
        cerr << "Failed to find ExecuteCommand function. Error code: " << GetLastError() << '\n';
        FreeLibrary(hModule);
        return;
    }

    // 명령어를 wide string으로 변환
    wstring wideCmdLine = wstring(cmdLine.begin(), cmdLine.end());

    // ExecuteCommand 함수 호출
    INT32 result = ExecuteCommand(wideCmdLine.c_str());
    if (result != 0)
    {
        cerr << "Failed to execute command in SSD.dll. Error code: " << result << '\n';
    }

    // DLL 언로드
    FreeLibrary(hModule);
}
#endif