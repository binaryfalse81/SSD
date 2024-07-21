#include "Header.h"
#include "VerifyMgr.h"
#include "../Logger/Logger.cpp"

VOID VerifyMgr::SetVerifyPattern(UINT32 nLpn, string strPattern)
{
    if (nLpn < MAX_LPN)
    {
        astrVerifyPattern[nLpn] = strPattern;
    }
}

bool VerifyMgr::Verify()
{
    ifstream NandFileStream("nand.txt");
    string strLine;
    UINT32 nLpn = 0;

    if (NandFileStream.is_open())
    {
        while (getline(NandFileStream, strLine))
        {
            NAND_DATA strNandData = Parsing(strLine);
            if (strNandData.strPattern != astrVerifyPattern[nLpn])
            {
                return false;
            }

            nLpn++;
            if (nLpn >= MAX_LPN)
            {
                break;
            }
        }

        NandFileStream.close();
    }
    else
    {
        for (nLpn = 0; nLpn < MAX_LPN; nLpn++)
        {
            if (astrVerifyPattern[nLpn] != UNMAPED_PATTERN)
            {
                return false;
            }
        }
    }

    return true;
}

NAND_DATA VerifyMgr::Parsing(string strCmd)
{
    NAND_DATA LbaInfo;
    UINT32 nFirstSpaceIdx = (UINT32)strCmd.find(' ');
    UINT32 nSecondSpaceIdx = (UINT32)strCmd.find(' ', nFirstSpaceIdx + 1);
    LbaInfo.nLpn = stoi(strCmd.substr(0, nFirstSpaceIdx));
    if (nSecondSpaceIdx == string::npos)
    {
        LbaInfo.nSize = 1;
        LbaInfo.strPattern = strCmd.substr(nFirstSpaceIdx + 1);
    }
    else
    {
        LbaInfo.strPattern = strCmd.substr(nFirstSpaceIdx + 1, nSecondSpaceIdx - (nFirstSpaceIdx + 1));
        LbaInfo.nSize = stoi(strCmd.substr(nSecondSpaceIdx + 1));
    }

    return LbaInfo;
}
