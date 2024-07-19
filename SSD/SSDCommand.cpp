// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "SSDCommand.h"
#include "SSD.h"
#include "Parser.h"
#include "SSDInterface.h"
#include "../Logger/Logger.cpp"

void SSDCommand::Run(const string& strCommand)
{
    cmd = parser->Parse(strCommand);

    if (cmd->Command == CmdType::Write)
    {
        _Write();
    }
    else if (cmd->Command == CmdType::Read)
    {
        _Read();
    }
    else if (cmd->Command == CmdType::Erase)
    {
        _Erase();
    }
    else if (cmd->Command == CmdType::Flush)
    {
        _Flush();
    }
}

void SSDCommand::_Write()
{
    LOG_PRINT("send W cmd");
    ssd->Write(stoi(cmd->LBA), cmd->LBAData);
}

void SSDCommand::_Read()
{
    LOG_PRINT("send R cmd");
    ssd->Read(stoi(cmd->LBA));
}

void SSDCommand::_Erase()
{
    LOG_PRINT("send E cmd");
    ssd->Erase(stoi(cmd->LBA), cmd->EraseSize);
}

void SSDCommand::_Flush()
{
    LOG_PRINT("send F cmd");
    ssd->Flush();
}
