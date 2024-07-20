// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"
#include "SSDDriver.h"
#include "ShellCommandFactory.h"

class Shell
{
public:
    VOID Run(const string& str);
    VOID SetSsdDriver(SSDDriver* sd);

private:
    SSDDriver* ssdDriver;
    ShellCommandFactory shellCommandFactory;
    ShellCommand* shellCommand;
};
