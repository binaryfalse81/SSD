// Copyright.2024.binaryfalse81@gmail.com
#pragma once
#include <iostream>
#include <ctime>
#include <string>
#include <fstream>

using namespace std;

#define LOG_PRINT(text)  Logger::getInstance().print(__FUNCTION__, text)

class Logger
{
public:
    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }

    void print(const char* funcName, const char* logMessage)
    {
        const unsigned int KB = 1000;
        const unsigned int MAX_LOG_FILE_SIZE = KB * KB;

        time_t timer = time(NULL);
        tm nowTime;
        localtime_s(&nowTime, &timer);

        const unsigned int BUFFER_SIZE = 200;
        char logCommit[BUFFER_SIZE];
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
    const char* _LOG_FILE;
    const char* _OLD_FILE;
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

    int GetLogFileSize(const char* fileName)
    {
        ifstream readFile(fileName);
        if (!readFile.is_open())
        {
            throw runtime_error("[Error] failed to open readFile");
        }
        readFile.seekg(0, ios::end);
        int readFileSize = (int)readFile.tellg();

        return readFileSize;
    }

    void WriteToLogFile(const char* fileName,
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

    void CompressOldestFile()
    {
        string oldfile;
        if (IsExistPreviousOldFile(oldfile))
        {
            string newFileName = oldfile.substr(0, oldfile.find(".")) + string(".zip");
            int ret = rename(oldfile.c_str(), newFileName.c_str());
        }
    }

    int IsExistPreviousOldFile(string& oldFileName)
    {
        ifstream readFile(_OLD_FILE);
        if (!readFile.is_open())
        {
            throw runtime_error("[Error] failed to open readFile");
        }

        string stateAndName;
        getline(readFile, stateAndName);
        if (stateAndName.empty()) return 0;

        int isOldFileExist = stoi(stateAndName.substr(0, stateAndName.find(" ")));
        oldFileName = stateAndName.substr(stateAndName.find(" ")+1);

        return isOldFileExist;
    }

    void ChangeNameWithTimeStamp(const tm& nowTime)
    {
        const unsigned int BUFFER_SIZE = 50;
        char over10KBLogFile[BUFFER_SIZE];
        sprintf_s(over10KBLogFile,
                  BUFFER_SIZE,
                  "util_%02d%02d%02d_%02dh_%02dm_%02ds.log",
                  nowTime.tm_year - 100,
                  nowTime.tm_mon + 1,
                  nowTime.tm_mday,
                  nowTime.tm_hour,
                  nowTime.tm_min,
                  nowTime.tm_sec);
        int ret = rename(_LOG_FILE, over10KBLogFile);

        WriteToLogFile(_OLD_FILE, ios::out, string("1 ") + string(over10KBLogFile));
    }
};
