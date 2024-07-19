// Copyright [2024] <CRA/BestReviewer>
#pragma once
#include<vector>
#include<string>

enum class CmdType {
    Read,
    Write,
    Erase,
    Flush,
};

struct CmdStatus {
 public:
    CmdType Command;
    std::string LBA;
    std::string LBAData;
    int EraseSize;
};

class Parser {
 public:
    CmdStatus* Parse(const std::string& strCommand);

 private:
    void TokenArgument(const std::string& strCommand);
    CmdStatus* UpdateCmdStatus();
    CmdStatus* UpdateWriteCmdStatus();
    CmdStatus* UpdateReadCmdStatus();
    CmdStatus* UpdateEraseCmdStatus();
    CmdStatus* UpdateFlushCmdStatus();
    void CheckWriteCommandToken();
    void CheckReadCommandToken();
    void CheckEraseCommandToken();
    void CheckFlushCommandToken();

    std::vector<std::string> CommandToken;
    const std::string WRITE_CMD = "W";
    const std::string READ_CMD = "R";
    const std::string ERASE_CMD = "E";
    const std::string FLUSH_CMD = "F";
};
