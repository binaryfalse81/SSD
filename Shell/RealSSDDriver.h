// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"
#include "SSDDriver.h"
#include "CompareBufferManager.h"

class RealSSDDriver : public SSDDriver
{
public:
    RealSSDDriver();
    string Read(INT32 LBA) override;
    VOID Write(INT32 LBA, string Data) override;
    VOID Erase(INT32 startLBA, INT32 Size) override;
    VOID Flush() override;
    UINT32 Compare() override;
    INT32 GetMinLBA() override { return MIN_LBA; }
    INT32 GetMaxLBA() override { return MAX_LBA; }

private:
    VOID SystemCall(string cmdLine);
    CompareBufferMgr cmpBufMgr;
    const INT32 MIN_LBA = 0;
    const INT32 MAX_LBA = 99;
    const INT32 ERASE_LBA_UNIT = 10;
};
