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
    MOCK_METHOD(string, Read, (INT32 LBA), (override));
    MOCK_METHOD(VOID, Write, (INT32 LBA, string Data), (override));
    MOCK_METHOD(VOID, Erase, (INT32 startLBA, INT32 Size), (override));
    MOCK_METHOD(VOID, Flush, (), (override));
    MOCK_METHOD(UINT32, Compare, (), (override));
    INT32 GetMinLBA() override { return 0; }
    INT32 GetMaxLBA() override { return 99; }
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
    INT32 startOneLBA = 3;
    const INT32 MAX_LBA_CNT = 100;
    const string UNMAPED_DATA       = "0x00000000";
    const string WRITE_DATA         = "0xAABBCCDD";
    const string INV_WRITE_DATA     = "0xAAGGCCDD";
    const string INVALID_COMMAND    = "INVALID COMMAND\n";

    string MakeExpectedOutStr(INT32 LBA, string Data)
    {
        string expectedOutStr;
        if (LBA == MAX_LBA_CNT)
        {
            expectedOutStr = "[FullRead]\n";
            for (INT32 i = 0; i < MAX_LBA_CNT; i++)
            {
                expectedOutStr += ("[Read] LBA : " + to_string(i));
                expectedOutStr += (", Data : " + Data + "\n");
            }
        }
        else
        {
            expectedOutStr = "[Read] LBA : " + to_string(LBA);
            expectedOutStr += ", Data : " + Data + "\n";
        }

        return expectedOutStr;
    }

    VOID VerifyResult(string expectedOutStr)
    {
        EXPECT_THAT(actualOutput.str(), StrEq(expectedOutStr));
    }
};

TEST_F(MockSSDTestShellFixture, Read_OneLBA)
{
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(1)
        .WillRepeatedly(Return(UNMAPED_DATA));

    shell.Run("read " + to_string(startOneLBA));
    VerifyResult(MakeExpectedOutStr(3, UNMAPED_DATA));
}

TEST_F(MockSSDTestShellFixture, Read_FullLBA)
{
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(MAX_LBA_CNT)
        .WillRepeatedly(Return(UNMAPED_DATA));

    shell.Run("fullread");
    VerifyResult(MakeExpectedOutStr(MAX_LBA_CNT, UNMAPED_DATA));
}

TEST_F(MockSSDTestShellFixture, WriteAndRead_OneLBA)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(1);
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(1)
        .WillRepeatedly(Return(WRITE_DATA));

    shell.Run("write " + to_string(startOneLBA) + " " + WRITE_DATA);
    shell.Run("read " + to_string(startOneLBA));
    VerifyResult(MakeExpectedOutStr(startOneLBA, WRITE_DATA));
}

TEST_F(MockSSDTestShellFixture, WriteAndRead_FullLBA)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(MAX_LBA_CNT);
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(MAX_LBA_CNT)
        .WillRepeatedly(Return(WRITE_DATA));

    shell.Run("fullwrite " + WRITE_DATA);
    shell.Run("fullread");
    VerifyResult(MakeExpectedOutStr(MAX_LBA_CNT, WRITE_DATA));
}

TEST_F(MockSSDTestShellFixture, ExitTest)
{
    EXPECT_THROW(shell.Run("exit"), ExitTestShell);
}

TEST_F(MockSSDTestShellFixture, HelpTest)
{
    string expectResult = "";
    expectResult += "[Help]\n";
    expectResult += "1. write {LBA} {Data}\n";
    expectResult += "2. read {LBA}\n";
    expectResult += "3. exit\n";
    expectResult += "4. help\n";
    expectResult += "5. fullwrite {Data}\n";
    expectResult += "6. fullread\n";
    expectResult += "7. erase {LBA} {Size}\n";
    expectResult += "8. erase_range {LBA} {LBA}\n";
    expectResult += "9. flush\n";
    expectResult += "{LBA} = {x is an integer | 0 <= x <= 99}\n";
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
        .WillRepeatedly(Return(UNMAPED_DATA));

    shell.Run("erase " + to_string(startOneLBA) + " 1");
    shell.Run("read " + to_string(startOneLBA));
    VerifyResult(MakeExpectedOutStr(startOneLBA, UNMAPED_DATA));
}

TEST_F(MockSSDTestShellFixture, WriteErase_FullLBA)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(MAX_LBA_CNT);
    EXPECT_CALL(mockSSDDriver, Erase)
        .Times(1);
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(MAX_LBA_CNT)
        .WillRepeatedly(Return(UNMAPED_DATA));

    shell.Run("fullwrite " + WRITE_DATA);
    shell.Run("erase 0 " + to_string(MAX_LBA_CNT));
    shell.Run("fullread");
    VerifyResult(MakeExpectedOutStr(MAX_LBA_CNT, UNMAPED_DATA));
}

TEST_F(MockSSDTestShellFixture, Erase_Partial)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(MAX_LBA_CNT);
    EXPECT_CALL(mockSSDDriver, Erase)
        .Times(1);
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(MAX_LBA_CNT)
        .WillOnce(Return(UNMAPED_DATA))
        .WillOnce(Return(UNMAPED_DATA))
        .WillOnce(Return(UNMAPED_DATA))
        .WillOnce(Return(UNMAPED_DATA))
        .WillOnce(Return(UNMAPED_DATA))
        .WillRepeatedly(Return(WRITE_DATA));

    shell.Run("fullwrite " + WRITE_DATA);
    shell.Run("erase 0 " + to_string(5));
    shell.Run("fullread");

    string expectBuffer = "[FullRead]\n";
    for (INT32 LBA = 0; LBA < 5; LBA++)
    {
        expectBuffer += MakeExpectedOutStr(LBA, UNMAPED_DATA);
    }

    for (INT32 LBA = 5; LBA < MAX_LBA_CNT; LBA++)
    {
        expectBuffer += MakeExpectedOutStr(LBA, WRITE_DATA);
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
TEST_F(MockSSDTestShellFixture, Write_OneLBA_ArgCntFail)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(0);

    shell.Run("write " + to_string(startOneLBA) + " " + WRITE_DATA + " test");
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Read_OneLBA_ArgCntFail)
{
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(0);

    shell.Run("read " + to_string(startOneLBA) + " " + WRITE_DATA);
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Erase_OneLBA_ArgCntFail)
{
    EXPECT_CALL(mockSSDDriver, Erase)
        .Times(0);

    shell.Run("erase " + to_string(startOneLBA) + " 1 1");
    VerifyResult(INVALID_COMMAND);
}

// OOR
TEST_F(MockSSDTestShellFixture, Write_OneLBA_OOR)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(0);

    shell.Run("write " + to_string(MAX_LBA_CNT) + " " + WRITE_DATA);
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Read_OneLBA_OOR)
{
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(0);

    shell.Run("read " + to_string(MAX_LBA_CNT));
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Erase_OneLBA_OOR)
{
    EXPECT_CALL(mockSSDDriver, Erase)
        .Times(0);

    shell.Run("erase " + to_string(MAX_LBA_CNT) + " 1");
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Erase_Size_OOR)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(MAX_LBA_CNT);
    EXPECT_CALL(mockSSDDriver, Erase)
        .Times(1);
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(MAX_LBA_CNT)
        .WillRepeatedly(Return(UNMAPED_DATA));

    shell.Run("fullwrite " + WRITE_DATA);
    shell.Run("erase 0 " + to_string(MAX_LBA_CNT + MAX_LBA_CNT));
    shell.Run("fullread");
    VerifyResult(MakeExpectedOutStr(MAX_LBA_CNT, UNMAPED_DATA));
}

TEST_F(MockSSDTestShellFixture, EraseRange_OOR)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(MAX_LBA_CNT);
    EXPECT_CALL(mockSSDDriver, Erase)
        .Times(1);
    EXPECT_CALL(mockSSDDriver, Read)
        .Times(MAX_LBA_CNT)
        .WillRepeatedly(Return(UNMAPED_DATA));

    shell.Run("fullwrite " + WRITE_DATA);
    shell.Run("erase_range 0 " + to_string(MAX_LBA_CNT + MAX_LBA_CNT));
    shell.Run("fullread");
    VerifyResult(MakeExpectedOutStr(MAX_LBA_CNT, UNMAPED_DATA));
}

// InvalidStringDec Case
TEST_F(MockSSDTestShellFixture, Write_InvalidStringDec)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(0);

    shell.Run("write 1h 0x0000000A");
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

TEST_F(MockSSDTestShellFixture, Write_OneLBA_InvWriteData)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(0);

    shell.Run("write " + to_string(startOneLBA) + " " + INV_WRITE_DATA);
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Write_OneLBA_InvWriteData2)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(0);

    shell.Run("write " + to_string(startOneLBA) + " test");
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, InvCommand)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(0);

    shell.Run("fullrite " + to_string(startOneLBA));
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Erase_Invalid)
{
    EXPECT_CALL(mockSSDDriver, Erase)
        .Times(0);

    shell.Run("erase " + to_string(MAX_LBA_CNT) + " 1");
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, EraseRange_InvalidInverted)
{
    EXPECT_CALL(mockSSDDriver, Erase)
        .Times(0);

    shell.Run("erase_range" + to_string(startOneLBA) + " " + to_string(startOneLBA - 1));
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSSDTestShellFixture, Flush)
{
    EXPECT_CALL(mockSSDDriver, Write)
        .Times(7);
    EXPECT_CALL(mockSSDDriver, Flush)
        .Times(1);

    for (INT32 i = 0; i < 7; i++)
    {
        shell.Run("write " + to_string(i) + " " + WRITE_DATA);
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
    INT32 startOneLBA = 3;
    const INT32 MAX_LBA_CNT = 100;
    const string UNMAPPED_DATA = "0x00000000";
    const string WRITE_DATA = "0xAABBCCDD";
    const string INV_WRITE_DATA = "0xAAGGCCDD";
    const string INVALID_COMMAND = "INVALID COMMAND\n";
    vector<string> strDataList;

    VOID Compare()
    {
        shell.Run("compare");
    }

    VOID FullWrite(string data)
    {
        for (INT32 i = 0; i < MAX_LBA_CNT; i++)
    {
            shell.Run("write " + to_string(i) + " " + data);
            strDataList[i] = data;
        }
    }

    VOID Write(INT32 LBA, INT32 Length, string data)
    {
        for (INT32 i = 0; i < Length; i++)
        {
            shell.Run("write " + to_string(LBA + i) + " " + data);
            strDataList[LBA + i] = data;
        }
    }

    VOID Erase(INT32 LBA, INT32 Length)
    {
        shell.Run("erase " + to_string(LBA) + " " + to_string(Length));
        for (INT32 i = 0; i < Length; i++)
        {
            if (LBA + i >= MAX_LBA_CNT) break;
            strDataList[LBA + i] = UNMAPPED_DATA;
        }
    }
    VOID Flush()
    {
        shell.Run("flush");
    }

    VOID PrintCurrentStep(INT32 step, string stepname)
    {
        cout << "\r [Step #" << step << "] " << stepname;
    }

    string MakeRandomData(INT32 randvalue)
    {
        stringstream stream;
        stream << "0x" << setfill('0') << setw(8) << hex << uppercase << randvalue;
        return stream.str();
    }

protected:
    VOID SetUp() override
    {
        shell.SetSsdDriver(&realSSDDriver);
        FormatSSD();
        strDataList.clear();
        for (INT32 i = 0; i < MAX_LBA_CNT; i++)
        {
            strDataList.push_back("0x00000000");
        }
    }
    VOID TearDown() override
    {
        cout << endl;
    }

private:
    bool deleteFileIfExists(const string& file_path)
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
    shell.Run("fullwrite 0x0000000A");
    shell.Run("fullread");
    shell.Run("compare");
}

TEST_F(RealSsdTestShellFixture, FullRead10Compare)
{
    for (INT32 i = 0; i < 10; i++)
    {
        shell.Run("fullread");
    }
    shell.Run("compare");
}

TEST_F(RealSsdTestShellFixture, Loop_WriteAndReadCompare)
{
    for (INT32 i = 0; i < 8; i++)
    {
        shell.Run("write 0 0x0000000A");
        shell.Run("read 0");
    }
    shell.Run("compare");
}

TEST_F(RealSsdTestShellFixture, Write10AndCompare)
{
    for (INT32 i = 0; i < 10; i++)
    {
        shell.Run("write " + to_string(i) + " 0x0000000A");
    }
    shell.Run("compare");
}

TEST_F(RealSsdTestShellFixture, LongTermTest)
{
    PrintCurrentStep(0, "FullWrite");
    FullWrite("0xABCDABCD");
    Compare();

    srand(0);
    for (INT32 i = 0; i < 1000; i++)
    {
        try
        {
            if (true)
            {
                PrintCurrentStep(i, "PartialWrite");
                INT32 LBA = (rand() % 100);
                INT32 Length = rand() % 13;
                if (LBA + Length >= MAX_LBA_CNT) Length = MAX_LBA_CNT - LBA - 1;
                Write(LBA, Length, MakeRandomData(rand()));
            }

            if (i % 5 == 0)
            {
                PrintCurrentStep(i, "PartialErase");
                INT32 LBA = (rand() % 100);
                INT32 Length = (rand() % 10) + 1;
                if (LBA + Length >= MAX_LBA_CNT) Length = MAX_LBA_CNT - LBA - 1;
                Erase(LBA, Length);
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
