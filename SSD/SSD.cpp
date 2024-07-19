// Copyright [2024] <CRA/BestReviewer>
#include<stdexcept>
#include<fstream>
#include<iostream>
#include <sstream>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include"SSD.h"
#include "../Logger/logger.cpp"

class LBARangeException : public std::exception {};
class DataRangeException : public std::exception {};
class DataPreFIxException : public std::exception {};
class DataTypeException : public std::exception {};
class EraseSizeException : public std::exception {};

void SSD::Write(const int& LBA, const std::string& data) {
    CheckWriteCondition(LBA, data);
    StoreCommand(LBA, data, InitialUpdateSize);
}

void SSD::Read(const int& LBA) {
    CheckLBARange(LBA);
    std::vector<std::string> readLine = FindLBAData(LBA);
    WriteFile(ReadFileName, readLine);
}

void SSD::Erase(const int& LBA, const int& size) {
    CheckEraseCondition(LBA, size);
    StoreCommand(LBA, InitialLBAData, size);
}

void SSD::Flush() {
    ReadMemory();
    std::vector<std::string> lines = ReadFile(CommandBufferFileName);
    CheckValidCommand(lines);
    RunValidCommand();
    remove(CommandBufferFileName.c_str());
    StoreMemory();
}

CmdContent SSD::ParseCmd(const std::string& line) {
    CmdContent LBAData;
    int firstSpacePosition = line.find(' ');
    int secondSpacePosition = line.find(' ', firstSpacePosition + 1);
    LBAData.LBA = stoi(line.substr(0, firstSpacePosition));
    if (secondSpacePosition == std::string::npos) {
        LBAData.LBASize = 1;
        LBAData.LBAData = line.substr(firstSpacePosition + 1);
    } else {
        LBAData.LBAData = line.substr(
        firstSpacePosition + 1, secondSpacePosition - (firstSpacePosition + 1));
        LBAData.LBASize = stoi(line.substr(secondSpacePosition + 1));
    }
    return LBAData;
}

std::vector<std::string> SSD::FindLBAData(const int& LBA) {
    std::vector<std::string> lines = ReadFile(CommandBufferFileName);
    for (int line_index = lines.size() - 1; line_index >= 0; line_index--) {
        std::string line = lines[line_index];
        CmdContent bufferData = ParseCmd(line);
        if (IsInLBA(LBA, bufferData))
            return { bufferData.LBAData };
    }

    ReadMemory();
    return { memory[LBA] };
}

bool SSD::IsInLBA(const int& LBA, CmdContent& bufferData) {
    return LBA >= bufferData.LBA && LBA < (bufferData.LBA + bufferData.LBASize);
}

void SSD::StoreCommand(const int& LBA, const std::string& data, const int& size) {
    LOG_PRINT("store new cmd into 'Command Buffer'");
    std::vector<std::string> lines = ReadFile(CommandBufferFileName);
    lines.push_back(std::to_string(LBA) + " " + data + " " + std::to_string(size));
    WriteFile(CommandBufferFileName, lines);
    CheckFlush(lines.size());
}

void SSD::CheckFlush(const int& bufferSize) {
    if (bufferSize == Buffer_MAX_LINE) {
        Flush();
    }
}

void SSD::ReadMemory() {
    std::vector<std::string> lines = ReadFile(WriteFIleName);
    for (const auto &line : lines) {
        CmdContent bufferData = ParseCmd(line);
        UpdateMemory(bufferData.LBA, bufferData.LBAData, bufferData.LBASize);
    }

    if (lines.empty()) {
        for (int i = 0; i <= MAX_LBA; i++) {
            memory[i] = InitialLBAData;
        }
    }
}

void SSD::UpdateMemory(const int& LBA, const std::string& data, const int& size) {
    int endLBA = LBA + size;
    endLBA = endLBA > MAX_LBA ? MAX_LBA + 1 : endLBA;
    for (int iLBA = LBA; iLBA < endLBA; iLBA++) {
        memory[iLBA] = data;
    }
}

void SSD::CheckValidCommand(const std::vector<std::string> &lines) {
    std::memset(isUsedBuffer, 0, sizeof(isUsedBuffer));
    validDataMap.clear();
    for (auto line_it = lines.rbegin(); line_it != lines.rend(); line_it++) {
        CmdContent bufferData = ParseCmd(*line_it);
        for (int LBA_i = bufferData.LBA;  LBA_i < bufferData.LBA + bufferData.LBASize; LBA_i++) {
            if (!isUsedBuffer[LBA_i]) {
                validDataMap[LBA_i] = bufferData.LBAData;
                isUsedBuffer[LBA_i] = 1;
            }
        }
    }
}

void SSD::RunValidCommand() {
    for (int LBA_i = MIN_LBA; LBA_i < MAX_LBA + 1; LBA_i++) {
        if (isUsedBuffer[LBA_i]) {
            UpdateMemory(LBA_i, validDataMap[LBA_i], InitialUpdateSize);
        }
    }
}

void SSD::StoreMemory() {
    std::ofstream file(WriteFIleName);
    if (file.is_open()) {
        for (int LBA = 0; LBA <= MAX_LBA; LBA++) {
            file << LBA << " " << memory[LBA] << "\n";
        }
        file.close();
    }
}

std::vector<std::string> SSD::ReadFile(const std::string& FileName) {
    std::vector<std::string> lines;
    std::ifstream file(FileName);
    std::string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            lines.push_back(line);
        }
        file.close();
    }

    return lines;
}

void SSD::WriteFile(const std::string& FileName, std::vector<std::string>& lines) {
    std::ofstream file(FileName);
    if (file.is_open()) {
        for (const auto& line : lines) {
            file << line << "\n";
        }
        file.close();
    }
}

void SSD::CheckWriteCondition(const int& LBA, const std::string& data) {
    CheckLBARange(LBA);
    CheckDataLength(data);
    CheckDataPreFix(data);
    CheckDataType(data);
}

void SSD::CheckEraseCondition(const int& LBA, const int& size) {
    CheckLBARange(LBA);
    CheckEraseSizeRange(size);
    CheckLBARange(LBA + size - 1);
}

void SSD::CheckLBARange(const int& LBA) {
    if (LBA < 0 || LBA > MAX_LBA)
        throw LBARangeException();
}

void SSD::CheckDataLength(const std::string& data) {
    if (data.length() != DATA_LENGTH)
        throw DataRangeException();
}

void SSD::CheckDataPreFix(const std::string& data) {
    if (data.substr(0, 2) != DataPreFix)
        throw DataPreFIxException();
}

void SSD::CheckDataType(const std::string& data) {
    for (int i = 2; i < data.length(); i++) {
        if (isHexData(data[i]))continue;
        throw DataTypeException();
    }
}

void SSD::CheckEraseSizeRange(const int& size) {
    if (size > 10 || size < 0)
        throw EraseSizeException();
}

bool SSD::isHexData(const char& data) {
    return (0 <= data - '0' && data - '0' < 10)
        || ('A' <= data && data <= 'F');
}
