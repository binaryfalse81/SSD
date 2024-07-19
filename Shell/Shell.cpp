// Copyright [2024] <CRA/BestReviewer>
#include "Shell.h"
#include "SsdDriver.h"
#include <string>

void Shell::Run(const string& strCommand) {
    shellCommand = shellCommandFactory.Make(strCommand);

    try {
        shellCommand->Run(ssddriver);
    }
    catch (FailTestShell) {
        throw FailTestShell();
    }
    catch (ExitTestShell) {
        throw ExitTestShell();
    }
    catch (ExceptionCompareFail) {
        throw ExceptionCompareFail();
    }

    delete(shellCommand);
}

void Shell::SetSsdDriver(SsdDriver* ssddriver) {
    this->ssddriver = ssddriver;
    shellCommandFactory.SetSsdDriver(ssddriver);
}
