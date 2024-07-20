// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"
#include "SSD.h"
#include "Parser.h"
#include "SSDInterface.h"

class SSDCommand
{
public:
    SSDCommand(SSDInterface* ssd, Parser* parser, CmdStatus* cmd)
        : ssd(ssd), parser(parser), cmd(cmd) {}
    VOID Run(const string& str);

private:
    SSDInterface *ssd;
    Parser* parser;
    CmdStatus* cmd;
    VOID _Read();
    VOID _Write();
    VOID _Erase();
    VOID _Flush();
};
