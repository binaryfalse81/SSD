// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"

enum class ENUM_CMD_TYPE
{
    Read,
    Write,
    Erase,
    Flush,
};

typedef struct
{
    ENUM_CMD_TYPE eCmdType;
    string nLpn;
    string strPattern;
    UINT32 nEraseSize;
} CMD_INFO;

class Parser
{
public:
    CMD_INFO* Parse(const string& strCommand);

private:
    VOID TokenArgument(const string& strCommand);
    CMD_INFO* UpdateCmdInfo();
    CMD_INFO* UpdateWriteCmdInfo();
    CMD_INFO* UpdateReadCmdInfo();
    CMD_INFO* UpdateEraseCmdInfo();
    CMD_INFO* UpdateFlushCmdInfo();
    VOID CheckWriteCmdToken();
    VOID CheckReadCmdToken();
    VOID CheckEraseCmdToken();
    VOID CheckFlushCmdToken();

    vector<string> astrCmdToken;
    const string WRITE_CMD = "W";
    const string READ_CMD = "R";
    const string ERASE_CMD = "E";
    const string FLUSH_CMD = "F";
};
