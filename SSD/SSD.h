// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"
#include"Disk.h"

class SSD : public Disk
{
public:
    virtual VOID Write(const UINT32& nLpn, const string& strPattern) override;
    virtual VOID Read(const UINT32& nLpn) override;
    virtual VOID Erase(const UINT32& nLpn, const UINT32& nSize) override;
    virtual VOID Flush() override;

private:
    map<UINT32, string> memory;
    unordered_map<UINT32, string> validDataMap;
    UINT32 IsUsedBuffer[MAX_LPN];
    string WriteFIleName{ "nand.txt" };
    string ReadFileName{ "result.txt" };
    string CommandBufferFileName{ "buffer.txt" };
    string DataPreFix{ "0x" };

    NAND_DATA ParseCmd(const string &line);
    vector<string> FindLpnData(const UINT32 &nLpn);
    bool IsInLpn(const UINT32 &nLpn, NAND_DATA& bufferData);
    VOID StoreCommand(const UINT32& nLpn, const string& strPattern, const INT32& nSize);
    VOID CheckFlush(const INT32& bufferSize);
    VOID ReadMemory();
    VOID UpdateMemory(const UINT32& nLpn, const string& strPattern, const INT32& nSize);
    VOID UpdateMemoryWithBuffer(const vector<string> &lines);
    VOID UpdateMemoryWithCmd(const vector<string> &lines);
    VOID CheckValidCommand(const vector<string> &lines);
    VOID RunValidCommand();
    VOID StoreMemory();
    vector<string> ReadFile(const string& FileName);
    VOID WriteFile(const string& FileName, vector<string>& lines);
    VOID CheckWriteCondition(const UINT32& nLpn, const string& strPattern);
    VOID CheckEraseCondition(const UINT32& nLpn, const UINT32& nSize);
    VOID CheckLpnRange(const UINT32& nLpn);
    VOID CheckDataLength(const string& strPattern);
    VOID CheckDataPreFix(const string& strPattern);
    VOID CheckDataType(const string& strPattern);
    VOID CheckEraseSizeRange(const INT32& nSize);
    bool IsHexData(const CHAR& ch);
};
