// Copyright.2024.binaryfalse81@gmail.com
#include "Header.h"
#include "Shell.h"
#include "RealSSDDriver.h"
#include "../Logger/Logger.cpp"

const INT32 COMMAND_MODE = 1;
const INT32 SCRIPT_MODE = 2;

VOID CommandMode(VOID);
VOID ScriptMode(CHAR* argv[]);
VOID RunScript(ifstream& runListFile);
VOID FormatSSD(VOID);

INT32 main(INT32 argc, CHAR* argv[])
{
    if (argc == COMMAND_MODE)
    {
        CommandMode();
    }
    else if (argc == SCRIPT_MODE)
    {
        ScriptMode(argv);
    }

    return 0;
}

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

VOID CommandMode(VOID)
{
    FormatSSD();
    LOG_PRINT("Execute the input command being supported");
    Shell TestShellApp;
    TestShellApp.SetSsdDriver(new RealSSDDriver());

    string command;
    while (true)
    {
        cout << "> ";
        command.clear();
        getline(cin, command);
        try
        {
            auto start = chrono::high_resolution_clock::now();

            TestShellApp.Run(command);

            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double, milli> elapsed = end - start;
            cout << "cmd : " << elapsed.count() << " ms\n";
        }
        catch (FailTestShell)
        {
            break;
        }
        catch (ExitTestShell)
        {
            break;
        }
    }
}

VOID ScriptMode(CHAR* argv[])
{
    string inputArg = argv[1];
    string strRunListFile{ inputArg };
    ifstream runListFile(strRunListFile);

    if (runListFile.is_open())
    {
        RunScript(runListFile);
        runListFile.close();
    }
    else
    {
        cerr << "script file open error " << strRunListFile << endl;
    }
}

VOID RunScript(ifstream& runListFile)
{
    string strScriptFile;

    while (getline(runListFile, strScriptFile))
    {
        ifstream scriptFile(strScriptFile);
        Shell TestShellApp;
        TestShellApp.SetSsdDriver(new RealSSDDriver());
        FormatSSD();
        LOG_PRINT("execute the script in the 'run_list'");

        if (scriptFile.is_open())
        {
            stringstream actualOutput;
            streambuf* backup_cout;
            string command;
            bool bIsPass = true;

            auto start = chrono::high_resolution_clock::now();
            cout << strScriptFile << " --- Run ... ";
            backup_cout = cout.rdbuf(actualOutput.rdbuf());

            while (getline(scriptFile, command))
            {
                try
                {
                    TestShellApp.Run(command);
                }
                catch (FailTestShell)
                {
                    bIsPass = false;
                    break;
                }
                catch (ExitTestShell)
                {
                    break;
                }
                catch (ExceptionCompareFail)
                {
                    bIsPass = false;
                    break;
                }
            }

            scriptFile.close();
            cout.rdbuf(backup_cout);

            if (bIsPass)
            {
                cout << "Pass" << endl;
            }
            else
            {
                cout << "FAIL!" << endl;
                return;
            }

            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double, milli> elapsed = end - start;
            cout << "script : " << elapsed.count() << " ms\n";
        }
        else
        {
            cerr << "script file open error " << strScriptFile << endl;
        }
    }
}
