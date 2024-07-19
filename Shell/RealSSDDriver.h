// Copyright [2024] <CRA/BestReviewer>
#pragma once
#include <iostream>
#include <string>
#include "SsdDriver.h"

using namespace std;

const int CONFIG_MAX_LBA = 100;

struct LBA_INFO {
    int LBA;
    string LBAData;
    int LBASize;
};

class CompareBufferMgr {
public:
    bool CompareBuf();
    void SetCompareData(int LBA, string Data);
private:
    LBA_INFO Parse(const string& line);
    string compareData[CONFIG_MAX_LBA];
    const string ERASE_DATA = "0x00000000";
};

class RealSsdDriver : public SsdDriver {
 public:
    RealSsdDriver();
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
