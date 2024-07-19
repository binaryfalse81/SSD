// Copyright [2024] <CRA/BestReviewer>
#include "SSDCommand.h"
#include <string>
#include <iostream>

std::string GetConcatenateArgv(int argc, char* argv[]) {
    std::string result{ "" };
    for (int i = 1; i < argc; i++) {
        result = result + " " + std::string(argv[i]);
    }
    return result;
}

int main(int argc, char* argv[]) {
    Parser parser;
    SSD ssd;
    CmdStatus status;
    SSDCommand cmd{ &ssd, &parser, &status };
    std::string arg{ argv[0] };

    arg += GetConcatenateArgv(argc, argv);
    try {
        cmd.Run(arg);
    }
    catch (...) {
        return -1;
    }

    return 0;
}
