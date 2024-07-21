// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"
#include "ShellCmd.h"
#include "SSDDriver.h"

class ShellCmdFactory
{
public:
    ShellCmd* Make(string strCmd);
    VOID SetSsdDriver(SSDDriver* sd);

protected:
    SSDDriver* sd;

private:
    VOID TokenArgument(string strCmd);
    VOID MakeCommand();
    ShellCmd* MakeInvalidCommand();
    ShellCmd* MakeWriteCommand();
    ShellCmd* MakeReadCommand();
    ShellCmd* MakeEraseCommand();
    ShellCmd* MakeEraseRangeCommand();
    ShellCmd* MakeFlushCommand();
    ShellCmd* MakeExitCommand();
    ShellCmd* MakeFailCommand();
    ShellCmd* MakeHelpCommand();
    ShellCmd* MakeFullWriteCommand();
    ShellCmd* MakeFullReadCommand();
    ShellCmd* MakeCompareCommand();

    bool IsStringDecimal(string str);
    bool IsStringHexadecimal(string str);
    bool IsStringValidLpn(string str);
    bool IsStringValidLength(string strSize);
    bool IsStringValidLength(string strStartLpn, string strEndLpn);

    vector<string> CommandToken;
    ShellCmd* result;
};
