// Copyright.2024.binaryfalse81@gmail.com
#include "Header.h"
#include "ShellCommand.h"
#include "SSDDriver.h"
#include "../Logger/Logger.cpp"

VOID InvalidCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("");
    cout << "INVALID COMMAND" << endl;
}

WriteCommand::WriteCommand(string strLpn, string strPattern)
{
    this->strLpn = strLpn;
    this->nLpn = stoi(strLpn);
    this->strPattern = strPattern;
}

VOID WriteCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("execute write command");
    sd->Write(this->nLpn, this->strPattern);
}

ReadCommand::ReadCommand(string strLpn)
{
    this->strLpn = strLpn;
    this->nLpn = stoi(strLpn);
}

VOID ReadCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("execute read command");
    string resultData = sd->Read(this->nLpn);
    cout << "[Read] nLpn : " << this->strLpn;
    cout << ", Data : " << resultData << endl;
}

EraseCommand::EraseCommand(string strLpn, string strSize)
{
    this->strLpn = strLpn;
    this->strSize = strSize;
    this->nLpn = stoi(strLpn);
    this->nSize = stoi(strSize);
}

VOID EraseCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("execute erase command");
    sd->Erase(this->nLpn, this->nSize);
}

EraseRangeCommand::EraseRangeCommand(string strStartLpn, string strEndLpn)
{
    this->strStartLpn = strStartLpn;
    this->strEndLpn = strEndLpn;
    this->nLpnStart = stoi(strStartLpn);
    this->nLpnEnd = stoi(strEndLpn);
}

VOID EraseRangeCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("execute erase range command");
    sd->Erase(this->nLpnStart, this->nLpnEnd - this->nLpnStart);
}

VOID FlushCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("execute flush command");
    sd->Flush();
}

VOID ExitCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("");
    cout << "[Exit] Quit Shell" << endl;
    throw ExitTestShell();
}

VOID FailCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("");
    cout << "[Fail] Fault Injection" << endl;
    throw FailTestShell();
}

VOID HelpCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("print supported commands");
    string HelpMessage = "";

    HelpMessage += "[Help]\n";
    HelpMessage += "1. write {nLpn} {Data}\n";
    HelpMessage += "2. read {nLpn}\n";
    HelpMessage += "3. exit\n";
    HelpMessage += "4. help\n";
    HelpMessage += "5. fullwrite {Data}\n";
    HelpMessage += "6. fullread\n";
    HelpMessage += "7. erase {nLpn} {Size}\n";
    HelpMessage += "8. erase_range {nLpn} {nLpn}\n";
    HelpMessage += "9. flush\n";
    HelpMessage += "{nLpn} = {x is an integer | 0 <= x <= 99}\n";
    HelpMessage += "{Data} = {""0x[0-9A-F]""}\n";
    cout << HelpMessage;
}

FullWriteCommand::FullWriteCommand(string strPattern)
{
    this->strPattern = strPattern;
}

VOID FullWriteCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("execute full write command");
    for (UINT32 nLpn = 0; nLpn < MAX_LPN; nLpn++)
    {
        sd->Write(nLpn, this->strPattern);
    }
}

VOID FullReadCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("execute full read command");
    cout << "[FullRead]\n";
    for (UINT32 nLpn = 0; nLpn < MAX_LPN; nLpn++)
    {
        string strResult = sd->Read(nLpn);
        cout << "[Read] nLpn : " << to_string(nLpn);
        cout << ", Data : " << strResult << endl;
    }
}

VOID Compare::Run(SSDDriver* sd)
{
    LOG_PRINT("execute compare command");
    cout << "[Compare]\n";
    sd->Flush();
    if (IsCompareFail(sd))
    {
        cout << "Fail" << endl;
        throw ExceptionCompareFail();
    }

    cout << "Pass" << endl;
    return;
}

bool Compare::IsCompareFail(SSDDriver* sd)
{
    return 0 == sd->Compare();
}
