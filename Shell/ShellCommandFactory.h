// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"
#include "ShellCommand.h"
#include "SSDDriver.h"

class ShellCommandFactory
{
public:
    ShellCommand* Make(const string& strCommand);
    VOID SetSsdDriver(SSDDriver* sd);

protected:
    SSDDriver* sd;

private:
    VOID TokenArgument(const string& strCommand);
    VOID MakeCommand();
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

    const INT32 MAX_STR_LENGTH_DATA = 10;
};
