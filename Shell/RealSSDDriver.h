// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include <iostream>
#include <string>
#include "SSDDriver.h"

using namespace std;

const int CONFIG_MAX_LBA = 100;

typedef struct
{
    int LBA;
    string LBAData;
    int LBASize;
} LBA_INFO;

class CompareBufferMgr
{
public:
    bool CompareBuf();
    void SetCompareData(int LBA, string Data);

private:
    LBA_INFO Parse(const string& line);
    string compareData[CONFIG_MAX_LBA];
    const string ERASE_DATA = "0x00000000";
};

class RealSSDDriver : public SSDDriver
{
public:
    RealSSDDriver();
    string Read(int LBA) override;
    void Write(int LBA, string Data) override;
    void Erase(int startLBA, int Size) override;
    void Flush() override;
    unsigned int Compare() override;
    int GetMinLBA() override { return MIN_LBA; }
    int GetMaxLBA() override { return MAX_LBA; }

private:
    void SystemCall(string cmdLine);
    CompareBufferMgr cmpBufMgr;
    const int MIN_LBA = 0;
    const int MAX_LBA = 99;
    const int ERASE_LBA_UNIT = 10;
};
