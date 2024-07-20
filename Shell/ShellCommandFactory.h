// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"
#include "ShellCommand.h"
#include "SSDDriver.h"

class ShellCommandFactory
{
public:
    ShellCommand* Make(string strCmd);
    VOID SetSsdDriver(SSDDriver* sd);

protected:
    SSDDriver* sd;

private:
    VOID TokenArgument(string strCmd);
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
    ShellCommand* MakeCompareCommand();

    bool IsStringDecimal(string str);
    bool IsStringHexadecimal(string str);
    bool IsStringValidLpn(string str);
    bool IsStringValidLength(string strSize);
    bool IsStringValidLength(string strStartLpn, string strEndLpn);

    vector<string> CommandToken;
    ShellCommand* result;
};
