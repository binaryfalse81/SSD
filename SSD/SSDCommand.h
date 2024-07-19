// Copyright [2024] <CRA/BestReviewer>
#pragma once
#include <vector>
#include <string>
#include "SSD.h"
#include "Parser.h"
#include "SSDInterface.h"

class SSDCommand {
 public:
    SSDCommand(SSDInterface* ssd, Parser* parser, CmdStatus* cmd)
        : ssd(ssd), parser(parser), cmd(cmd) {
    }
    void Run(const std::string& str);

 private:
    SSDInterface *ssd;
    Parser* parser;
    CmdStatus* cmd;
    void _Read();
    void _Write();
    void _Erase();
    void _Flush();
};
