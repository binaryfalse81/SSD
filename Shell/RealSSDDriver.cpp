// Copyright [2024] <CRA/BestReviewer>
#include "RealSsdDriver.h"
#include <iostream>
#include <fstream>
#include <string>
#include "../Logger/logger.cpp"

using namespace std;

void CompareBufferMgr::SetCompareData(int LBA, string Data) {
    if (0 <= LBA && LBA < CONFIG_MAX_LBA) {
        compareData[LBA] = Data;
    }
}

bool CompareBufferMgr::CompareBuf() {
    string ReadFileName{ "nand.txt" };
    ifstream resultFile(ReadFileName);
    string line;
    int LBA = 0;

    if (resultFile.is_open()) {
        while (getline(resultFile, line)) {
            LBA_INFO LbaInfo = Parse(line);
            if (LbaInfo.LBAData != compareData[LBA])
            {
                return false;
            }

            LBA++;
            if (LBA >= CONFIG_MAX_LBA)
            {
                break;
            }
        }
        resultFile.close();
    }
    else
    {
        for (LBA = 0; LBA < CONFIG_MAX_LBA; LBA++)
        {
            if (compareData[LBA] != ERASE_DATA)
            {
                return false;
            }
        }
    }

    return true;
}

LBA_INFO CompareBufferMgr::Parse(const string& line) {
    LBA_INFO LbaInfo;
    int firstSpacePos = (int)line.find(' ');
    int secondSpacePos = (int)line.find(' ', firstSpacePos + 1);
    LbaInfo.LBA = stoi(line.substr(0, firstSpacePos));
    if (secondSpacePos == string::npos) {
        LbaInfo.LBASize = 1;
        LbaInfo.LBAData = line.substr(firstSpacePos + 1);
    }
    else {
        LbaInfo.LBAData = line.substr(firstSpacePos + 1, secondSpacePos - (firstSpacePos + 1));
        LbaInfo.LBASize = stoi(line.substr(secondSpacePos + 1));
    }
    return LbaInfo;
}

RealSsdDriver::RealSsdDriver() {
    for (int i = 0; i < CONFIG_MAX_LBA; i++) {
        cmpBufMgr.SetCompareData(i, "0x00000000");
    }
}

string RealSsdDriver::Read(int LBA) {
    LOG_PRINT("Read from LBA");
    string cmdLine = "R " + to_string(LBA);
    SystemCall(cmdLine);
    string ReadFileName{ "result.txt" };
    ifstream resultFile(ReadFileName);
    string line;

    if (resultFile.is_open()) {
        getline(resultFile, line);
        resultFile.close();
    }
    else {
            cerr << "result file open error " << ReadFileName << endl;
    }

    return line;
}

void RealSsdDriver::Write(int LBA, string Data) {
    LOG_PRINT("Write a data to LBA");
    string cmdLine = "W " + to_string(LBA) + " " + Data;
    cmpBufMgr.SetCompareData(LBA, Data);
    SystemCall(cmdLine);
}

void RealSsdDriver::Erase(int startLBA, int Size) {
    LOG_PRINT("Erase data in specific area");
    int LBA = startLBA;
    while (Size > 0) {
        int EraseUnitSize = ((ERASE_LBA_UNIT < Size) ? (ERASE_LBA_UNIT) : (Size));
        string cmdLine = "E " + to_string(LBA);
        cmdLine += " " + to_string(EraseUnitSize);
        for (int i = LBA; i < LBA + EraseUnitSize; i++)
        {
            cmpBufMgr.SetCompareData(i, "0x00000000");
        }
        SystemCall(cmdLine);
        Size -= EraseUnitSize;
        LBA += EraseUnitSize;
    }
}

void RealSsdDriver::Flush() {
    LOG_PRINT("Execute commands in 'Command Buffer'");
    SystemCall("F");
}

unsigned int RealSsdDriver::Compare() {
    return (unsigned int)cmpBufMgr.CompareBuf();
}

#if 0
#include <windows.h>

void RealSsdDriver::SystemCall(std::string cmdLine)
{
    LOG_PRINT("Execute SSD.exe with a command");

    std::wstring ssd_exe_path;
#ifdef _DEBUG
    ssd_exe_path = L"..\\x64\\Debug\\SSD.exe"; // 유니코드 문자열 (L 접두사 사용)
#else
    ssd_exe_path = L"SSD.exe"; // 유니코드 문자열 (L 접두사 사용)
#endif

    ssd_exe_path += L" ";
    ssd_exe_path += std::wstring(cmdLine.begin(), cmdLine.end()); // std::string을 std::wstring으로 변환

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
                        FALSE, // 핸들 상속 여부
                        0, // 생성 플래그
                        NULL, // 새로운 프로세스의 환경 변수
                        NULL, // 현재 디렉토리
                        &si, // STARTUPINFOW 구조체
                        &pi // PROCESS_INFORMATION 구조체
    ))
    {
        std::cerr << "Failed to execute SSD.exe. Error code: " << GetLastError() << '\n';
        return;
    }

    // 프로세스가 종료될 때까지 기다림
    WaitForSingleObject(pi.hProcess, INFINITE);

    // 프로세스 생성 후 핸들 닫기
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
#endif

#if 0
void RealSsdDriver::SystemCall(string cmdLine)
{
    LOG_PRINT("Execute SSD.exe with a command");
#ifdef _DEBUG
    string ssd_exe_path = "..\\x64\\Debug\\SSD.exe";
#else
    string ssd_exe_path = "SSD.exe";
#endif
    ssd_exe_path += " ";
    ssd_exe_path += cmdLine;
    int result = system(ssd_exe_path.c_str());
    if (result)
    {
        cerr << "Failed to execute SSD.exe. Error code: " << result << '\n';
    }
}
#endif

#if 1
#include <windows.h>

typedef int(*ExecuteCommandFunc)(const wchar_t*);

void RealSsdDriver::SystemCall(string cmdLine) {
    LOG_PRINT("Execute SSD.dll with a command");

    // DLL 핸들을 로드
    HMODULE hModule = LoadLibrary(L"SSD.dll");
    if (hModule == NULL) {
        cerr << "Failed to load SSD.dll. Error code: " << GetLastError() << '\n';
        return;
    }

    // ExecuteCommand 함수의 주소를 가져옴
    ExecuteCommandFunc ExecuteCommand = (ExecuteCommandFunc)GetProcAddress(hModule, "ExecuteCommand");
    if (ExecuteCommand == NULL) {
        cerr << "Failed to find ExecuteCommand function. Error code: " << GetLastError() << '\n';
        FreeLibrary(hModule);
        return;
    }

    // 명령어를 wide string으로 변환
    wstring wideCmdLine = std::wstring(cmdLine.begin(), cmdLine.end());

    // ExecuteCommand 함수 호출
    int result = ExecuteCommand(wideCmdLine.c_str());
    if (result != 0) {
        cerr << "Failed to execute command in SSD.dll. Error code: " << result << '\n';
    }

    // DLL 언로드
    FreeLibrary(hModule);
}
#endif