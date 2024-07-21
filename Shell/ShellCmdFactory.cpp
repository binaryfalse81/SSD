// Copyright.2024.binaryfalse81@gmail.com
#include "Header.h"
#include "ShellCmdFactory.h"
#include "../Logger/Logger.cpp"

ShellCmd* ShellCmdFactory::Make(string strCmd)
{
    TokenArgument(strCmd);
    MakeCommand();
    return result;
}

VOID ShellCmdFactory::SetSsdDriver(SSDDriver* sd)
{
    this->sd = sd;
}

VOID ShellCmdFactory::TokenArgument(string strCmd)
{
    LOG_PRINT("Separate commands into tokens");
    string token;
    size_t start = 0, end = 0;
    CommandToken.clear();

    while ((end = strCmd.find(' ', start)) != string::npos)
    {
        token = strCmd.substr(start, end - start);
        CommandToken.push_back(token);
        start = end + 1;
    }

    // 마지막 단어 처리
    token = strCmd.substr(start);
    CommandToken.push_back(token);
}

VOID ShellCmdFactory::MakeCommand()
{
    LOG_PRINT("Generate the appropriate command");
    if (CommandToken.empty() == true) result = MakeInvalidCommand();
    else if (CommandToken[0] == "write") result = MakeWriteCommand();
    else if (CommandToken[0] == "read") result = MakeReadCommand();
    else if (CommandToken[0] == "erase") result = MakeEraseCommand();
    else if (CommandToken[0] == "erase_range") result = MakeEraseRangeCommand();
    else if (CommandToken[0] == "flush") result = MakeFlushCommand();
    else if (CommandToken[0] == "exit") result = MakeExitCommand();
    else if (CommandToken[0] == "fail") result = MakeFailCommand();
    else if (CommandToken[0] == "help") result = MakeHelpCommand();
    else if (CommandToken[0] == "fullwrite") result = MakeFullWriteCommand();
    else if (CommandToken[0] == "fullread") result = MakeFullReadCommand();
    else if (CommandToken[0] == "compare") result = MakeCompareCommand();
    else
        result = MakeInvalidCommand();
}

ShellCmd* ShellCmdFactory::MakeInvalidCommand()
{
    return new InvalidCommand();
}

ShellCmd* ShellCmdFactory::MakeWriteCommand()
{
    // Check Invalid 1) Argument Length
    if (CommandToken.size() != 3)
    {
        return new InvalidCommand();
    }

    // Check Invalid 2) nLpn
    if (IsStringDecimal(CommandToken[1]) == false)
    {
        return new InvalidCommand();
    }

    if (IsStringValidLpn(CommandToken[1]) == false)
    {
        return new InvalidCommand();
    }

    // Check Invalid 3) Data
    if (IsStringHexadecimal(CommandToken[2]) == false)
    {
        return new InvalidCommand();
    }

    return new WriteCommand(CommandToken[1], CommandToken[2]);
}


ShellCmd* ShellCmdFactory::MakeReadCommand()
{
    // Check Invalid 1) Argument Length
    if (CommandToken.size() != 2)
    {
        return new InvalidCommand();
    }

    // Check Invalid 2) nLpn
    if (IsStringDecimal(CommandToken[1]) == false)
    {
        return new InvalidCommand();
    }

    if (IsStringValidLpn(CommandToken[1]) == false)
    {
        return new InvalidCommand();
    }

    return new ReadCommand(CommandToken[1]);
}

ShellCmd* ShellCmdFactory::MakeEraseCommand()
{
    // Check Invalid 1) Argument Length
    if (CommandToken.size() != 3)
    {
        return new InvalidCommand();
    }

    // Check Invalid 2) nLpn
    if (IsStringDecimal(CommandToken[1]) == false)
    {
        return new InvalidCommand();
    }

    if (IsStringValidLpn(CommandToken[1]) == false)
    {
        return new InvalidCommand();
    }

    // Check Invalid 3) Size
    if (IsStringDecimal(CommandToken[2]) == false)
    {
        return new InvalidCommand();
    }

    if (IsStringValidLength(CommandToken[2]) == false)
    {
        return new InvalidCommand();
    }

    return new EraseCommand(CommandToken[1], CommandToken[2]);
}

ShellCmd* ShellCmdFactory::MakeEraseRangeCommand()
{
    // Check Invalid 1) Argument Length
    if (CommandToken.size() != 3)
    {
        return new InvalidCommand();
    }

    // Check Invalid 2) StartLpn
    if (IsStringDecimal(CommandToken[1]) == false)
    {
        return new InvalidCommand();
    }

    if (IsStringValidLpn(CommandToken[1]) == false)
    {
        return new InvalidCommand();
    }

    // Check Invalid 3) EndLpn
    if (IsStringDecimal(CommandToken[2]) == false)
    {
        return new InvalidCommand();
    }

    // Check Invalid 4) Size
    if (IsStringValidLength(CommandToken[1], CommandToken[2]) == false)
    {
        return new InvalidCommand();
    }

    return new EraseRangeCommand(CommandToken[1], CommandToken[2]);
}

ShellCmd* ShellCmdFactory::MakeFlushCommand()
{
    // Check Invalid 1) Argument Length
    if (CommandToken.size() != 1)
    {
        return new InvalidCommand();
    }

    return new FlushCommand();
}

ShellCmd* ShellCmdFactory::MakeExitCommand()
{
    // Check Invalid 1) Argument Length
    if (CommandToken.size() != 1)
    {
        return new InvalidCommand();
    }

    return new ExitCommand();
}

ShellCmd* ShellCmdFactory::MakeFailCommand()
{
    // Check Invalid 1) Argument Length
    if (CommandToken.size() != 1)
    {
        return new InvalidCommand();
    }

    return new FailCommand();
}

ShellCmd* ShellCmdFactory::MakeHelpCommand()
{
    // Check Invalid 1) Argument Length
    if (CommandToken.size() != 1)
    {
        return new InvalidCommand();
    }

    return new HelpCommand();
}

ShellCmd* ShellCmdFactory::MakeFullWriteCommand()
{
    // Check Invalid 1) Argument Length
    if (CommandToken.size() != 2)
    {
        return new InvalidCommand();
    }

    // Check Invalid 2) Data
    if (IsStringHexadecimal(CommandToken[1]) == false)
    {
        return new InvalidCommand();
    }

    return new FullWriteCommand(CommandToken[1]);
}

ShellCmd* ShellCmdFactory::MakeFullReadCommand()
{
    // Check Invalid 1) Argument Length
    if (CommandToken.size() != 1)
    {
        return new InvalidCommand();
    }

    return new FullReadCommand();
}

ShellCmd* ShellCmdFactory::MakeCompareCommand()
{
    // Check Invalid 1) Argument Length
    if (CommandToken.size() != 1)
    {
        return new InvalidCommand();
    }

    return new Compare();
}

bool ShellCmdFactory::IsStringDecimal(string str)
{
    for (CHAR ch = 0; ch < str.size(); ch++)
    {
        if ('0' > str[ch] || str[ch] > '9')
        {
            return false;
        }
    }
    return true;
}

bool ShellCmdFactory::IsStringHexadecimal(string str)
{
    if ((str[0] != '0') ||
        (str[1] != 'x') ||
        (str.size() != MAX_PATTERN_LENGTH))
    {
        return false;
    }

    for (UINT32 idx = 2; idx < str.size(); idx++)
    {
        if (('0' > str[idx] || str[idx] > '9') &&
            ('A' > str[idx] || str[idx] > 'F'))
        {
            return false;
        }
    }
    return true;
}

bool ShellCmdFactory::IsStringValidLpn(string str)
{
    UINT32 nLpn = stoi(str);
    if (nLpn < MAX_LPN)
    {
        return true;
    }
    return false;
}

bool ShellCmdFactory::IsStringValidLength(string str)
{
    return (stoi(str) > 0);
}

bool ShellCmdFactory::IsStringValidLength(string strStartLpn, string strEndLpn)
{
    return ((stoi(strEndLpn) - stoi(strStartLpn)) > 0);
}

