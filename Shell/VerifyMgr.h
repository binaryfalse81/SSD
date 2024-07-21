#pragma once
#include "Header.h"

class VerifyMgr
{
public:
    bool Verify();
    VOID SetVerifyPattern(UINT32 nLpn, string strPattern);

private:
    NAND_DATA Parsing(string strLine);
    string astrVerifyPattern[MAX_LPN];
};