// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"
#include "SSDDriver.h"
#include "CompareBufferManager.h"

class RealSSDDriver : public SSDDriver
{
public:
    RealSSDDriver();
    string Read(UINT32 nLpn) override;
    VOID Write(UINT32 nLpn, string strPattern) override;
    VOID Erase(UINT32 nLpn, UINT32 nSize) override;
    VOID Flush() override;
    UINT32 Compare() override;

private:
    VOID SystemCall(string strCmd);
    CompareBufferMgr cmpBufMgr;
};
