// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include <string>
using namespace std;

class SSDDriver
{
public:
    virtual string Read(int LBA) = 0;
    virtual void Write(int LBA, string Data) = 0;
    virtual void Erase(int startLBA, int Size) = 0;
    virtual void Flush() = 0;
    virtual unsigned int Compare() = 0;

    virtual int GetMinLBA() = 0;
    virtual int GetMaxLBA() = 0;
};
