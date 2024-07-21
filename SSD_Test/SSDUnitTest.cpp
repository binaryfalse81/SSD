//  Copyright [2024] <CRA/BestReviewer>
#pragma once
#include "Header.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "../SSD/SSD.cpp"
#include "../SSD/SSDCmd.cpp"
#include "../SSD/Parser.cpp"

using namespace testing;

class MockSSD : public Disk
{
public:
    MOCK_METHOD(VOID, Write, (const UINT32& nLpn, const string& strPattern), (override));
    MOCK_METHOD(VOID, Read, (const UINT32& nLpn), (override));
    MOCK_METHOD(VOID, Erase, (const UINT32& nLpn, const UINT32& nSize), (override));
    MOCK_METHOD(VOID, Flush, (), (override));
};

class SSDTestFixture : public testing::Test
{
public:
    NiceMock<MockSSD> mockSSD;
    Parser parser;
    PARSING_RESULT stParsingResult;
    SSDCmd testCmd{ &mockSSD, &parser, &stParsingResult };
    const string UNMAPED_PATTERN = "0x00000000";

    SSD ssd;
    string getLSBData(UINT32 nLpn)
    {
        string strLine;
        ifstream NandFileStream("nand.txt");

        if (NandFileStream.is_open())
        {
            for (UINT32 i = 0; i < nLpn; i++)
            {
                if (!getline(NandFileStream, strLine))
                {
                    break;
                }
            }
            getline(NandFileStream, strLine);
            UINT32 LpnDataFIrstIndex = (UINT32)strLine.find(" ");
            NandFileStream.close();
            return strLine.substr(LpnDataFIrstIndex + 1);
        }
        return UNMAPED_PATTERN;
    }
};

TEST_F(SSDTestFixture, TestLpnRangeExceptionWhenWrite)
{
    EXPECT_THROW(ssd.Write(MAX_LPN, "0x10000000"), LpnRangeException);
}

TEST_F(SSDTestFixture, TestDataRangeException)
{
    EXPECT_THROW(ssd.Write(MAX_LPN - 1, "0x100000"), DataRangeException);
}

TEST_F(SSDTestFixture, TestDataPreFIxException)
{
    EXPECT_THROW(ssd.Write(MAX_LPN - 1, "0110000000"), DataPreFIxException);
}

TEST_F(SSDTestFixture, TestDataTypeException)
{
    EXPECT_THROW(ssd.Write(MAX_LPN - 1, "0x1000000Z"), DataTypeException);
}

TEST_F(SSDTestFixture, TestWriteMemory)
{
    ssd.Write(0, "0x10000001");
    ssd.Flush();
    EXPECT_EQ("0x10000001", getLSBData(0));
}

TEST_F(SSDTestFixture, TestWriteCommandWithMock)
{
    EXPECT_CALL(mockSSD, Write).Times(1);
    testCmd.Run("SSD.exe W 0 0x00000001");
}

TEST_F(SSDTestFixture, TestLpnRangeExceptionWhenRead)
{
    EXPECT_THROW(ssd.Read(-10), LpnRangeException);
    EXPECT_THROW(ssd.Read(MAX_LPN), LpnRangeException);
}

TEST_F(SSDTestFixture, TestReadCommandWithMock)
{
    EXPECT_CALL(mockSSD, Read).Times(1);
    testCmd.Run("SSD.exe R 0");
}

TEST_F(SSDTestFixture, TestLpnRangeExceptionWhenErase)
{
    EXPECT_THROW(ssd.Erase(-10, 10), LpnRangeException);
    EXPECT_THROW(ssd.Erase(MAX_LPN, 10), LpnRangeException);
}

TEST_F(SSDTestFixture, TestEraseSizeRangeException)
{
    EXPECT_THROW(ssd.Erase(1, 11), EraseSizeException);
    EXPECT_THROW(ssd.Erase(1, -1), EraseSizeException);
}

TEST_F(SSDTestFixture, TestEraseMemory)
{
    ssd.Erase(0, 1);
    ssd.Flush();
    EXPECT_EQ(UNMAPED_PATTERN, getLSBData(0));
}

TEST_F(SSDTestFixture, TestEraseMemoryOverMaxLpnException)
{
    EXPECT_THROW(ssd.Erase(MAX_LPN - 2, 10), LpnRangeException);
}

TEST_F(SSDTestFixture, TestEraseMemoryWithMaxLpn)
{
    ssd.Write(MAX_LPN - 1, "0x00000099");
    ssd.Erase(MAX_LPN - 1, 1);
    ssd.Flush();
    EXPECT_EQ(UNMAPED_PATTERN, getLSBData(MAX_LPN - 1));
}

TEST_F(SSDTestFixture, TestEraseMemoryWithMaxRange)
{
    ssd.Write(MAX_LPN - 2, "0x00000098");
    ssd.Erase(90, 10);
    ssd.Flush();
    EXPECT_EQ(UNMAPED_PATTERN, getLSBData(MAX_LPN - 2));
}

TEST_F(SSDTestFixture, TestEraseCommandWithMock)
{
    EXPECT_CALL(mockSSD, Erase).Times(1);
    testCmd.Run("SSD.exe E 0 0x00000001");
}
