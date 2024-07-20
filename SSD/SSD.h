// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"
#include"SSDInterface.h"

typedef struct
{
    INT32 LBA;
    string LBAData;
    INT32 LBASize;
} CmdContent;

class SSD : public SSDInterface
{
public:
    virtual VOID Write(const INT32& LBA, const string& data) override;
    virtual VOID Read(const INT32 &LBA) override;
    virtual VOID Erase(const INT32 &LBA, const INT32 &size) override;
    virtual VOID Flush() override;

private:
    map<INT32, string> memory;
    unordered_map<INT32, string> validDataMap;
    INT32 isUsedBuffer[100];
    string InitialLBAData{ "0x00000000" };
    string WriteFIleName{ "nand.txt" };
    string ReadFileName{ "result.txt" };
    string CommandBufferFileName{ "buffer.txt" };
    string DataPreFix{ "0x" };
    const INT32 MAX_LBA{ 99 };
    const INT32 MIN_LBA{ 0 };
    const INT32 InitialUpdateSize{1};
    const INT32 Buffer_MAX_LINE{10};
    const INT32 DATA_LENGTH{10};

    CmdContent ParseCmd(const string &line);
    vector<string> FindLBAData(const INT32 &LBA);
    bool IsInLBA(const INT32 &LBA, CmdContent &bufferData);
    VOID StoreCommand(const INT32 &LBA, const string &data, const INT32 &size);
    VOID CheckFlush(const INT32& bufferSize);
    VOID ReadMemory();
    VOID UpdateMemory(const INT32 &LBA, const string &data, const INT32 &size);
    VOID UpdateMemoryWithBuffer(const vector<string> &lines);
    VOID UpdateMemoryWithCmd(const vector<string> &lines);
    VOID CheckValidCommand(const vector<string> &lines);
    VOID RunValidCommand();
    VOID StoreMemory();
    vector<string> ReadFile(const string& FileName);
    VOID WriteFile(const string& FileName, vector<string>& lines);
    VOID CheckWriteCondition(const INT32& LBA, const string& data);
    VOID CheckEraseCondition(const INT32& LBA, const INT32& size);
    VOID CheckLBARange(const INT32& LBA);
    VOID CheckDataLength(const string& data);
    VOID CheckDataPreFix(const string& data);
    VOID CheckDataType(const string& data);
    VOID CheckEraseSizeRange(const INT32 &size);
    bool isHexData(const CHAR& data);
};
