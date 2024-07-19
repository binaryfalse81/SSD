// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include "SSDDriver.h"
#include "ShellCommandFactory.h"

using namespace std;

class Shell
{
public:
    void Run(const string& str);
    void SetSsdDriver(SSDDriver* sd);

private:
    SSDDriver* ssdDriver;
    ShellCommandFactory shellCommandFactory;
    ShellCommand* shellCommand;
};
