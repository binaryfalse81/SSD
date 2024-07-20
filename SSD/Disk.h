// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"

class Disk
{
public:
    virtual VOID Write(const UINT32& nLpn, const string& strPattern) = 0;
    virtual VOID Read(const UINT32& nLpn) = 0;
    virtual VOID Erase(const UINT32& nLpn, const UINT32& nSize) = 0;
    virtual VOID Flush() = 0;
};
