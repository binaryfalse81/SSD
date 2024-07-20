// Copyright.2024.binaryfalse81@gmail.com
#include "Header.h"
#include"SSD.h"
#include "../Logger/Logger.cpp"

string UNMAPED_PATTERN = "0x00000000";

class LpnRangeException : public exception {};
class DataRangeException : public exception {};
class DataPreFIxException : public exception {};
class DataTypeException : public exception {};
class EraseSizeException : public exception {};

VOID SSD::Write(const UINT32& nLpn, const string& strPattern)
{
    CheckWriteCondition(nLpn, strPattern);
    StoreCommand(nLpn, strPattern, INITIAL_UPDATE_SIZE);
}

VOID SSD::Read(const UINT32& nLpn)
{
    CheckLpnRange(nLpn);
    vector<string> readLine = FindLpnData(nLpn);
    WriteFile(ReadFileName, readLine);
}

VOID SSD::Erase(const UINT32& nLpn, const UINT32& nSize)
{
    CheckEraseCondition(nLpn, nSize);
    StoreCommand(nLpn, UNMAPED_PATTERN, nSize);
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

NAND_DATA SSD::ParseCmd(const string& line)
{
    NAND_DATA strPattern;
    INT32 firstSpacePosition = (INT32)line.find(' ');
    INT32 secondSpacePosition = (INT32)line.find(' ', firstSpacePosition + 1);
    strPattern.nLpn = stoi(line.substr(0, firstSpacePosition));
    if (secondSpacePosition == string::npos)
    {
        strPattern.nSize = 1;
        strPattern.strPattern = line.substr(firstSpacePosition + 1);
    }
    else
    {
        strPattern.strPattern = line.substr(firstSpacePosition + 1, secondSpacePosition - (firstSpacePosition + 1));
        strPattern.nSize = stoi(line.substr(secondSpacePosition + 1));
    }
    return strPattern;
}

vector<string> SSD::FindLpnData(const UINT32& nLpn)
{
    vector<string> lines = ReadFile(CommandBufferFileName);
    for (INT32 i = (INT32)lines.size() - 1; i >= 0; i--)
    {
        string line = lines[i];
        NAND_DATA stNandData = ParseCmd(line);
        if (IsInLpn(nLpn, stNandData))
            return { stNandData.strPattern };
    }

    ReadMemory();
    return { memory[nLpn] };
}

bool SSD::IsInLpn(const UINT32& nLpn, NAND_DATA& stNandData)
{
    return nLpn >= stNandData.nLpn && nLpn < (stNandData.nLpn + stNandData.nSize);
}

VOID SSD::StoreCommand(const UINT32& nLpn, const string& strPattern, const UINT32& nSize)
{
    LOG_PRINT("store new cmd into 'Command Buffer'");
    vector<string> lines = ReadFile(CommandBufferFileName);
    lines.push_back(to_string(nLpn) + " " + strPattern + " " + to_string(nSize));
    WriteFile(CommandBufferFileName, lines);
    CheckFlush((INT32)lines.size());
}

VOID SSD::CheckFlush(const INT32& bufferSize)
{
    if (bufferSize == MAX_CMD_BUF_CNT)
    {
        Flush();
    }
}

VOID SSD::ReadMemory()
{
    vector<string> lines = ReadFile(WriteFIleName);
    for (const auto &line : lines)
    {
        NAND_DATA stNandData = ParseCmd(line);
        UpdateMemory(stNandData.nLpn, stNandData.strPattern, stNandData.nSize);
    }

    if (lines.empty())
    {
        for (UINT32 i = 0; i < MAX_LPN; i++)
        {
            memory[i] = UNMAPED_PATTERN;
        }
    }
}

VOID SSD::UpdateMemory(const UINT32& nLpn, const string& strPattern, const UINT32& nSize)
{
    UINT32 endLpn = nLpn + nSize;
    endLpn = (endLpn >= MAX_LPN) ? MAX_LPN : endLpn;
    for (UINT32 i = nLpn; i < endLpn; i++)
    {
        memory[i] = strPattern;
    }
}

VOID SSD::CheckValidCommand(const vector<string> &lines)
{
    memset(IsUsedBuffer, 0, sizeof(IsUsedBuffer));
    validDataMap.clear();
    for (auto line_it = lines.rbegin(); line_it != lines.rend(); line_it++)
    {
        NAND_DATA stNandData = ParseCmd(*line_it);
        for (UINT32 i = stNandData.nLpn;  i < stNandData.nLpn + stNandData.nSize; i++)
        {
            if (!IsUsedBuffer[i])
            {
                validDataMap[i] = stNandData.strPattern;
                IsUsedBuffer[i] = 1;
            }
        }
    }
}

VOID SSD::RunValidCommand()
{
    for (UINT32 i = 0; i < MAX_LPN; i++)
    {
        if (IsUsedBuffer[i])
        {
            UpdateMemory(i, validDataMap[i], INITIAL_UPDATE_SIZE);
        }
    }
}

VOID SSD::StoreMemory()
{
    ofstream file(WriteFIleName);
    if (file.is_open())
    {
        for (UINT32 nLpn = 0; nLpn < MAX_LPN; nLpn++)
        {
            file << nLpn << " " << memory[nLpn] << "\n";
        }
        file.close();
    }
}

vector<string> SSD::ReadFile(const string& strFileName)
{
    vector<string> astrLines;
    ifstream file(strFileName);
    string strCmd;

    if (file.is_open())
    {
        while (getline(file, strCmd))
        {
            astrLines.push_back(strCmd);
        }
        file.close();
    }

    return astrLines;
}

VOID SSD::WriteFile(const string& strFileName, vector<string>& lines)
{
    ofstream file(strFileName);
    if (file.is_open())
    {
        for (const auto& line : lines)
        {
            file << line << "\n";
        }
        file.close();
    }
}

VOID SSD::CheckWriteCondition(const UINT32& nLpn, const string& strPattern)
{
    CheckLpnRange(nLpn);
    CheckDataLength(strPattern);
    CheckDataPreFix(strPattern);
    CheckDataType(strPattern);
}

VOID SSD::CheckEraseCondition(const UINT32& nLpn, const UINT32& nSize)
{
    CheckLpnRange(nLpn);
    CheckEraseSizeRange(nSize);
    CheckLpnRange(nLpn + nSize - 1);
}

VOID SSD::CheckLpnRange(const UINT32& nLpn)
{
    if (nLpn >= MAX_LPN)
        throw LpnRangeException();
}

VOID SSD::CheckDataLength(const string& strPattern)
{
    if (strPattern.length() != MAX_PATTERN_LENGTH)
        throw DataRangeException();
}

VOID SSD::CheckDataPreFix(const string& strPattern)
{
    if (strPattern.substr(0, 2) != DataPreFix)
        throw DataPreFIxException();
}

VOID SSD::CheckDataType(const string& strPattern)
{
    for (UINT32 i = 2; i < strPattern.length(); i++)
    {
        if (IsHexData(strPattern[i])) continue;
        throw DataTypeException();
    }
}

VOID SSD::CheckEraseSizeRange(const UINT32& nSize)
{
    if (nSize > 10)
        throw EraseSizeException();
}

bool SSD::IsHexData(const CHAR& ch)
{
    return (0 <= ch - '0' && ch - '0' < 10) || ('A' <= ch && ch <= 'F');
}
