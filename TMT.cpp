#using <System.dll>

#include "TMT.h"
#include "Laser.h"
#include "GNSS.h"


error_state ThreadManagement::setupSharedMemory() {
    SM_TM_ = gcnew SM_ThreadManagement;
    SM_Laser_ = gcnew SM_Laser;
    SM_GNSS_ = gcnew SM_GNSS;

    return SUCCESS;
}

// thread function
void ThreadManagement::threadFunction() 
{

    Console::WriteLine("TMT Thread is starting");
    // list of thread properties
    ThreadPropertiesList = gcnew array<ThreadProperties^>
    {
        gcnew ThreadProperties(gcnew ThreadStart(gcnew Laser(SM_TM_, SM_Laser_), &Laser::threadFunction), true, bit_LASER, "Laser thread"),
        gcnew ThreadProperties(gcnew ThreadStart(gcnew GNSS(SM_TM_, SM_GNSS_), &GNSS::threadFunction), true, bit_GNSS, "GNSS thread")

    };
    
    // list of threads
    ThreadList = gcnew array<Thread^>(ThreadPropertiesList->Length);
    // make the stopwatch list
    StopwatchList = gcnew array<Stopwatch^>(ThreadPropertiesList->Length);
    // make the thread barrier
    SM_TM_->ThreadBarrier = gcnew Barrier(ThreadPropertiesList->Length + 1);
    
    // start all threads
    for (int i = 0; i < ThreadPropertiesList->Length; i++) {
        
        StopwatchList[i] = gcnew Stopwatch;
        ThreadList[i] = gcnew Thread(ThreadPropertiesList[i]->ThreadStart_);
        ThreadList[i]->Start();
            
    }
    // wait at the tmt thread barrier
    SM_TM_->ThreadBarrier->SignalAndWait();
    // start all the stop watches
    for (int i = 0; i < ThreadList->Length; i++) {
        StopwatchList[i]->Start();
    }
    // start the thread loop
    while (!Console::KeyAvailable && !getShutdownFlag()) 
    {
    
        Console::WriteLine("TMT Thread is running ");
        // keep checking the heart beats
        processHeartbeats();
        Thread::Sleep(50);

        
    }
    
    shutdownThreads();


    //end of thread loop 
    for (int i = 0; i < ThreadPropertiesList->Length; i++) {
        ThreadList[i]->Join();
    }
    Console::WriteLine("TMT Thread terminating... ");

}

// process heartbeats
error_state ThreadManagement::processHeartbeats() 
{
    for (int i = 0; i < ThreadList->Length; i++)
    {
        // check heartbeat flag of thread (is it high?)
     
        if (SM_TM_->heartbeats & ThreadPropertiesList[i]->BitID)

            // if so put flag down and reset stopwatch
        {
            SM_TM_->heartbeats ^= ThreadPropertiesList[i]->BitID;
            StopwatchList[i]->Restart();
        }
        // not - check stopwatch if it has exceeded time limit
        else {

            if (StopwatchList[i]->ElapsedMilliseconds > CRASH_LIMIT)
            {
                //if it has, it is critical?
                if (ThreadPropertiesList[i]->Critical) {
                    //if so then restart everything
                Console::WriteLine(ThreadList[i]->Name + " failed: Critical Thread failure, restarting all.");
                    shutdownThreads();
                    return ERR_CRITICAL_PROCESS_FAILURE;
                }
                else { 

                    //if not restart thread
                    Console::WriteLine(ThreadList[i]->Name + " failed: Non-critical thread failure, restarting thread.");
                    ThreadList[i]->Abort();
                    ThreadList[i] = gcnew Thread(ThreadPropertiesList[i]->ThreadStart_);
                    SM_TM_->ThreadBarrier = gcnew Barrier(1);
                    ThreadList[i]->Start();
                    return ERR_NONCRITICAL_PROCESS_FAILURE;
                }
                

            }
            
                
        
        }
        
    }
    return SUCCESS;
}

// shutdown threads
void ThreadManagement::shutdownThreads() {

    SM_TM_->shutdown = 0xFF; // 0b11111111
}

// get shutdown flag
bool ThreadManagement::getShutdownFlag() {
    return (SM_TM_->shutdown & bit_TM);
}