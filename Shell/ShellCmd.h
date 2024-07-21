// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"
#include "SSDDriver.h"

class ShellCmd
{
public:
     virtual VOID Run(SSDDriver* sd) = 0;
};


class InvalidCommand : public ShellCmd
{
public:
    InvalidCommand() {}
    VOID Run(SSDDriver* sd) override;
};

class WriteCommand : public ShellCmd
{
public:
    WriteCommand(string strLpn, string strPattern);
    VOID Run(SSDDriver* sd) override;

private:
    UINT32 nLpn;
    string strLpn;
    string strPattern;
};

class ReadCommand : public ShellCmd
{
public:
    explicit ReadCommand(string strLpn);
    VOID Run(SSDDriver* sd) override;

private:
    UINT32 nLpn;
    string strLpn;
};

class EraseCommand : public ShellCmd
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

class EraseRangeCommand : public ShellCmd
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

class FlushCommand : public ShellCmd
{
public:
    FlushCommand() {}
    VOID Run(SSDDriver* sd) override;
};

class ExitCommand : public ShellCmd
{
public:
    ExitCommand() {}
    VOID Run(SSDDriver* sd) override;
};

class FailCommand : public ShellCmd
{
public:
    FailCommand() {}
    VOID Run(SSDDriver* sd) override;
};

class HelpCommand : public ShellCmd
{
public:
    HelpCommand() {}
    VOID Run(SSDDriver* sd) override;
};

class FullWriteCommand : public ShellCmd
{
public:
    explicit FullWriteCommand(string strPattern);
    VOID Run(SSDDriver* sd) override;

private:
    string strPattern;
};

class FullReadCommand : public ShellCmd
{
public:
    FullReadCommand() {}
    VOID Run(SSDDriver* sd) override;
};

class Compare : public ShellCmd
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
