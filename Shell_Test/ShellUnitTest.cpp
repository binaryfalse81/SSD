// Copyright [2024] <CRA/BestReviewer>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include "../Shell/ShellCommandFactory.cpp"
#include "../Shell/RealSSDDriver.cpp"
#include "../Shell/Shell.cpp"
#include "../Shell/ShellCommand.cpp"

using namespace std;
using namespace testing;

class MockSsdDriver : public SsdDriver {
 public:
    MOCK_METHOD(string, Read, (int LBA), (override));
    MOCK_METHOD(void, Write, (int LBA, string Data), (override));
    MOCK_METHOD(void, Erase, (int startLBA, int Size), (override));
    MOCK_METHOD(void, Flush, (), (override));
    MOCK_METHOD(unsigned int, Compare, (), (override));
    int GetMinLBA() override { return 0; }
    int GetMaxLBA() override { return 99; }
};

class MockSsdTestShellFixture : public testing::Test {
 protected:
    void SetUp() override {
        backup_cout = cout.rdbuf(actualOutput.rdbuf());
        testShell.SetSsdDriver(&mockSsdDriver);
    }
    void TearDown() override {
        cout.rdbuf(backup_cout);
    }

 public:
    Shell testShell;
    NiceMock<MockSsdDriver> mockSsdDriver;
    stringstream actualOutput;
    streambuf* backup_cout;
    int startOneLBA = 3;
    const int MAX_LBA_CNT = 100;
    const string UNMAPED_DATA       = "0x00000000";
    const string WRITE_DATA         = "0xAABBCCDD";
    const string INV_WRITE_DATA     = "0xAAGGCCDD";
    const string INVALID_COMMAND    = "INVALID COMMAND\n";

    string MakeExpectedOutStr(int LBA, string Data) {
        string expectedOutStr;
        if (LBA == MAX_LBA_CNT) {
            expectedOutStr = "[FullRead]\n";
            for (int i = 0; i < MAX_LBA_CNT; i++) {
                expectedOutStr += ("[Read] LBA : " + to_string(i));
                expectedOutStr += (", Data : " + Data + "\n");
            }
        } else {
            expectedOutStr = "[Read] LBA : " + to_string(LBA);
            expectedOutStr += ", Data : " + Data + "\n";
        }

        return expectedOutStr;
    }

    void VerifyResult(string expectedOutStr) {
        EXPECT_THAT(actualOutput.str(), StrEq(expectedOutStr));
    }
};

TEST_F(MockSsdTestShellFixture, Read_OneLBA) {
    EXPECT_CALL(mockSsdDriver, Read)
        .Times(1)
        .WillRepeatedly(Return(UNMAPED_DATA));

    testShell.Run("read " + to_string(startOneLBA));
    VerifyResult(MakeExpectedOutStr(3, UNMAPED_DATA));
}

TEST_F(MockSsdTestShellFixture, Read_FullLBA) {
    EXPECT_CALL(mockSsdDriver, Read)
        .Times(MAX_LBA_CNT)
        .WillRepeatedly(Return(UNMAPED_DATA));

    testShell.Run("fullread");
    VerifyResult(MakeExpectedOutStr(MAX_LBA_CNT, UNMAPED_DATA));
}

TEST_F(MockSsdTestShellFixture, WriteAndRead_OneLBA) {
    EXPECT_CALL(mockSsdDriver, Write)
        .Times(1);
    EXPECT_CALL(mockSsdDriver, Read)
        .Times(1)
        .WillRepeatedly(Return(WRITE_DATA));

    testShell.Run("write " + to_string(startOneLBA) + " " + WRITE_DATA);
    testShell.Run("read " + to_string(startOneLBA));
    VerifyResult(MakeExpectedOutStr(startOneLBA, WRITE_DATA));
}

TEST_F(MockSsdTestShellFixture, WriteAndRead_FullLBA) {
    EXPECT_CALL(mockSsdDriver, Write)
        .Times(MAX_LBA_CNT);
    EXPECT_CALL(mockSsdDriver, Read)
        .Times(MAX_LBA_CNT)
        .WillRepeatedly(Return(WRITE_DATA));

    testShell.Run("fullwrite " + WRITE_DATA);
    testShell.Run("fullread");
    VerifyResult(MakeExpectedOutStr(MAX_LBA_CNT, WRITE_DATA));
}

TEST_F(MockSsdTestShellFixture, ExitTest) {
    EXPECT_THROW(testShell.Run("exit"), ExitTestShell);
}

TEST_F(MockSsdTestShellFixture, HelpTest) {
    string expectResult = "";
    expectResult += "[Help]\n";
    expectResult += "1. write {LBA} {Data}\n";
    expectResult += "2. read {LBA}\n";
    expectResult += "3. exit\n";
    expectResult += "4. help\n";
    expectResult += "5. fullwrite {Data}\n";
    expectResult += "6. fullread\n";
    expectResult += "7. testapp1\n";
    expectResult += "8. testapp2\n";
    expectResult += "9. erase {LBA} {Size}\n";
    expectResult += "10. erase_range {LBA} {LBA}\n";
    expectResult += "11. flush\n";
    expectResult += "{LBA} = {x is an integer | 0 <= x <= 99}\n";
    expectResult += "{Data} = {""0x[0-9A-F]""}\n";
    testShell.Run("help");
    VerifyResult(expectResult);
}

TEST_F(MockSsdTestShellFixture, Erase_One) {
    EXPECT_CALL(mockSsdDriver, Erase)
        .Times(1);
    EXPECT_CALL(mockSsdDriver, Read)
        .Times(1)
        .WillRepeatedly(Return(UNMAPED_DATA));

    testShell.Run("erase " + to_string(startOneLBA) + " 1");
    testShell.Run("read " + to_string(startOneLBA));
    VerifyResult(MakeExpectedOutStr(startOneLBA, UNMAPED_DATA));
}

TEST_F(MockSsdTestShellFixture, WriteErase_FullLBA) {
    EXPECT_CALL(mockSsdDriver, Write)
        .Times(MAX_LBA_CNT);
    EXPECT_CALL(mockSsdDriver, Erase)
        .Times(1);
    EXPECT_CALL(mockSsdDriver, Read)
        .Times(MAX_LBA_CNT)
        .WillRepeatedly(Return(UNMAPED_DATA));

    testShell.Run("fullwrite " + WRITE_DATA);
    testShell.Run("erase 0 " + to_string(MAX_LBA_CNT));
    testShell.Run("fullread");
    VerifyResult(MakeExpectedOutStr(MAX_LBA_CNT, UNMAPED_DATA));
}

TEST_F(MockSsdTestShellFixture, Erase_Partial) {
    EXPECT_CALL(mockSsdDriver, Write)
        .Times(MAX_LBA_CNT);
    EXPECT_CALL(mockSsdDriver, Erase)
        .Times(1);
    EXPECT_CALL(mockSsdDriver, Read)
        .Times(MAX_LBA_CNT)
        .WillOnce(Return(UNMAPED_DATA))
        .WillOnce(Return(UNMAPED_DATA))
        .WillOnce(Return(UNMAPED_DATA))
        .WillOnce(Return(UNMAPED_DATA))
        .WillOnce(Return(UNMAPED_DATA))
        .WillRepeatedly(Return(WRITE_DATA));

    testShell.Run("fullwrite " + WRITE_DATA);
    testShell.Run("erase 0 " + to_string(5));
    testShell.Run("fullread");

    string expectBuffer = "[FullRead]\n";
    for (int LBA = 0; LBA < 5; LBA++) {
        expectBuffer += MakeExpectedOutStr(LBA, UNMAPED_DATA);
    }
    for (int LBA = 5; LBA < MAX_LBA_CNT; LBA++) {
        expectBuffer += MakeExpectedOutStr(LBA, WRITE_DATA);
    }
    VerifyResult(expectBuffer);
}

TEST_F(MockSsdTestShellFixture, TestApp1) {
    EXPECT_CALL(mockSsdDriver, Write)
        .Times(MAX_LBA_CNT);

    EXPECT_CALL(mockSsdDriver, Read)
        .Times(MAX_LBA_CNT);

    testShell.Run("testapp1");
}

TEST_F(MockSsdTestShellFixture, TestApp2) {
    EXPECT_CALL(mockSsdDriver, Write)
        .Times(36);

    EXPECT_CALL(mockSsdDriver, Read)
        .Times(6);

    testShell.Run("testapp2");
}

TEST_F(MockSsdTestShellFixture, UnmapCompare) {
    EXPECT_CALL(mockSsdDriver, Compare)
        .Times(1)
        .WillRepeatedly(Return(1));

    testShell.Run("compare");
}

TEST_F(MockSsdTestShellFixture, UnmapCompareFail) {
    EXPECT_CALL(mockSsdDriver, Compare)
        .Times(1)
        .WillRepeatedly(Return(0));

    EXPECT_THROW(testShell.Run("compare"), ExceptionCompareFail);
}

// Arg Cnt Fail
TEST_F(MockSsdTestShellFixture, Write_OneLBA_ArgCntFail) {
    EXPECT_CALL(mockSsdDriver, Write)
        .Times(0);

    testShell.Run("write " + to_string(startOneLBA) + " " + WRITE_DATA + " test");
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSsdTestShellFixture, Read_OneLBA_ArgCntFail) {
    EXPECT_CALL(mockSsdDriver, Read)
        .Times(0);

    testShell.Run("read " + to_string(startOneLBA) + " " + WRITE_DATA);
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSsdTestShellFixture, Erase_OneLBA_ArgCntFail) {
    EXPECT_CALL(mockSsdDriver, Erase)
        .Times(0);

    testShell.Run("erase " + to_string(startOneLBA) + " 1 1");
    VerifyResult(INVALID_COMMAND);
}

// OOR
TEST_F(MockSsdTestShellFixture, Write_OneLBA_OOR) {
    EXPECT_CALL(mockSsdDriver, Write)
        .Times(0);

    testShell.Run("write " + to_string(MAX_LBA_CNT) + " " + WRITE_DATA);
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSsdTestShellFixture, Read_OneLBA_OOR) {
    EXPECT_CALL(mockSsdDriver, Read)
        .Times(0);

    testShell.Run("read " + to_string(MAX_LBA_CNT));
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSsdTestShellFixture, Erase_OneLBA_OOR) {
    EXPECT_CALL(mockSsdDriver, Erase)
        .Times(0);

    testShell.Run("erase " + to_string(MAX_LBA_CNT) + " 1");
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSsdTestShellFixture, Erase_Size_OOR) {
    EXPECT_CALL(mockSsdDriver, Write)
        .Times(MAX_LBA_CNT);
    EXPECT_CALL(mockSsdDriver, Erase)
        .Times(1);
    EXPECT_CALL(mockSsdDriver, Read)
        .Times(MAX_LBA_CNT)
        .WillRepeatedly(Return(UNMAPED_DATA));

    testShell.Run("fullwrite " + WRITE_DATA);
    testShell.Run("erase 0 " + to_string(MAX_LBA_CNT + MAX_LBA_CNT));
    testShell.Run("fullread");
    VerifyResult(MakeExpectedOutStr(MAX_LBA_CNT, UNMAPED_DATA));
}

TEST_F(MockSsdTestShellFixture, EraseRange_OOR) {
    EXPECT_CALL(mockSsdDriver, Write)
        .Times(MAX_LBA_CNT);
    EXPECT_CALL(mockSsdDriver, Erase)
        .Times(1);
    EXPECT_CALL(mockSsdDriver, Read)
        .Times(MAX_LBA_CNT)
        .WillRepeatedly(Return(UNMAPED_DATA));

    testShell.Run("fullwrite " + WRITE_DATA);
    testShell.Run("erase_range 0 " + to_string(MAX_LBA_CNT + MAX_LBA_CNT));
    testShell.Run("fullread");
    VerifyResult(MakeExpectedOutStr(MAX_LBA_CNT, UNMAPED_DATA));
}

// InvalidStringDec Case
TEST_F(MockSsdTestShellFixture, Write_InvalidStringDec) {
    EXPECT_CALL(mockSsdDriver, Write)
        .Times(0);

    testShell.Run("write 1h 0x0000000A");
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSsdTestShellFixture, Read_InvalidStringDec) {
    EXPECT_CALL(mockSsdDriver, Read)
        .Times(0);

    testShell.Run("read 1h");
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSsdTestShellFixture, Erase_InvalidStringDec) {
    EXPECT_CALL(mockSsdDriver, Erase)
        .Times(0);

    testShell.Run("erase 1h 1");
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSsdTestShellFixture, Write_OneLBA_InvWriteData) {
    EXPECT_CALL(mockSsdDriver, Write)
        .Times(0);

    testShell.Run("write " + to_string(startOneLBA) + " " + INV_WRITE_DATA);
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSsdTestShellFixture, Write_OneLBA_InvWriteData2) {
    EXPECT_CALL(mockSsdDriver, Write)
        .Times(0);

    testShell.Run("write " + to_string(startOneLBA) + " test");
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSsdTestShellFixture, InvCommand) {
    EXPECT_CALL(mockSsdDriver, Write)
        .Times(0);

    testShell.Run("fullrite " + to_string(startOneLBA));
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSsdTestShellFixture, Erase_Invalid) {
    EXPECT_CALL(mockSsdDriver, Erase)
        .Times(0);

    testShell.Run("erase " + to_string(MAX_LBA_CNT) + " 1");
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSsdTestShellFixture, EraseRange_InvalidInverted) {
    EXPECT_CALL(mockSsdDriver, Erase)
        .Times(0);

    testShell.Run("erase_range" + to_string(startOneLBA) + " " + to_string(startOneLBA - 1));
    VerifyResult(INVALID_COMMAND);
}

TEST_F(MockSsdTestShellFixture, Flush) {
    EXPECT_CALL(mockSsdDriver, Write)
        .Times(7);
    EXPECT_CALL(mockSsdDriver, Flush)
        .Times(1);

    for (int i = 0; i < 7; i++) {
        testShell.Run("write " + to_string(i) + " " + WRITE_DATA);
    }
    testShell.Run("flush");
}

#ifdef _DEBUG
// Real Ssd Driver 관련 Test Case
class RealSsdTestShellFixture : public testing::Test {
 public:
    Shell testShell;
    RealSsdDriver realSsdDriver;
    stringstream actualOutput;
    streambuf* backup_cout;
    int startOneLBA = 3;
    const int MAX_LBA_CNT = 100;
    const string UNMAPPED_DATA = "0x00000000";
    const string WRITE_DATA = "0xAABBCCDD";
    const string INV_WRITE_DATA = "0xAAGGCCDD";
    const string INVALID_COMMAND = "INVALID COMMAND\n";
    vector<string> strDataList;
    string MakeFullReadData() {
        string expectedOutStr;
        expectedOutStr = "[FullRead]\n";
        for (int i = 0; i < MAX_LBA_CNT; i++) {
            expectedOutStr += ("[Read] LBA : " + to_string(i));
            expectedOutStr += (", Data : " + strDataList[i] + "\n");
        }

        return expectedOutStr;
    }

    void VerifyFullReadData() {
        testShell.Run("fullread");
        EXPECT_THAT(actualOutput.str(), StrEq(MakeFullReadData()));
        actualOutput.str("");
    }

    void FullWrite(string data) {
        for (int i = 0; i < MAX_LBA_CNT; i++) {
            testShell.Run("write " + to_string(i) + " " + data);
            strDataList[i] = data;
        }
    }

    void Write(int LBA, int Length, string data) {
        for (int i = 0; i < Length; i++) {
            testShell.Run("write " + to_string(LBA + i) + " " + data);
            strDataList[LBA + i] = data;
        }
    }

    void Erase(int LBA, int Length) {
        testShell.Run("erase " + to_string(LBA) + " " + to_string(Length));
        for (int i = 0; i < Length; i++) {
            if (LBA + i >= MAX_LBA_CNT) break;
            strDataList[LBA + i] = UNMAPPED_DATA;
        }
    }
    void Flush() {
        testShell.Run("flush");
    }

    void PrintCurrentStep(int step, string stepname) {
        cout.rdbuf(backup_cout);
        cout << "\r [Step #" << step << "] " << stepname;
        backup_cout = cout.rdbuf(actualOutput.rdbuf());
    }

    string MakeRandomData(int randvalue) {
        stringstream stream;
        stream << "0x" << setfill('0') << setw(8) << hex << uppercase << randvalue;
        return stream.str();
    }

 protected:
    void SetUp() override {
        backup_cout = cout.rdbuf(actualOutput.rdbuf());
        testShell.SetSsdDriver(&realSsdDriver);
        FormatSSD();
        strDataList.clear();
        for (int i = 0; i < MAX_LBA_CNT; i++) {
            strDataList.push_back("0x00000000");
        }
    }
    void TearDown() override {
        cout.rdbuf(backup_cout);
        cout << endl;
    }

 private:
    bool deleteFileIfExists(const string& file_path) {
        ifstream file(file_path);

        if (file.good()) {
            file.close();
            if (remove(file_path.c_str()) == 0) {
                return true;
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }
    }

    void FormatSSD(void)
    {
        deleteFileIfExists("nand.txt");
        deleteFileIfExists("result.txt");
        deleteFileIfExists("buffer.txt");
    }
};

TEST_F(RealSsdTestShellFixture, FullWriteReadCompare) {
    testShell.Run("fullwrite 0x0000000A");
    testShell.Run("fullread");
    testShell.Run("compare");
}

TEST_F(RealSsdTestShellFixture, FullRead10Compare) {
    for (int i = 0; i < 10; i++)
    {
        testShell.Run("fullread");
    }
    testShell.Run("compare");
}

TEST_F(RealSsdTestShellFixture, Loop_WriteAndReadCompare) {
    for (int i = 0; i < 8; i++)
    {
        testShell.Run("write 0 0x0000000A");
        testShell.Run("read 0");
    }
    testShell.Run("compare");
}

TEST_F(RealSsdTestShellFixture, Write10AndCompare) {
    for (int i = 0; i < 10; i++)
    {
        testShell.Run("write " + to_string(i) + " 0x0000000A");
    }
    testShell.Run("compare");
}

TEST_F(RealSsdTestShellFixture, LongTermTest) {
    PrintCurrentStep(0, "FullWrite");
    FullWrite("0xABCDABCD");
    VerifyFullReadData();

    srand(0);
    for (int i = 0; i < 1000; i++) {
        if (true) {
            PrintCurrentStep(i, "PartialWrite");
            int LBA = (rand() % 100);
            int Length = rand() % 13;
            if (LBA + Length >= MAX_LBA_CNT) Length = MAX_LBA_CNT - LBA - 1;
            Write(LBA, Length, MakeRandomData(rand()));
        }
        if (i % 5 == 0) {
            PrintCurrentStep(i, "PartialErase");
            Erase((rand() % 100), (rand() % 10) + 1);
        }
        if (i % 23 == 0) {
            PrintCurrentStep(i, "Flush ");
            Flush();
        }

        // Verify가 너무 오래 걸려 Interval을 늘림. TC Fail 시 Interval 조정하여 Verify.
        if (i % 31 == 0) {
            VerifyFullReadData();
        }
    }
}

#endif
