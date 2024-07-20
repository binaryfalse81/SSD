// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"
#include "SSDDriver.h"

class ShellCommand
{
public:
     virtual VOID Run(SSDDriver* sd) = 0;
};


class InvalidCommand : public ShellCommand
{
public:
    InvalidCommand() {}
    VOID Run(SSDDriver* sd) override;
};

class WriteCommand : public ShellCommand
{
public:
    WriteCommand(string strLpn, string strPattern);
    VOID Run(SSDDriver* sd) override;

private:
    UINT32 nLpn;
    string strLpn;
    string strPattern;
};

class ReadCommand : public ShellCommand
{
public:
    explicit ReadCommand(string strLpn);
    VOID Run(SSDDriver* sd) override;

private:
    UINT32 nLpn;
    string strLpn;
};

class EraseCommand : public ShellCommand
{
public:
    EraseCommand(string strStartLpn, string strSize);
    VOID Run(SSDDriver* sd) override;

private:
    UINT32 nLpn;
    UINT32 nSize;
    string strLpn;
    string strSize;
};

class EraseRangeCommand : public ShellCommand
{
public:
    EraseRangeCommand(string strStartLpn, string strEndLpn);
    VOID Run(SSDDriver* sd) override;

private:
    UINT32 nLpnStart;
    UINT32 nLpnEnd;
    string strStartLpn;
    string strEndLpn;
};

class FlushCommand : public ShellCommand
{
public:
    FlushCommand() {}
    VOID Run(SSDDriver* sd) override;
};

class ExitCommand : public ShellCommand
{
public:
    ExitCommand() {}
    VOID Run(SSDDriver* sd) override;
};

class FailCommand : public ShellCommand
{
public:
    FailCommand() {}
    VOID Run(SSDDriver* sd) override;
};

class HelpCommand : public ShellCommand
{
public:
    HelpCommand() {}
    VOID Run(SSDDriver* sd) override;
};

class FullWriteCommand : public ShellCommand
{
public:
    explicit FullWriteCommand(string strPattern);
    VOID Run(SSDDriver* sd) override;

private:
    string strPattern;
};

class FullReadCommand : public ShellCommand
{
public:
    FullReadCommand() {}
    VOID Run(SSDDriver* sd) override;
};

class Compare : public ShellCommand
{
public:
    Compare() {}
    VOID Run(SSDDriver* sd) override;
    bool IsCompareFail(SSDDriver* sd);
};

class ExitTestShell : public exception
{
public:
    CHAR const* what() const override
    {
        return "Exit Test Shell!";
    }
};

class FailTestShell : public exception
{
public:
    CHAR const* what() const override
    {
        return "Fail Test Shell!";
    }
};

class ExceptionCompareFail : public exception
{
public:
    CHAR const* what() const override
    {
        return "Compare Fail";
    }
};
