// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"
#include "SSD.h"
#include "Parser.h"
#include "Disk.h"

class SSDCommand
{
public:
    SSDCommand(Disk* disk, Parser* parser, CMD_INFO* pstCmdInfo)
        : disk(disk), parser(parser), pstCmdInfo(pstCmdInfo) {}
    VOID Run(const string& str);

private:
    Disk* disk;
    Parser* parser;
    CMD_INFO* pstCmdInfo;
    VOID _Read();
    VOID _Write();
    VOID _Erase();
    VOID _Flush();
};
