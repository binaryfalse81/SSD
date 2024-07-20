// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"
#include "SSDCommand.h"
#include "SSD.h"
#include "Parser.h"
#include "Disk.h"
#include "../Logger/Logger.cpp"

VOID SSDCommand::Run(const string& strCmd)
{
    pstCmdInfo = parser->Parse(strCmd);

    if (pstCmdInfo->eCmdType == ENUM_CMD_TYPE::Write)
    {
        _Write();
    }
    else if (pstCmdInfo->eCmdType == ENUM_CMD_TYPE::Read)
    {
        _Read();
    }
    else if (pstCmdInfo->eCmdType == ENUM_CMD_TYPE::Erase)
    {
        _Erase();
    }
    else if (pstCmdInfo->eCmdType == ENUM_CMD_TYPE::Flush)
    {
        _Flush();
    }
}

VOID SSDCommand::_Write()
{
    LOG_PRINT("send W cmd");
    disk->Write(stoi(pstCmdInfo->nLpn), pstCmdInfo->strPattern);
}

VOID SSDCommand::_Read()
{
    LOG_PRINT("send R cmd");
    disk->Read(stoi(pstCmdInfo->nLpn));
}

VOID SSDCommand::_Erase()
{
    LOG_PRINT("send E cmd");
    disk->Erase(stoi(pstCmdInfo->nLpn), pstCmdInfo->nEraseSize);
}

VOID SSDCommand::_Flush()
{
    LOG_PRINT("send F cmd");
    disk->Flush();
}
