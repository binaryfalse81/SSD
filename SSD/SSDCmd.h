// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"
#include "SSD.h"
#include "Parser.h"
#include "Disk.h"

class SSDCmd
{
public:
    SSDCmd(Disk* disk, Parser* parser, PARSING_RESULT* pstParsingResult)
        : disk(disk), parser(parser), pstParsingResult(pstParsingResult) {}
    VOID Run(const string& strInputCmd);

private:
    Disk* disk;
    Parser* parser;
    PARSING_RESULT* pstParsingResult;
    VOID _Read();
    VOID _Write();
    VOID _Erase();
    VOID _Flush();
};
