// Copyright.2024.binaryfalse81@gmail.com
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Header.h"
#include "../Shell/SsdDriver.h"
#include "../Shell/RealSsdDriver.h"
#include "../Shell/Shell.h"

using namespace testing;

class MockSSDDriver : public SSDDriver
{
public:
    MOCK_METHOD(string, Read, (UINT32 nLpn), (override));
    MOCK_METHOD(VOID, Write, (UINT32 nLpn, string strPattern), (override));
    MOCK_METHOD(VOID, Erase, (UINT32 nLpn, UINT32 nSize), (override));
    MOCK_METHOD(VOID, Flush, (), (override));
    MOCK_METHOD(UINT32, Compare, (), (override));
};

class MockSSDTestShellFixture : public testing::Test
{
protected:
    VOID SetUp() override
    {
        backup_cout = cout.rdbuf(actualOutput.rdbuf());
        shell.SetSsdDriver(&mockSSDDriver);
    }
    VOID TearDown() override
    {
        cout.rdbuf(backup_cout);
    }

public:
    Shell shell;
    NiceMock<MockSSDDriver> mockSSDDriver;
    stringstream actualOutput;
    streambuf* backup_cout;
    UINT32 nLpn = 3;
    const string UNMAPED_PATTERN        = "0x00000000";
    const string WRITE_PATTERN          = "0xAABBCCDD";
    const string INVALID_WRITE_PATTERN  = "0xAAGGCCDD";
    const string INVALID_COMMAND        = "INVALID COMMAND\n";

    string MakeExpectedOutStr(UINT32 nLpn, string strPattern)
    {
        string expectedOutStr;
        if (nLpn == MAX_LPN)
        {
            expectedOutStr = "[FullRead]\n";
            for (UINT32 i = 0; i < MAX_LPN; i++)
            {
                expectedOutStr += ("[Read] nLpn : " + to_string(i));
                expectedOutStr += (", Data : " + strPattern + "\n");
            }
        }
        else
        {
            expectedOutStr = "[Read] nLpn : " + to_string(nLpn);
            expectedOutStr += ", Data : " + strPattern + "\n";
        }

        return expectedOutStr;
    }

    VOID VerifyResult(string expectedOutStr)
    {
        EXPECT_THAT(actualOutput.str(), StrEq(expectedOutStr));
    }
};

TEST_F(MockSSDTestShellFixture, Read_OneLpn)
{
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(1)
        .WillRepeatedly(Return(UNMAPED_PATTERN));

    shell.Run("read " + to_string(nLpn));
    VerifyResult(MakeExpectedOutStr(3, UNMAPED_PATTERN));
}

TEST_F(MockSSDTestShellFixture, Read_FullLpn)
{
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(MAX_LPN)
        .WillRepeatedly(Return(UNMAPED_PATTERN));

    shell.Run("fullread");
    VerifyResult(MakeExpectedOutStr(MAX_LPN, UNMAPED_PATTERN));
}

TEST_F(MockSSDTestShellFixture, WriteAndRead_OneLpn)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(1);
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(1)
        .WillRepeatedly(Return(WRITE_PATTERN));

    shell.Run("write " + to_string(nLpn) + " " + WRITE_PATTERN);
    shell.Run("read " + to_string(nLpn));
    VerifyResult(MakeExpectedOutStr(nLpn, WRITE_PATTERN));
}

TEST_F(MockSSDTestShellFixture, WriteAndRead_FullLpn)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(MAX_LPN);
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(MAX_LPN)
        .WillRepeatedly(Return(WRITE_PATTERN));

    shell.Run("fullwrite " + WRITE_PATTERN);
    shell.Run("fullread");
    VerifyResult(MakeExpectedOutStr(MAX_LPN, WRITE_PATTERN));
}

TEST_F(MockSSDTestShellFixture, ExitTest)
{
    EXPECT_THROW(shell.Run("exit"), ExitTestShell);
}

TEST_F(MockSSDTestShellFixture, HelpTest)
{
    string expectResult = "";
    expectResult += "[Help]\n";
    expectResult += "1. write {nLpn} {Data}\n";
    expectResult += "2. read {nLpn}\n";
    expectResult += "3. exit\n";
    expectResult += "4. help\n";
    expectResult += "5. fullwrite {Data}\n";
    expectResult += "6. fullread\n";
    expectResult += "7. erase {nLpn} {Size}\n";
    expectResult += "8. erase_range {nLpn} {nLpn}\n";
    expectResult += "9. flush\n";
    expectResult += "{nLpn} = {x is an integer | 0 <= x <= 99}\n";
    expectResult += "{Data} = {""0x[0-9A-F]""}\n";
    shell.Run("help");
    VerifyResult(expectResult);
}

TEST_F(MockSSDTestShellFixture, Erase_One)
{
    EXPECT_CALL(mockSSDDriver, Erase)
        .Times(1);
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(1)
        .WillRepeatedly(Return(UNMAPED_PATTERN));

    shell.Run("erase " + to_string(nLpn) + " 1");
    shell.Run("read " + to_string(nLpn));
    VerifyResult(MakeExpectedOutStr(nLpn, UNMAPED_PATTERN));
}

TEST_F(MockSSDTestShellFixture, WriteErase_FullLpn)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(MAX_LPN);
    EXPECT_CALL(mockSSDDriver, Erase)
        .Times(1);
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(MAX_LPN)
        .WillRepeatedly(Return(UNMAPED_PATTERN));

    shell.Run("fullwrite " + WRITE_PATTERN);
    shell.Run("erase 0 " + to_string(MAX_LPN));
    shell.Run("fullread");
    VerifyResult(MakeExpectedOutStr(MAX_LPN, UNMAPED_PATTERN));
}

TEST_F(MockSSDTestShellFixture, Erase_Partial)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(MAX_LPN);
    EXPECT_CALL(mockSSDDriver, Erase)
        .Times(1);
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(MAX_LPN)
        .WillOnce(Return(UNMAPED_PATTERN))
        .WillOnce(Return(UNMAPED_PATTERN))
        .WillOnce(Return(UNMAPED_PATTERN))
        .WillOnce(Return(UNMAPED_PATTERN))
        .WillOnce(Return(UNMAPED_PATTERN))
        .WillRepeatedly(Return(WRITE_PATTERN));

    shell.Run("fullwrite " + WRITE_PATTERN);
    shell.Run("erase 0 " + to_string(5));
    shell.Run("fullread");

    string expectBuffer = "[FullRead]\n";
    for (UINT32 nLpn = 0; nLpn < 5; nLpn++)
    {
        expectBuffer += MakeExpectedOutStr(nLpn, UNMAPED_PATTERN);
    }

    for (UINT32 nLpn = 5; nLpn < MAX_LPN; nLpn++)
    {
        expectBuffer += MakeExpectedOutStr(nLpn, WRITE_PATTERN);
    }

    VerifyResult(expectBuffer);
}

TEST_F(MockSSDTestShellFixture, UnmapCompare)
{
    EXPECT_CALL(mockSSDDriver, Compare)
        .Times(1)
        .WillRepeatedly(Return(1));

    shell.Run("compare");
}

TEST_F(MockSSDTestShellFixture, UnmapCompareFail)
{
    EXPECT_CALL(mockSSDDriver, Compare)
        .Times(1)
        .WillRepeatedly(Return(0));

    EXPECT_THROW(shell.Run("compare"), ExceptionCompareFail);
}

// Arg Cnt Fail
TEST_F(MockSSDTestShellFixture, Write_OneLpn_ArgCntFail)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(0);

    shell.Run("write " + to_string(nLpn) + " " + WRITE_PATTERN + " test");
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Read_OneLpn_ArgCntFail)
{
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(0);

    shell.Run("read " + to_string(nLpn) + " " + WRITE_PATTERN);
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Erase_OneLpn_ArgCntFail)
{
    EXPECT_CALL(mockSSDDriver, Erase)
        .Times(0);

    shell.Run("erase " + to_string(nLpn) + " 1 1");
    VerifyResult(INVALID_COMMAND);
}

// OOR
TEST_F(MockSSDTestShellFixture, Write_OneLpn_OOR)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(0);

    shell.Run("write " + to_string(MAX_LPN) + " " + WRITE_PATTERN);
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Read_OneLpn_OOR)
{
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(0);

    shell.Run("read " + to_string(MAX_LPN));
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Erase_OneLpn_OOR)
{
    EXPECT_CALL(mockSSDDriver, Erase)
        .Times(0);

    shell.Run("erase " + to_string(MAX_LPN) + " 1");
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Erase_Size_OOR)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(MAX_LPN);
    EXPECT_CALL(mockSSDDriver, Erase)
        .Times(1);
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(MAX_LPN)
        .WillRepeatedly(Return(UNMAPED_PATTERN));

    shell.Run("fullwrite " + WRITE_PATTERN);
    shell.Run("erase 0 " + to_string(MAX_LPN + MAX_LPN));
    shell.Run("fullread");
    VerifyResult(MakeExpectedOutStr(MAX_LPN, UNMAPED_PATTERN));
}

TEST_F(MockSSDTestShellFixture, EraseRange_OOR)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(MAX_LPN);
    EXPECT_CALL(mockSSDDriver, Erase)
        .Times(1);
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(MAX_LPN)
        .WillRepeatedly(Return(UNMAPED_PATTERN));

    shell.Run("fullwrite " + WRITE_PATTERN);
    shell.Run("erase_range 0 " + to_string(MAX_LPN + MAX_LPN));
    shell.Run("fullread");
    VerifyResult(MakeExpectedOutStr(MAX_LPN, UNMAPED_PATTERN));
}

// InvalidStringDec Case
TEST_F(MockSSDTestShellFixture, Write_InvalidStringDec)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(0);

    shell.Run("write 1h " + WRITE_PATTERN);
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Read_InvalidStringDec)
{
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(0);

    shell.Run("read 1h");
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Erase_InvalidStringDec)
{
    EXPECT_CALL(mockSSDDriver, Erase)
        .Times(0);

    shell.Run("erase 1h 1");
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Write_OneLpn_InvWriteData)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(0);

    shell.Run("write " + to_string(nLpn) + " " + INVALID_WRITE_PATTERN);
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Write_OneLpn_InvWriteData2)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(0);

    shell.Run("write " + to_string(nLpn) + " test");
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, InvCommand)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(0);

    shell.Run("fullrite " + to_string(nLpn));
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Erase_Invalid)
{
    EXPECT_CALL(mockSSDDriver, Erase)
        .Times(0);

    shell.Run("erase " + to_string(MAX_LPN) + " 1");
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, EraseRange_InvalidInverted)
{
    EXPECT_CALL(mockSSDDriver, Erase)
        .Times(0);

    shell.Run("erase_range" + to_string(nLpn) + " " + to_string(nLpn - 1));
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Flush)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(7);
    EXPECT_CALL(mockSSDDriver, Flush)
        .Times(1);

    for (UINT32 i = 0; i < 7; i++)
    {
        shell.Run("write " + to_string(i) + " " + WRITE_PATTERN);
    }

    shell.Run("flush");
}

#ifdef _DEBUG
// Real Ssd Driver 관련 Test Case
class RealSsdTestShellFixture : public testing::Test
{
public:
    Shell shell;
    RealSSDDriver realSSDDriver;
    const string UNMAPED_PATTERN        = "0x00000000";
    const string WRITE_PATTERN          = "0xAABBCCDD";
    const string INVALID_WRITE_PATTERN  = "0xAAGGCCDD";
    vector<string> strDataList;

    VOID Compare()
    {
        shell.Run("compare");
    }

    VOID FullWrite(string strPattern)
    {
        for (UINT32 nLpn = 0; nLpn < MAX_LPN; nLpn++)
    {
            shell.Run("write " + to_string(nLpn) + " " + strPattern);
            strDataList[nLpn] = strPattern;
        }
    }

    VOID Write(UINT32 nLpn, UINT32 nSize, string strPattern)
    {
        for (UINT32 i = 0; i < nSize; i++)
        {
            shell.Run("write " + to_string(nLpn + i) + " " + strPattern);
            strDataList[nLpn + i] = strPattern;
        }
    }

    VOID Erase(UINT32 nLpn, UINT32 nSize)
    {
        shell.Run("erase " + to_string(nLpn) + " " + to_string(nSize));
        for (UINT32 i = 0; i < nSize; i++)
        {
            if (nLpn + i >= MAX_LPN) break;
            strDataList[nLpn + i] = UNMAPED_PATTERN;
        }
    }

    VOID Flush()
    {
        shell.Run("flush");
    }

    VOID PrintCurrentStep(UINT32 nStep, string strTitle)
    {
        cout << "\r [Step #" << nStep << "] " << strTitle;
    }

    string MakeRandomData(UINT32 nRandValue)
    {
        stringstream stream;
        stream << "0x" << setfill('0') << setw(8) << hex << uppercase << nRandValue;
        return stream.str();
    }

protected:
    VOID SetUp() override
    {
        shell.SetSsdDriver(&realSSDDriver);
        FormatSSD();
        strDataList.clear();
        for (UINT32 nLpn = 0; nLpn < MAX_LPN; nLpn++)
        {
            strDataList.push_back(UNMAPED_PATTERN);
        }
    }
    VOID TearDown() override
    {
        cout << endl;
    }

private:
    bool deleteFileIfExists(string file_path)
    {
        ifstream file(file_path);

        if (file.good())
        {
            file.close();
            if (remove(file_path.c_str()) == 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    VOID FormatSSD(VOID)
    {
        deleteFileIfExists("nand.txt");
        deleteFileIfExists("result.txt");
        deleteFileIfExists("buffer.txt");
    }
};

TEST_F(RealSsdTestShellFixture, FullWriteReadCompare)
{
    shell.Run("fullwrite " + WRITE_PATTERN);
    shell.Run("fullread");
    shell.Run("compare");
}

TEST_F(RealSsdTestShellFixture, FullRead10Compare)
{
    for (UINT32 i = 0; i < 10; i++)
    {
        shell.Run("fullread");
    }
    shell.Run("compare");
}

TEST_F(RealSsdTestShellFixture, Loop_WriteAndReadCompare)
{
    for (UINT32 i = 0; i < 8; i++)
    {
        shell.Run("write 0 " + WRITE_PATTERN);
        shell.Run("read 0");
    }
    shell.Run("compare");
}

TEST_F(RealSsdTestShellFixture, Write10AndCompare)
{
    for (UINT32 i = 0; i < 10; i++)
    {
        shell.Run("write " + to_string(i) + " " + WRITE_PATTERN);
    }
    shell.Run("compare");
}

TEST_F(RealSsdTestShellFixture, LongTermTest)
{
    PrintCurrentStep(0, "FullWrite");
    FullWrite(WRITE_PATTERN);
    Compare();

    srand(0);
    for (UINT32 i = 0; i < 1000; i++)
    {
        try
        {
            PrintCurrentStep(i, "PartialWrite");
            UINT32 nLpn = (rand() % MAX_LPN);
            UINT32 nSize = rand() % 13;
            if (nLpn + nSize >= MAX_LPN) nSize = MAX_LPN - nLpn - 1;
            Write(nLpn, nSize, MakeRandomData(rand()));

            if (i % 5 == 0)
            {
                PrintCurrentStep(i, "PartialErase");
                nLpn = (rand() % MAX_LPN);
                nSize = (rand() % 10) + 1;
                if (nLpn + nSize >= MAX_LPN) nSize = MAX_LPN - nLpn - 1;
                Erase(nLpn, nSize);
            }

            if (i % 23 == 0)
            {
                PrintCurrentStep(i, "Flush");
                Flush();
            }

            if (i % 31 == 0)
            {
                PrintCurrentStep(i, "Compare");
                Compare();
            }
        }
        catch (...)
        {
            while (1);
        }
    }
}
#endif
