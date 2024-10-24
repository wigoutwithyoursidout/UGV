#pragma once

#using <System.dll>
#include "GNSS.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;

GNSS::GNSS(SM_ThreadManagement^ SM_TM, SM_GNSS^ SM_GNSS)
{
	SM_GNSS_ = SM_GNSS;
	SM_TM_ = SM_TM;
	Watch = gcnew Stopwatch;

}

error_state GNSS::setupSharedMemory()
{
	return SUCCESS;
}
void GNSS::threadFunction()
{
    Console::WriteLine("GNSS thread is starting");

    // setup the stopwatch
    Watch = gcnew Stopwatch;
    // wait at the barrier
    SM_TM_->ThreadBarrier->SignalAndWait();
    // Start the stopwatch
    Watch->Start();
    // Start thread loop
    while (!Console::KeyAvailable && !getShutdownFlag())
    {
        Console::WriteLine("GNSS thread is running.");
        processHeartbeats();
        if (communicate() == SUCCESS && checkData() == SUCCESS)
        {
            processSharedMemory();
        }
        Thread::Sleep(20);
    }
    Console::WriteLine("GNSS thread is terminating");
}
error_state GNSS::processHeartbeats()
{
    // Is the gnss bit in the heartbeat byte down?
    if ((SM_TM_->heartbeats & bit_GNSS) == 0)
    {
        // put the GNSS bit up
        SM_TM_->heartbeats |= bit_GNSS;
        // reset stopwatch
        Watch->Restart();
    }
    // else
    else
    {
        // has the time elapsed exceeded the crash time limit?
        if (Watch->ElapsedMilliseconds > CRASH_LIMIT)
        {
            // shutdown all threads
            shutdownThreads();
            return ERR_TMT_FAILURE;
        }
    }

    return SUCCESS;
}
void GNSS::shutdownThreads()
{
	SM_TM_->shutdown = 0xFF;
}
bool GNSS::getShutdownFlag()
{
	return (SM_TM_->shutdown & bit_GNSS);
}
error_state GNSS::processSharedMemory() { return SUCCESS; }
error_state GNSS::communicate() { return SUCCESS; }
error_state GNSS::checkData() { return SUCCESS; }
GNSS::~GNSS() {}