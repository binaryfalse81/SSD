// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include<string>

using namespace std;

class SSDInterface
{
public:
    virtual void Write(const int &LBA, const string &data) = 0;
    virtual void Read(const int &LBA) = 0;
    virtual void Erase(const int &LBA, const int &size) = 0;
    virtual void Flush() = 0;
};
