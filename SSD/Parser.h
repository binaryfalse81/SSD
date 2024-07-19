// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include<vector>
#include<string>

using namespace std;

enum class CmdType
{
    Read,
    Write,
    Erase,
    Flush,
};

typedef struct
{
    CmdType Command;
    string LBA;
    string LBAData;
    int EraseSize;
} CmdStatus;

class Parser
{
public:
    CmdStatus* Parse(const string& strCommand);

private:
    void TokenArgument(const string& strCommand);
    CmdStatus* UpdateCmdStatus();
    CmdStatus* UpdateWriteCmdStatus();
    CmdStatus* UpdateReadCmdStatus();
    CmdStatus* UpdateEraseCmdStatus();
    CmdStatus* UpdateFlushCmdStatus();
    void CheckWriteCommandToken();
    void CheckReadCommandToken();
    void CheckEraseCommandToken();
    void CheckFlushCommandToken();

    vector<string> CommandToken;
    const string WRITE_CMD = "W";
    const string READ_CMD = "R";
    const string ERASE_CMD = "E";
    const string FLUSH_CMD = "F";
};
