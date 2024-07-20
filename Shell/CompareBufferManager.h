#pragma once
#include "Header.h"

const INT32 CONFIG_MAX_LBA = 100;

typedef struct
{
    INT32 LBA;
    string LBAData;
    INT32 LBASize;
} LBA_INFO;

class CompareBufferMgr
{
public:
    bool CompareBuf();
    VOID SetCompareData(INT32 LBA, string Data);

private:
    LBA_INFO Parse(const string& line);
    string compareData[CONFIG_MAX_LBA];
    const string ERASE_DATA = "0x00000000";
};