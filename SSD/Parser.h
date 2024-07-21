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
} PARSING_RESULT;

class Parser
{
public:
    PARSING_RESULT* Parse(const string& strCommand);

private:
    VOID TokenArgument(const string& strCommand);
    PARSING_RESULT* UpdateCmdInfo();
    PARSING_RESULT* UpdateWriteCmdInfo();
    PARSING_RESULT* UpdateReadCmdInfo();
    PARSING_RESULT* UpdateEraseCmdInfo();
    PARSING_RESULT* UpdateFlushCmdInfo();
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
