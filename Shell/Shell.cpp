// Copyright.2024.binaryfalse81@gmail.com
#include <string>
#include "Shell.h"
#include "SSDDriver.h"

void Shell::Run(const string& strCommand)
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

void Shell::SetSsdDriver(SSDDriver* sd)
{
    this->ssdDriver = sd;
    shellCommandFactory.SetSsdDriver(sd);
}
