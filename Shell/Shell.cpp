// Copyright.2024.binaryfalse81@gmail.com
#include "Header.h"
#include "Shell.h"
#include "SSDDriver.h"

VOID Shell::Run(const string& strCommand)
{
    shellCommand = shellCommandFactory.Make(strCommand);

    try
    {
        shellCommand->Run(ssdDriver);
    }
    catch (FailTestShell)
    {
        throw FailTestShell();
    }
    catch (ExitTestShell)
    {
        throw ExitTestShell();
    }
    catch (ExceptionCompareFail)
    {
        throw ExceptionCompareFail();
    }

    delete(shellCommand);
}

VOID Shell::SetSsdDriver(SSDDriver* sd)
{
    this->ssdDriver = sd;
    shellCommandFactory.SetSsdDriver(sd);
}
