// Copyright [2024] <CRA/BestReviewer>
#include <iostream>
#include <string>
#include "ShellCommand.h"
#include "SsdDriver.h"
#include "../Logger/logger.cpp"

using namespace std;

void InvalidCommand::Run(SsdDriver* ssddriver) {
    LOG_PRINT("");
    cout << "INVALID COMMAND" << endl;
}

WriteCommand::WriteCommand(string strLBA, string strData) {
    this->strLBA = strLBA;
    this->nLBA = stoi(strLBA);
    this->strData = strData;
}

void WriteCommand::Run(SsdDriver* ssddriver) {
    LOG_PRINT("execute write command");
    ssddriver->Write(this->nLBA, this->strData);
}

ReadCommand::ReadCommand(string strLBA) {
    this->strLBA = strLBA;
    this->nLBA = stoi(strLBA);
}

void ReadCommand::Run(SsdDriver* ssddriver) {
    LOG_PRINT("execute read command");
    string resultData = ssddriver->Read(this->nLBA);
    cout << "[Read] LBA : " << this->strLBA;
    cout << ", Data : " << resultData << endl;
}

EraseCommand::EraseCommand(string strStartLBA, string strSize) {
    this->strStartLBA = strStartLBA;
    this->strSize = strSize;
    this->nStartLBA = stoi(strStartLBA);
    this->nSize = stoi(strSize);
}

void EraseCommand::Run(SsdDriver* ssddriver) {
    LOG_PRINT("execute erase command");
    ssddriver->Erase(this->nStartLBA, this->nSize);
}

EraseRangeCommand::EraseRangeCommand(string strStartLBA, string strEndLBA) {
    this->strStartLBA = strStartLBA;
    this->strEndLBA = strEndLBA;
    this->nStartLBA = stoi(strStartLBA);
    this->nEndLBA = stoi(strEndLBA);
}

void EraseRangeCommand::Run(SsdDriver* ssddriver) {
    LOG_PRINT("execute erase range command");
    ssddriver->Erase(this->nStartLBA, this->nEndLBA - this->nStartLBA);
}

void FlushCommand::Run(SsdDriver* ssddriver) {
    LOG_PRINT("execute flush command");
    ssddriver->Flush();
}

void ExitCommand::Run(SsdDriver* ssddriver) {
    LOG_PRINT("");
    cout << "[Exit] Quit Shell" << endl;
    throw ExitTestShell();
}

void FailCommand::Run(SsdDriver* ssddriver) {
    LOG_PRINT("");
    cout << "[Fail] Fault Injection" << endl;
    throw FailTestShell();
}

void HelpCommand::Run(SsdDriver* ssddriver) {
    LOG_PRINT("print supported commands");
    string HelpMessage = "";

    HelpMessage += "[Help]\n";
    HelpMessage += "1. write {LBA} {Data}\n";
    HelpMessage += "2. read {LBA}\n";
    HelpMessage += "3. exit\n";
    HelpMessage += "4. help\n";
    HelpMessage += "5. fullwrite {Data}\n";
    HelpMessage += "6. fullread\n";
    HelpMessage += "7. testapp1\n";
    HelpMessage += "8. testapp2\n";
    HelpMessage += "9. erase {LBA} {Size}\n";
    HelpMessage += "10. erase_range {LBA} {LBA}\n";
    HelpMessage += "11. flush\n";
    HelpMessage += "{LBA} = {x is an integer | 0 <= x <= 99}\n";
    HelpMessage += "{Data} = {""0x[0-9A-F]""}\n";
    cout << HelpMessage;
}

FullWriteCommand::FullWriteCommand(string strData) {
    this->strData = strData;
}

void FullWriteCommand::Run(SsdDriver* ssddriver) {
    LOG_PRINT("execute full write command");
    for (int LBA = ssddriver->GetMinLBA(); LBA <= ssddriver->GetMaxLBA(); LBA++) {
        ssddriver->Write(LBA, this->strData);
    }
}

void FullReadCommand::Run(SsdDriver* ssddriver) {
    LOG_PRINT("execute full read command");
    cout << "[FullRead]\n";
    for (int LBA = ssddriver->GetMinLBA(); LBA <= ssddriver->GetMaxLBA(); LBA++) {
        string resultData = ssddriver->Read(LBA);
        cout << "[Read] LBA : " << to_string(LBA);
        cout << ", Data : " << resultData << endl;
    }
}

void TestApp1::Run(SsdDriver* ssddriver) {
    LOG_PRINT("execute testapp1 scenario");
    cout << "[TestApp1]\n";
    for (int LBA = ssddriver->GetMinLBA(); LBA <= ssddriver->GetMaxLBA(); LBA++) {
        ssddriver->Write(LBA, "0x12341234");
    }
    for (int LBA = ssddriver->GetMinLBA(); LBA <= ssddriver->GetMaxLBA(); LBA++) {
        string resultData = ssddriver->Read(LBA);
        cout << "[Read] LBA : " << to_string(LBA);
        cout << ", Data : " << resultData << endl;
    }
}

void TestApp2::Run(SsdDriver* ssddriver) {
    LOG_PRINT("execute testapp2 scenario");
    cout << "[TestApp2]\n";
    for (unsigned int LBA = 0; LBA <= 5; LBA++) {
        for (unsigned int cnt = 1; cnt <= 5; cnt++) {
            ssddriver->Write(LBA, "0xAAAABBBB");
        }
    }

    for (unsigned int LBA = 0; LBA <= 5; LBA++) {
        ssddriver->Write(LBA, "0x12345678");
    }

    for (int LBA = 0; LBA <= 5; LBA++) {
        string resultData = ssddriver->Read(LBA);
        cout << "[Read] LBA : " << to_string(LBA);
        cout << ", Data : " << resultData << endl;
    }
}

void Compare::Run(SsdDriver* ssddriver) {
    LOG_PRINT("execute compare command");
    cout << "[Compare]\n";
    ssddriver->Flush();
    if (IsCompareFail(ssddriver))
    {
        cout << "Fail" << endl;
        throw ExceptionCompareFail();
    }

    cout << "Pass" << endl;
    return;
}

bool Compare::IsCompareFail(SsdDriver* ssddriver)
{
    return 0 == ssddriver->Compare();
}
