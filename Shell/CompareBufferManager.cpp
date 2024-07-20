#include "Header.h"
#include "CompareBufferManager.h"
#include "../Logger/Logger.cpp"

VOID CompareBufferMgr::SetCompareData(INT32 LBA, string Data)
{
    if (0 <= LBA && LBA < CONFIG_MAX_LBA)
    {
        compareData[LBA] = Data;
    }
}

bool CompareBufferMgr::CompareBuf()
{
    string ReadFileName{ "nand.txt" };
    ifstream resultFile(ReadFileName);
    string line;
    INT32 LBA = 0;

    if (resultFile.is_open())
    {
        while (getline(resultFile, line))
        {
            LBA_INFO LbaInfo = Parse(line);
            if (LbaInfo.LBAData != compareData[LBA])
            {
                return false;
            }

            LBA++;
            if (LBA >= CONFIG_MAX_LBA)
            {
                break;
            }
        }
        resultFile.close();
    }
    else
    {
        for (LBA = 0; LBA < CONFIG_MAX_LBA; LBA++)
        {
            if (compareData[LBA] != ERASE_DATA)
            {
                return false;
            }
        }
    }

    return true;
}

LBA_INFO CompareBufferMgr::Parse(const string& line)
{
    LBA_INFO LbaInfo;
    INT32 firstSpacePos = (INT32)line.find(' ');
    INT32 secondSpacePos = (INT32)line.find(' ', firstSpacePos + 1);
    LbaInfo.LBA = stoi(line.substr(0, firstSpacePos));
    if (secondSpacePos == string::npos)
    {
        LbaInfo.LBASize = 1;
        LbaInfo.LBAData = line.substr(firstSpacePos + 1);
    }
    else
    {
        LbaInfo.LBAData = line.substr(firstSpacePos + 1, secondSpacePos - (firstSpacePos + 1));
        LbaInfo.LBASize = stoi(line.substr(secondSpacePos + 1));
    }
    return LbaInfo;
}
