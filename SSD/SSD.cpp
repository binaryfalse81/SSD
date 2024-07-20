// Copyright.2024.binaryfalse81@gmail.com
#include "Header.h"
#include"SSD.h"
#include "../Logger/Logger.cpp"

class LBARangeException : public exception {};
class DataRangeException : public exception {};
class DataPreFIxException : public exception {};
class DataTypeException : public exception {};
class EraseSizeException : public exception {};

VOID SSD::Write(const INT32& LBA, const string& data)
{
    CheckWriteCondition(LBA, data);
    StoreCommand(LBA, data, InitialUpdateSize);
}

VOID SSD::Read(const INT32& LBA)
{
    CheckLBARange(LBA);
    vector<string> readLine = FindLBAData(LBA);
    WriteFile(ReadFileName, readLine);
}

VOID SSD::Erase(const INT32& LBA, const INT32& size)
{
    CheckEraseCondition(LBA, size);
    StoreCommand(LBA, InitialLBAData, size);
}

VOID SSD::Flush()
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
    INT32 firstSpacePosition = (INT32)line.find(' ');
    INT32 secondSpacePosition = (INT32)line.find(' ', firstSpacePosition + 1);
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

vector<string> SSD::FindLBAData(const INT32& LBA)
{
    vector<string> lines = ReadFile(CommandBufferFileName);
    for (INT32 line_index = (INT32)lines.size() - 1; line_index >= 0; line_index--)
    {
        string line = lines[line_index];
        CmdContent bufferData = ParseCmd(line);
        if (IsInLBA(LBA, bufferData))
            return { bufferData.LBAData };
    }

    ReadMemory();
    return { memory[LBA] };
}

bool SSD::IsInLBA(const INT32& LBA, CmdContent& bufferData)
{
    return LBA >= bufferData.LBA && LBA < (bufferData.LBA + bufferData.LBASize);
}

VOID SSD::StoreCommand(const INT32& LBA, const string& data, const INT32& size)
{
    LOG_PRINT("store new cmd into 'Command Buffer'");
    vector<string> lines = ReadFile(CommandBufferFileName);
    lines.push_back(to_string(LBA) + " " + data + " " + to_string(size));
    WriteFile(CommandBufferFileName, lines);
    CheckFlush((INT32)lines.size());
}

VOID SSD::CheckFlush(const INT32& bufferSize)
{
    if (bufferSize == Buffer_MAX_LINE)
    {
        Flush();
    }
}

VOID SSD::ReadMemory()
{
    vector<string> lines = ReadFile(WriteFIleName);
    for (const auto &line : lines)
    {
        CmdContent bufferData = ParseCmd(line);
        UpdateMemory(bufferData.LBA, bufferData.LBAData, bufferData.LBASize);
    }

    if (lines.empty())
    {
        for (INT32 i = 0; i <= MAX_LBA; i++)
        {
            memory[i] = InitialLBAData;
        }
    }
}

VOID SSD::UpdateMemory(const INT32& LBA, const string& data, const INT32& size)
{
    INT32 endLBA = LBA + size;
    endLBA = endLBA > MAX_LBA ? MAX_LBA + 1 : endLBA;
    for (INT32 iLBA = LBA; iLBA < endLBA; iLBA++)
    {
        memory[iLBA] = data;
    }
}

VOID SSD::CheckValidCommand(const vector<string> &lines)
{
    memset(isUsedBuffer, 0, sizeof(isUsedBuffer));
    validDataMap.clear();
    for (auto line_it = lines.rbegin(); line_it != lines.rend(); line_it++)
    {
        CmdContent bufferData = ParseCmd(*line_it);
        for (INT32 LBA_i = bufferData.LBA;  LBA_i < bufferData.LBA + bufferData.LBASize; LBA_i++)
        {
            if (!isUsedBuffer[LBA_i])
            {
                validDataMap[LBA_i] = bufferData.LBAData;
                isUsedBuffer[LBA_i] = 1;
            }
        }
    }
}

VOID SSD::RunValidCommand()
{
    for (INT32 LBA_i = MIN_LBA; LBA_i < MAX_LBA + 1; LBA_i++)
    {
        if (isUsedBuffer[LBA_i])
        {
            UpdateMemory(LBA_i, validDataMap[LBA_i], InitialUpdateSize);
        }
    }
}

VOID SSD::StoreMemory()
{
    ofstream file(WriteFIleName);
    if (file.is_open())
    {
        for (INT32 LBA = 0; LBA <= MAX_LBA; LBA++)
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

VOID SSD::WriteFile(const string& FileName, vector<string>& lines)
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

VOID SSD::CheckWriteCondition(const INT32& LBA, const string& data)
{
    CheckLBARange(LBA);
    CheckDataLength(data);
    CheckDataPreFix(data);
    CheckDataType(data);
}

VOID SSD::CheckEraseCondition(const INT32& LBA, const INT32& size)
{
    CheckLBARange(LBA);
    CheckEraseSizeRange(size);
    CheckLBARange(LBA + size - 1);
}

VOID SSD::CheckLBARange(const INT32& LBA)
{
    if (LBA < 0 || LBA > MAX_LBA)
        throw LBARangeException();
}

VOID SSD::CheckDataLength(const string& data)
{
    if (data.length() != DATA_LENGTH)
        throw DataRangeException();
}

VOID SSD::CheckDataPreFix(const string& data)
{
    if (data.substr(0, 2) != DataPreFix)
        throw DataPreFIxException();
}

VOID SSD::CheckDataType(const string& data)
{
    for (INT32 i = 2; i < data.length(); i++)
    {
        if (isHexData(data[i]))continue;
        throw DataTypeException();
    }
}

VOID SSD::CheckEraseSizeRange(const INT32& size)
{
    if (size > 10 || size < 0)
        throw EraseSizeException();
}

bool SSD::isHexData(const CHAR& data)
{
    return (0 <= data - '0' && data - '0' < 10)
        || ('A' <= data && data <= 'F');
}
