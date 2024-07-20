// Copyright.2024.binaryfalse81@gmail.com
#include "Header.h"
#include "Shell.h"
#include "SSDDriver.h"

VOID Shell::Run(string strCmd)
{
    sc = scf.Make(strCmd);

    try
    {
        sc->Run(sd);
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

    delete(sc);
}

VOID Shell::SetSsdDriver(SSDDriver* sd)
{
    this->sd = sd;
    scf.SetSsdDriver(sd);
}
