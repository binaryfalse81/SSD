// Copyright.2024.binaryfalse81@gmail.com
#include <iostream>
#include <string>
#include "ShellCommand.h"
#include "SSDDriver.h"
#include "../Logger/Logger.cpp"

using namespace std;

void InvalidCommand::Run(SSDDriver* sd)
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

void WriteCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("execute write command");
    sd->Write(this->nLBA, this->strData);
}

ReadCommand::ReadCommand(string strLBA)
{
    this->strLBA = strLBA;
    this->nLBA = stoi(strLBA);
}

void ReadCommand::Run(SSDDriver* sd)
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

void EraseCommand::Run(SSDDriver* sd)
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

void EraseRangeCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("execute erase range command");
    sd->Erase(this->nStartLBA, this->nEndLBA - this->nStartLBA);
}

void FlushCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("execute flush command");
    sd->Flush();
}

void ExitCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("");
    cout << "[Exit] Quit Shell" << endl;
    throw ExitTestShell();
}

void FailCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("");
    cout << "[Fail] Fault Injection" << endl;
    throw FailTestShell();
}

void HelpCommand::Run(SSDDriver* sd)
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

void FullWriteCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("execute full write command");
    for (int LBA = sd->GetMinLBA(); LBA <= sd->GetMaxLBA(); LBA++)
    {
        sd->Write(LBA, this->strData);
    }
}

void FullReadCommand::Run(SSDDriver* sd)
{
    LOG_PRINT("execute full read command");
    cout << "[FullRead]\n";
    for (int LBA = sd->GetMinLBA(); LBA <= sd->GetMaxLBA(); LBA++)
    {
        string resultData = sd->Read(LBA);
        cout << "[Read] LBA : " << to_string(LBA);
        cout << ", Data : " << resultData << endl;
    }
}

void Compare::Run(SSDDriver* sd)
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
