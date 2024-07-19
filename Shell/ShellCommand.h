// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include "SSDDriver.h"

class ShellCommand
{
public:
     virtual void Run(SSDDriver* sd) = 0;

protected:
     const int MAX_LBA = 99;
     const int MIN_LBA = 0;
};


class InvalidCommand : public ShellCommand
{
public:
    InvalidCommand() {}
    void Run(SSDDriver* sd) override;
};

class WriteCommand : public ShellCommand
{
public:
    WriteCommand(string strLBA, string strData);
    void Run(SSDDriver* sd) override;

private:
    int nLBA;
    string strLBA;
    string strData;
};

class ReadCommand : public ShellCommand
{
public:
    explicit ReadCommand(string strLBA);
    void Run(SSDDriver* sd) override;

private:
    int nLBA;
    string strLBA;
};

class EraseCommand : public ShellCommand
{
public:
    EraseCommand(string strStartLBA, string strSize);
    void Run(SSDDriver* sd) override;

private:
    int nStartLBA;
    int nSize;
    string strStartLBA;
    string strSize;
};

class EraseRangeCommand : public ShellCommand
{
public:
    EraseRangeCommand(string strStartLBA, string strEndLBA);
    void Run(SSDDriver* sd) override;

private:
    int nStartLBA;
    int nEndLBA;
    string strStartLBA;
    string strEndLBA;
};

class FlushCommand : public ShellCommand
{
public:
    FlushCommand() {}
    void Run(SSDDriver* sd) override;
};

class ExitCommand : public ShellCommand
{
public:
    ExitCommand() {}
    void Run(SSDDriver* sd) override;
};

class FailCommand : public ShellCommand
{
public:
    FailCommand() {}
    void Run(SSDDriver* sd) override;
};

class HelpCommand : public ShellCommand
{
public:
    HelpCommand() {}
    void Run(SSDDriver* sd) override;
};

class FullWriteCommand : public ShellCommand
{
public:
    explicit FullWriteCommand(string strData);
    void Run(SSDDriver* sd) override;

private:
    string strData;
};

class FullReadCommand : public ShellCommand
{
public:
    FullReadCommand() {}
    void Run(SSDDriver* sd) override;
};

class Compare : public ShellCommand
{
public:
    Compare() {}
    void Run(SSDDriver* sd) override;
    bool IsCompareFail(SSDDriver* sd);
};

class ExitTestShell : public exception
{
public:
    char const* what() const override
    {
        return "Exit Test Shell!";
    }
};

class FailTestShell : public exception
{
public:
    char const* what() const override
    {
        return "Fail Test Shell!";
    }
};

class ExceptionCompareFail : public exception
{
public:
    char const* what() const override
    {
        return "Compare Fail";
    }
};
