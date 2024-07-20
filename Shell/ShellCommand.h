// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"
#include "SSDDriver.h"

class ShellCommand
{
public:
     virtual VOID Run(SSDDriver* sd) = 0;

protected:
     const INT32 MAX_LBA = 99;
     const INT32 MIN_LBA = 0;
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
    WriteCommand(string strLBA, string strData);
    VOID Run(SSDDriver* sd) override;

private:
    INT32 nLBA;
    string strLBA;
    string strData;
};

class ReadCommand : public ShellCommand
{
public:
    explicit ReadCommand(string strLBA);
    VOID Run(SSDDriver* sd) override;

private:
    INT32 nLBA;
    string strLBA;
};

class EraseCommand : public ShellCommand
{
public:
    EraseCommand(string strStartLBA, string strSize);
    VOID Run(SSDDriver* sd) override;

private:
    INT32 nStartLBA;
    INT32 nSize;
    string strStartLBA;
    string strSize;
};

class EraseRangeCommand : public ShellCommand
{
public:
    EraseRangeCommand(string strStartLBA, string strEndLBA);
    VOID Run(SSDDriver* sd) override;

private:
    INT32 nStartLBA;
    INT32 nEndLBA;
    string strStartLBA;
    string strEndLBA;
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
    explicit FullWriteCommand(string strData);
    VOID Run(SSDDriver* sd) override;

private:
    string strData;
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
