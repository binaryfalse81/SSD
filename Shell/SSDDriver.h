// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"

class SSDDriver
{
public:
    virtual string Read(INT32 LBA) = 0;
    virtual VOID Write(INT32 LBA, string Data) = 0;
    virtual VOID Erase(INT32 startLBA, INT32 Size) = 0;
    virtual VOID Flush() = 0;
    virtual UINT32 Compare() = 0;
    virtual INT32 GetMinLBA() = 0;
    virtual INT32 GetMaxLBA() = 0;
};
