// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include"SSDInterface.h"

typedef struct
{
    int LBA;
    string LBAData;
    int LBASize;
} CmdContent;

class SSD : public SSDInterface
{
public:
    virtual void Write(const int& LBA, const string& data) override;
    virtual void Read(const int &LBA) override;
    virtual void Erase(const int &LBA, const int &size) override;
    virtual void Flush() override;

private:
    map<int, string> memory;
    unordered_map<int, string> validDataMap;
    int isUsedBuffer[100];
    string InitialLBAData{ "0x00000000" };
    string WriteFIleName{ "nand.txt" };
    string ReadFileName{ "result.txt" };
    string CommandBufferFileName{ "buffer.txt" };
    string DataPreFix{ "0x" };
    const int MAX_LBA{ 99 };
    const int MIN_LBA{ 0 };
    const int InitialUpdateSize{1};
    const int Buffer_MAX_LINE{10};
    const int DATA_LENGTH{10};

    CmdContent ParseCmd(const string &line);
    vector<string> FindLBAData(const int &LBA);
    bool IsInLBA(const int &LBA, CmdContent &bufferData);
    void StoreCommand(const int &LBA, const string &data, const int &size);
    void CheckFlush(const int& bufferSize);
    void ReadMemory();
    void UpdateMemory(const int &LBA, const string &data, const int &size);
    void UpdateMemoryWithBuffer(const vector<string> &lines);
    void UpdateMemoryWithCmd(const vector<string> &lines);
    void CheckValidCommand(const vector<string> &lines);
    void RunValidCommand();
    void StoreMemory();
    vector<string> ReadFile(const string& FileName);
    void WriteFile(const string& FileName, vector<string>& lines);
    void CheckWriteCondition(const int& LBA, const string& data);
    void CheckEraseCondition(const int& LBA, const int& size);
    void CheckLBARange(const int& LBA);
    void CheckDataLength(const string& data);
    void CheckDataPreFix(const string& data);
    void CheckDataType(const string& data);
    void CheckEraseSizeRange(const int &size);
    bool isHexData(const char& data);
};
