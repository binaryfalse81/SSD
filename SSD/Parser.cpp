// Copyright.2024.binaryfalse81@gmail.com
#include "Header.h"
#include"Parser.h"
#include "../Logger/Logger.cpp"

class ArgsLengthNotMatchException : public exception {};

CMD_INFO *Parser::Parse(const string &strCmd)
{
    TokenArgument(strCmd);
    return UpdateCmdInfo();
}

VOID Parser::TokenArgument(const string& strCmd)
{
    LOG_PRINT("Separate commands into tokens");
    string token;
    size_t start = strCmd.find(' ', 0) + 1;
    size_t end = start;

    while ((end = strCmd.find(' ', start)) != string::npos)
    {
        token = strCmd.substr(start, end - start);
        astrCmdToken.push_back(token);
        start = end + 1;
    }

    token = strCmd.substr(start);
    astrCmdToken.push_back(token);
}

CMD_INFO* Parser::UpdateCmdInfo()
{
    LOG_PRINT("Generate the appropriate command : W/R/E/F");
    if (astrCmdToken[0] == WRITE_CMD) return UpdateWriteCmdInfo();
    else if (astrCmdToken[0] == READ_CMD) return UpdateReadCmdInfo();
    else if (astrCmdToken[0] == ERASE_CMD) return UpdateEraseCmdInfo();
    else if (astrCmdToken[0] == FLUSH_CMD) return UpdateFlushCmdInfo();
    return nullptr;
}

CMD_INFO *Parser::UpdateWriteCmdInfo()
{
    CheckWriteCmdToken();
    CMD_INFO* pstCmdInfo = new CMD_INFO();
    pstCmdInfo->eCmdType = ENUM_CMD_TYPE::Write;
    pstCmdInfo->nLpn = astrCmdToken[1];
    pstCmdInfo->strPattern = astrCmdToken[2];
    return pstCmdInfo;
}

CMD_INFO* Parser::UpdateReadCmdInfo()
{
    CheckReadCmdToken();
    CMD_INFO* pstCmdInfo = new CMD_INFO();
    pstCmdInfo->eCmdType = ENUM_CMD_TYPE::Read;
    pstCmdInfo->nLpn = astrCmdToken[1];
    return pstCmdInfo;
}

CMD_INFO *Parser::UpdateEraseCmdInfo()
{
    CheckEraseCmdToken();
    CMD_INFO * pstCmdInfo = new CMD_INFO();
    pstCmdInfo->eCmdType = ENUM_CMD_TYPE::Erase;
    pstCmdInfo->nLpn = astrCmdToken[1];
    pstCmdInfo->nEraseSize = stoi(astrCmdToken[2]);
    return pstCmdInfo;
}

CMD_INFO *Parser::UpdateFlushCmdInfo()
{
    CheckFlushCmdToken();
    CMD_INFO * pstCmdInfo = new CMD_INFO();
    pstCmdInfo->eCmdType = ENUM_CMD_TYPE::Flush;
    return pstCmdInfo;
}

VOID Parser::CheckWriteCmdToken()
{
    if (astrCmdToken.size() != 3)
    {
        throw ArgsLengthNotMatchException();
    }
}

VOID Parser::CheckReadCmdToken()
{
    if (astrCmdToken.size() != 2)
    {
        throw ArgsLengthNotMatchException();
    }
}

VOID Parser::CheckEraseCmdToken()
{
    if (astrCmdToken.size() != 3)
    {
        throw ArgsLengthNotMatchException();
    }
}

VOID Parser::CheckFlushCmdToken()
{
    if (astrCmdToken.size() != 1)
    {
        throw ArgsLengthNotMatchException();
    }
}
