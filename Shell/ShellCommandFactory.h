// Copyright [2024] <CRA/BestReviewer>
#pragma once
#include <vector>
#include <string>
#include "ShellCommand.h"
#include "SsdDriver.h"

class ShellCommandFactory {
 public:
    ShellCommand* Make(const string& strCommand);
    void SetSsdDriver(SsdDriver* ssddriver);
 protected:
    SsdDriver* ssddriver;
 private:
    void TokenArgument(const string& strCommand);
    void MakeCommand();
    ShellCommand* MakeInvalidCommand();
    ShellCommand* MakeWriteCommand();
    ShellCommand* MakeReadCommand();
    ShellCommand* MakeEraseCommand();
    ShellCommand* MakeEraseRangeCommand();
    ShellCommand* MakeFlushCommand();
    ShellCommand* MakeExitCommand();
    ShellCommand* MakeFailCommand();
    ShellCommand* MakeHelpCommand();
    ShellCommand* MakeFullWriteCommand();
    ShellCommand* MakeFullReadCommand();
    ShellCommand* MakeTestApp1Command();
    ShellCommand* MakeTestApp2Command();
    ShellCommand* MakeCompareCommand();

    bool IsStringDecimal(const string& str);
    bool IsStringHexadecimal(const string& str);
    bool IsStringValidLBA(const string& str);
    bool IsStringValidLength(const string& strSize);
    bool IsStringValidLength(const string& strStartLBA, const string& strEndLBA);
    string LimitToMinLBA(const string& str);
    string LimitToMaxLBA(const string& str);

    vector<string> CommandToken;
    ShellCommand* result;

    const int MAX_STR_LENGTH_DATA = 10;
};
