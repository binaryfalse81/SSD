// Copyright [2024] <CRA/BestReviewer>
#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include "SsdDriver.h"

class ShellCommand {
 public:
     virtual void Run(SsdDriver* ssddriver) = 0;
 protected:
     const int MAX_LBA = 99;
     const int MIN_LBA = 0;
};


class InvalidCommand : public ShellCommand {
 public:
    InvalidCommand() {}
    void Run(SsdDriver* ssddriver) override;
};

class WriteCommand : public ShellCommand {
 public:
    WriteCommand(string strLBA, string strData);
    void Run(SsdDriver* ssddriver) override;
 private:
    int nLBA;
    string strLBA;
    string strData;
};

class ReadCommand : public ShellCommand {
 public:
    explicit ReadCommand(string strLBA);
    void Run(SsdDriver* ssddriver) override;
 private:
    int nLBA;
    string strLBA;
};

class EraseCommand : public ShellCommand {
public:
    EraseCommand(string strStartLBA, string strSize);
    void Run(SsdDriver* ssddriver) override;
private:
    int nStartLBA;
    int nSize;
    string strStartLBA;
    string strSize;
};

class EraseRangeCommand : public ShellCommand {
public:
    EraseRangeCommand(string strStartLBA, string strEndLBA);
    void Run(SsdDriver* ssddriver) override;
private:
    int nStartLBA;
    int nEndLBA;
    string strStartLBA;
    string strEndLBA;
};

class FlushCommand : public ShellCommand {
public:
    FlushCommand() {}
    void Run(SsdDriver* ssddriver) override;
};

class ExitCommand : public ShellCommand {
 public:
    ExitCommand() {}
    void Run(SsdDriver* ssddriver) override;
};

class FailCommand : public ShellCommand {
public:
    FailCommand() {}
    void Run(SsdDriver* ssddriver) override;
};

class HelpCommand : public ShellCommand {
 public:
    HelpCommand() {}
    void Run(SsdDriver* ssddriver) override;
};

class FullWriteCommand : public ShellCommand {
 public:
    explicit FullWriteCommand(string strData);
    void Run(SsdDriver* ssddriver) override;
 private:
    string strData;
};

class FullReadCommand : public ShellCommand {
 public:
    FullReadCommand() {}
    void Run(SsdDriver* ssddriver) override;
};

class TestApp1 : public ShellCommand {
 public:
    TestApp1() {}
    void Run(SsdDriver* ssddriver) override;
};

class TestApp2 : public ShellCommand {
 public:
    TestApp2() {}
    void Run(SsdDriver* ssddriver) override;
};

class Compare : public ShellCommand {
public:
    Compare() {}
    void Run(SsdDriver* ssddriver) override;
    bool IsCompareFail(SsdDriver* ssddriver);
};

class ExitTestShell : public exception {
 public:
    char const* what() const override {
        return "Exit Test Shell!";
    }
};

class FailTestShell : public exception {
public:
    char const* what() const override {
        return "Fail Test Shell!";
    }
};

class ExceptionCompareFail : public exception {
public:
    char const* what() const override {
        return "Compare Fail";
    }
};
