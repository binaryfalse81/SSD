// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"
#include "SSDDriver.h"
#include "ShellCmdFactory.h"

class Shell
{
public:
    VOID Run(string str);
    VOID SetSsdDriver(SSDDriver* sd);

private:
    SSDDriver* sd;
    ShellCmdFactory scf;
    ShellCmd* sc;
};
