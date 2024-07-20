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

WriteCommand::WriteCommand(string strLBA, string strData)
{
    this->strLBA = strLBA;
    this->nLBA = stoi(strLBA);
    this->strData = strData;
}

VOID WriteCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("execute write command");
    sd->Write(this->nLBA, this->strData);
}

ReadCommand::ReadCommand(string strLBA)
{
    this->strLBA = strLBA;
    this->nLBA = stoi(strLBA);
}

VOID ReadCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("execute read command");
    string resultData = sd->Read(this->nLBA);
    cout << "[Read] LBA : " << this->strLBA;
    cout << ", Data : " << resultData << endl;
}

EraseCommand::EraseCommand(string strStartLBA, string strSize)
{
    this->strStartLBA = strStartLBA;
    this->strSize = strSize;
    this->nStartLBA = stoi(strStartLBA);
    this->nSize = stoi(strSize);
}

VOID EraseCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("execute erase command");
    sd->Erase(this->nStartLBA, this->nSize);
}

EraseRangeCommand::EraseRangeCommand(string strStartLBA, string strEndLBA)
{
    this->strStartLBA = strStartLBA;
    this->strEndLBA = strEndLBA;
    this->nStartLBA = stoi(strStartLBA);
    this->nEndLBA = stoi(strEndLBA);
}

VOID EraseRangeCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("execute erase range command");
    sd->Erase(this->nStartLBA, this->nEndLBA - this->nStartLBA);
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
    HelpMessage += "1. write {LBA} {Data}\n";
    HelpMessage += "2. read {LBA}\n";
    HelpMessage += "3. exit\n";
    HelpMessage += "4. help\n";
    HelpMessage += "5. fullwrite {Data}\n";
    HelpMessage += "6. fullread\n";
    HelpMessage += "7. erase {LBA} {Size}\n";
    HelpMessage += "8. erase_range {LBA} {LBA}\n";
    HelpMessage += "9. flush\n";
    HelpMessage += "{LBA} = {x is an integer | 0 <= x <= 99}\n";
    HelpMessage += "{Data} = {""0x[0-9A-F]""}\n";
    cout << HelpMessage;
}

FullWriteCommand::FullWriteCommand(string strData)
{
    this->strData = strData;
}

VOID FullWriteCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("execute full write command");
    for (INT32 LBA = sd->GetMinLBA(); LBA <= sd->GetMaxLBA(); LBA++)
    {
        sd->Write(LBA, this->strData);
    }
}

VOID FullReadCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("execute full read command");
    cout << "[FullRead]\n";
    for (INT32 LBA = sd->GetMinLBA(); LBA <= sd->GetMaxLBA(); LBA++)
    {
        string resultData = sd->Read(LBA);
        cout << "[Read] LBA : " << to_string(LBA);
        cout << ", Data : " << resultData << endl;
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
