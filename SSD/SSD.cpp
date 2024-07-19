// Copyright.2024.binaryfalse81@gmail.com
#include<stdexcept>
#include<fstream>
#include<iostream>
#include <sstream>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include"SSD.h"
#include "../Logger/Logger.cpp"

class LBARangeException : public exception {};
class DataRangeException : public exception {};
class DataPreFIxException : public exception {};
class DataTypeException : public exception {};
class EraseSizeException : public exception {};

void SSD::Write(const int& LBA, const string& data)
{
    CheckWriteCondition(LBA, data);
    StoreCommand(LBA, data, InitialUpdateSize);
}

void SSD::Read(const int& LBA)
{
    CheckLBARange(LBA);
    vector<string> readLine = FindLBAData(LBA);
    WriteFile(ReadFileName, readLine);
}

void SSD::Erase(const int& LBA, const int& size)
{
    CheckEraseCondition(LBA, size);
    StoreCommand(LBA, InitialLBAData, size);
}

void SSD::Flush()
{
    ReadMemory();
    vector<string> lines = ReadFile(CommandBufferFileName);
    CheckValidCommand(lines);
    RunValidCommand();
    remove(CommandBufferFileName.c_str());
    StoreMemory();
}

CmdContent SSD::ParseCmd(const string& line)
{
    CmdContent LBAData;
    int firstSpacePosition = (int)line.find(' ');
    int secondSpacePosition = (int)line.find(' ', firstSpacePosition + 1);
    LBAData.LBA = stoi(line.substr(0, firstSpacePosition));
    if (secondSpacePosition == string::npos)
    {
        LBAData.LBASize = 1;
        LBAData.LBAData = line.substr(firstSpacePosition + 1);
    }
    else
    {
        LBAData.LBAData = line.substr(firstSpacePosition + 1, secondSpacePosition - (firstSpacePosition + 1));
        LBAData.LBASize = stoi(line.substr(secondSpacePosition + 1));
    }
    return LBAData;
}

vector<string> SSD::FindLBAData(const int& LBA)
{
    vector<string> lines = ReadFile(CommandBufferFileName);
    for (int line_index = (int)lines.size() - 1; line_index >= 0; line_index--)
    {
        string line = lines[line_index];
        CmdContent bufferData = ParseCmd(line);
        if (IsInLBA(LBA, bufferData))
            return { bufferData.LBAData };
    }

    ReadMemory();
    return { memory[LBA] };
}

bool SSD::IsInLBA(const int& LBA, CmdContent& bufferData)
{
    return LBA >= bufferData.LBA && LBA < (bufferData.LBA + bufferData.LBASize);
}

void SSD::StoreCommand(const int& LBA, const string& data, const int& size)
{
    LOG_PRINT("store new cmd into 'Command Buffer'");
    vector<string> lines = ReadFile(CommandBufferFileName);
    lines.push_back(to_string(LBA) + " " + data + " " + to_string(size));
    WriteFile(CommandBufferFileName, lines);
    CheckFlush((int)lines.size());
}

void SSD::CheckFlush(const int& bufferSize)
{
    if (bufferSize == Buffer_MAX_LINE)
    {
        Flush();
    }
}

void SSD::ReadMemory()
{
    vector<string> lines = ReadFile(WriteFIleName);
    for (const auto &line : lines)
    {
        CmdContent bufferData = ParseCmd(line);
        UpdateMemory(bufferData.LBA, bufferData.LBAData, bufferData.LBASize);
    }

    if (lines.empty())
    {
        for (int i = 0; i <= MAX_LBA; i++)
        {
            memory[i] = InitialLBAData;
        }
    }
}

void SSD::UpdateMemory(const int& LBA, const string& data, const int& size)
{
    int endLBA = LBA + size;
    endLBA = endLBA > MAX_LBA ? MAX_LBA + 1 : endLBA;
    for (int iLBA = LBA; iLBA < endLBA; iLBA++)
    {
        memory[iLBA] = data;
    }
}

void SSD::CheckValidCommand(const vector<string> &lines)
{
    memset(isUsedBuffer, 0, sizeof(isUsedBuffer));
    validDataMap.clear();
    for (auto line_it = lines.rbegin(); line_it != lines.rend(); line_it++)
    {
        CmdContent bufferData = ParseCmd(*line_it);
        for (int LBA_i = bufferData.LBA;  LBA_i < bufferData.LBA + bufferData.LBASize; LBA_i++)
        {
            if (!isUsedBuffer[LBA_i])
            {
                validDataMap[LBA_i] = bufferData.LBAData;
                isUsedBuffer[LBA_i] = 1;
            }
        }
    }
}

void SSD::RunValidCommand()
{
    for (int LBA_i = MIN_LBA; LBA_i < MAX_LBA + 1; LBA_i++)
    {
        if (isUsedBuffer[LBA_i])
        {
            UpdateMemory(LBA_i, validDataMap[LBA_i], InitialUpdateSize);
        }
    }
}

void SSD::StoreMemory()
{
    ofstream file(WriteFIleName);
    if (file.is_open())
    {
        for (int LBA = 0; LBA <= MAX_LBA; LBA++)
        {
            file << LBA << " " << memory[LBA] << "\n";
        }
        file.close();
    }
}

vector<string> SSD::ReadFile(const string& FileName)
{
    vector<string> lines;
    ifstream file(FileName);
    string line;

    if (file.is_open())
    {
        while (getline(file, line))
        {
            lines.push_back(line);
        }
        file.close();
    }

    return lines;
}

void SSD::WriteFile(const string& FileName, vector<string>& lines)
{
    ofstream file(FileName);
    if (file.is_open())
    {
        for (const auto& line : lines)
        {
            file << line << "\n";
        }
        file.close();
    }
}

void SSD::CheckWriteCondition(const int& LBA, const string& data)
{
    CheckLBARange(LBA);
    CheckDataLength(data);
    CheckDataPreFix(data);
    CheckDataType(data);
}

void SSD::CheckEraseCondition(const int& LBA, const int& size)
{
    CheckLBARange(LBA);
    CheckEraseSizeRange(size);
    CheckLBARange(LBA + size - 1);
}

void SSD::CheckLBARange(const int& LBA)
{
    if (LBA < 0 || LBA > MAX_LBA)
        throw LBARangeException();
}

void SSD::CheckDataLength(const string& data)
{
    if (data.length() != DATA_LENGTH)
        throw DataRangeException();
}

void SSD::CheckDataPreFix(const string& data)
{
    if (data.substr(0, 2) != DataPreFix)
        throw DataPreFIxException();
}

void SSD::CheckDataType(const string& data)
{
    for (int i = 2; i < data.length(); i++)
    {
        if (isHexData(data[i]))continue;
        throw DataTypeException();
    }
}

void SSD::CheckEraseSizeRange(const int& size)
{
    if (size > 10 || size < 0)
        throw EraseSizeException();
}

bool SSD::isHexData(const char& data)
{
    return (0 <= data - '0' && data - '0' < 10)
        || ('A' <= data && data <= 'F');
}
