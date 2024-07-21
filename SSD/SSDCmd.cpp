// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"
#include "SSDCmd.h"
#include "SSD.h"
#include "Parser.h"
#include "Disk.h"
#include "../Logger/Logger.cpp"

VOID SSDCmd::Run(const string& strInputCmd)
{
    pstParsingResult = parser->Parse(strInputCmd);

    if (pstParsingResult->eCmdType == ENUM_CMD_TYPE::Write)
    {
        _Write();
    }
    else if (pstParsingResult->eCmdType == ENUM_CMD_TYPE::Read)
    {
        _Read();
    }
    else if (pstParsingResult->eCmdType == ENUM_CMD_TYPE::Erase)
    {
        _Erase();
    }
    else if (pstParsingResult->eCmdType == ENUM_CMD_TYPE::Flush)
    {
        _Flush();
    }
}

VOID SSDCmd::_Write()
{
    LOG_PRINT("send W cmd");
    disk->Write(stoi(pstParsingResult->nLpn), pstParsingResult->strPattern);
}

VOID SSDCmd::_Read()
{
    LOG_PRINT("send R cmd");
    disk->Read(stoi(pstParsingResult->nLpn));
}

VOID SSDCmd::_Erase()
{
    LOG_PRINT("send E cmd");
    disk->Erase(stoi(pstParsingResult->nLpn), pstParsingResult->nEraseSize);
}

VOID SSDCmd::_Flush()
{
    LOG_PRINT("send F cmd");
    disk->Flush();
}
