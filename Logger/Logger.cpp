// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include "Header.h"

#define LOG_PRINT(text)  Logger::getInstance().Print(__FUNCTION__, text)

class Logger
{
public:
    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }

    VOID Print(const CHAR* funcName, const CHAR* logMessage)
    {
        const UINT32 KB = 1000;
        const UINT32 MAX_LOG_FILE_SIZE = KB * KB;

        time_t timer = time(NULL);
        tm nowTime;
        localtime_s(&nowTime, &timer);

        const UINT32 BUFFER_SIZE = 200;
        CHAR logCommit[BUFFER_SIZE];
        sprintf_s(logCommit,
                  BUFFER_SIZE,
                  "[%02d.%02d.%02d %02d:%02d:%02d] %-5s : %s\n",
                  nowTime.tm_year - 100,
                  nowTime.tm_mon + 1,
                  nowTime.tm_mday,
                  nowTime.tm_hour,
                  nowTime.tm_min,
                  nowTime.tm_sec,
                  funcName,
                  logMessage);

        try
        {
            if (GetLogFileSize(_LOG_FILE) < MAX_LOG_FILE_SIZE)
            {
                WriteToLogFile(_LOG_FILE, ios::app, logCommit);  // append
                return;
            }

            CompressOldestFile();

            ChangeNameWithTimeStamp(nowTime);

            WriteToLogFile(_LOG_FILE, ios::out, logCommit);  // overwrite
        }
        catch (const exception& e)
        {
            cerr << e.what() << endl;
        }
    }

private:
    const CHAR* _LOG_FILE;
    const CHAR* _OLD_FILE;
    Logger()
    {
        _LOG_FILE = "latest.log";
        ofstream logfile;
        logfile.open(_LOG_FILE, ios::app);
        logfile.close();

        _OLD_FILE = "oldest.txt";
        ofstream flagfile;
        logfile.open(_OLD_FILE, ios::app);
        logfile.close();
    }

    INT32 GetLogFileSize(const CHAR* fileName)
    {
        ifstream readFile(fileName);
        if (!readFile.is_open())
        {
            throw runtime_error("[Error] failed to open readFile");
        }
        readFile.seekg(0, ios::end);
        INT32 readFileSize = (INT32)readFile.tellg();

        return readFileSize;
    }

    VOID WriteToLogFile(const CHAR* fileName,
                        ios_base::openmode mode,
                        const string& logContents)
    {
        ofstream writeFile(fileName, mode);
        if (!writeFile.is_open())
        {
            throw runtime_error("[Error] failed to open writeFile");
        }
        writeFile << logContents;
    }

    VOID CompressOldestFile()
    {
        string oldfile;
        if (IsExistPreviousOldFile(oldfile))
        {
            string newFileName = oldfile.substr(0, oldfile.find(".")) + string(".zip");
            INT32 ret = rename(oldfile.c_str(), newFileName.c_str());
        }
    }

    INT32 IsExistPreviousOldFile(string& oldFileName)
    {
        ifstream readFile(_OLD_FILE);
        if (!readFile.is_open())
        {
            throw runtime_error("[Error] failed to open readFile");
        }

        string stateAndName;
        getline(readFile, stateAndName);
        if (stateAndName.empty()) return 0;

        INT32 isOldFileExist = stoi(stateAndName.substr(0, stateAndName.find(" ")));
        oldFileName = stateAndName.substr(stateAndName.find(" ")+1);

        return isOldFileExist;
    }

    VOID ChangeNameWithTimeStamp(const tm& nowTime)
    {
        const UINT32 BUFFER_SIZE = 50;
        CHAR over10KBLogFile[BUFFER_SIZE];
        sprintf_s(over10KBLogFile,
                  BUFFER_SIZE,
                  "util_%02d%02d%02d_%02dh_%02dm_%02ds.log",
                  nowTime.tm_year - 100,
                  nowTime.tm_mon + 1,
                  nowTime.tm_mday,
                  nowTime.tm_hour,
                  nowTime.tm_min,
                  nowTime.tm_sec);
        INT32 ret = rename(_LOG_FILE, over10KBLogFile);

        WriteToLogFile(_OLD_FILE, ios::out, string("1 ") + string(over10KBLogFile));
    }
};
