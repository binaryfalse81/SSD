#include "Header.h"
#include "CompareBufferManager.h"
#include "../Logger/Logger.cpp"

VOID CompareBufferMgr::SetCompareData(UINT32 nLpn, string strPattern)
{
    if (nLpn < MAX_LPN)
    {
        compareData[nLpn] = strPattern;
    }
}

bool CompareBufferMgr::CompareBuf()
{
    string ReadFileName{ "nand.txt" };
    ifstream resultFile(ReadFileName);
    string line;
    UINT32 nLpn = 0;

    if (resultFile.is_open())
    {
        while (getline(resultFile, line))
        {
            NAND_DATA LbaInfo = Parse(line);
            if (LbaInfo.strPattern != compareData[nLpn])
            {
                return false;
            }

            nLpn++;
            if (nLpn >= MAX_LPN)
            {
                break;
            }
        }
        resultFile.close();
    }
    else
    {
        for (nLpn = 0; nLpn < MAX_LPN; nLpn++)
        {
            if (compareData[nLpn] != UNMAPED_PATTERN)
            {
                return false;
            }
        }
    }

    return true;
}

NAND_DATA CompareBufferMgr::Parse(string line)
{
    NAND_DATA LbaInfo;
    UINT32 firstSpacePos = (UINT32)line.find(' ');
    UINT32 secondSpacePos = (UINT32)line.find(' ', firstSpacePos + 1);
    LbaInfo.nLpn = stoi(line.substr(0, firstSpacePos));
    if (secondSpacePos == string::npos)
    {
        LbaInfo.nSize = 1;
        LbaInfo.strPattern = line.substr(firstSpacePos + 1);
    }
    else
    {
        LbaInfo.strPattern = line.substr(firstSpacePos + 1, secondSpacePos - (firstSpacePos + 1));
        LbaInfo.nSize = stoi(line.substr(secondSpacePos + 1));
    }
    return LbaInfo;
}
