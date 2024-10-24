#pragma once

#using <System.dll>
#include "Laser.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;

Laser::Laser(SM_ThreadManagement^SM_TM, SM_Laser^SM_Laser)
{
	SM_Laser_ = SM_Laser;
	SM_TM_ = SM_TM;
	Watch = gcnew Stopwatch;

}

error_state Laser::setupSharedMemory() 
{
	return SUCCESS;
}

void Laser::threadFunction()
{
    Console::WriteLine("Laser thread is starting");

    // setup the stopwatch
    Watch = gcnew Stopwatch;
    // wait at the barrier
    SM_TM_->ThreadBarrier->SignalAndWait();
    // Start the stopwatch
    Watch->Start();
    // Start thread loop
    while (!Console::KeyAvailable && !getShutdownFlag())
    {
        Console::WriteLine("Laser thread is running.");
        processHeartbeats();
        if (communicate() == SUCCESS && checkData() == SUCCESS)
        {
            processSharedMemory();
        }
        Thread::Sleep(20);
    }
    Console::WriteLine("Laser thread is terminating");
}
error_state Laser::processHeartbeats()
{
    // Is the laser bit in the heartbeat byte down?
    if ((SM_TM_->heartbeats & bit_LASER) == 0)
    {
        // put the laser bit up
        SM_TM_->heartbeats |= bit_LASER;
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
void Laser::shutdownThreads() 
{
	SM_TM_->shutdown = 0xFF;
}
bool Laser::getShutdownFlag() 
{
	return (SM_TM_->shutdown & bit_LASER);
}
error_state Laser::processSharedMemory() { return SUCCESS; }
error_state Laser::communicate() { return SUCCESS; }
error_state Laser::checkData() { return SUCCESS; }
Laser::~Laser() {}