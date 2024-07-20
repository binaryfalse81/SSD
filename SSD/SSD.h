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
    map<INT32, string> memory;
    unordered_map<INT32, string> validDataMap;
    INT32 isUsedBuffer[MAX_LPN];
    string WriteFIleName{ "nand.txt" };
    string ReadFileName{ "result.txt" };
    string CommandBufferFileName{ "buffer.txt" };
    string DataPreFix{ "0x" };
    const UINT32 InitialUpdateSize{1};
    const UINT32 Buffer_MAX_LINE{10};
    const UINT32 DATA_LENGTH{10};

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
    VOID CheckWriteCondition(const UINT32& nLpn, const string& data);
    VOID CheckEraseCondition(const UINT32& nLpn, const UINT32& nSize);
    VOID CheckLpnRange(const UINT32& nLpn);
    VOID CheckDataLength(const string& data);
    VOID CheckDataPreFix(const string& data);
    VOID CheckDataType(const string& data);
    VOID CheckEraseSizeRange(const INT32& nSize);
    bool isHexData(const CHAR& data);
};
