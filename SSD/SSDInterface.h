// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"

class SSDInterface
{
public:
    virtual VOID Write(const INT32 &LBA, const string &data) = 0;
    virtual VOID Read(const INT32 &LBA) = 0;
    virtual VOID Erase(const INT32 &LBA, const INT32 &size) = 0;
    virtual VOID Flush() = 0;
};
