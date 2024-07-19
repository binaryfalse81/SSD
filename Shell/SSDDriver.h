// Copyright [2024] <CRA/BestReviewer>
#pragma once
#include <string>
using namespace std;

class SsdDriver {
 public:
    virtual string Read(int LBA) = 0;
    virtual void Write(int LBA, string Data) = 0;
    virtual void Erase(int startLBA, int Size) = 0;
    virtual void Flush() = 0;
    virtual unsigned int Compare() = 0;

    virtual int GetMinLBA() = 0;
    virtual int GetMaxLBA() = 0;
};
