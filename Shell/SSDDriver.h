// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"

class SSDDriver
{
public:
    virtual string Read(UINT32 nLpn) = 0;
    virtual VOID Write(UINT32 nLpn, string strPattern) = 0;
    virtual VOID Erase(UINT32 nLpn, UINT32 nSize) = 0;
    virtual VOID Flush() = 0;
    virtual UINT32 Compare() = 0;
};
