#pragma once
#include "Header.h"

class VerifyMgr
{
public:
    bool Verify();
    VOID SetVerifyPattern(UINT32 nLpn, string strPattern);

private:
    NAND_DATA Parsing(string line);
    string astrVerifyPattern[MAX_LPN];
};