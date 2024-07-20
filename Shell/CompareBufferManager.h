#pragma once
#include "Header.h"

class CompareBufferMgr
{
public:
    bool CompareBuf();
    VOID SetCompareData(UINT32 nLpn, string strPattern);

private:
    NAND_DATA Parse(string line);
    string compareData[MAX_LPN];
};