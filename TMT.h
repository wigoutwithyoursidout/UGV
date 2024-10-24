#pragma once

#using <System.dll>

#include "SMObjects.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;


ref struct ThreadProperties
{

	ThreadStart^ ThreadStart_;
	bool Critical;
	String^ ThreadName;
	uint8_t BitID;

	ThreadProperties(ThreadStart^ start, bool crit, uint8_t bit_id, String^ threadName)
	{
		ThreadStart_ = start;
		Critical = crit;
		BitID = bit_id;
		ThreadName = threadName;
	}

};


ref class ThreadManagement 
{
public:
	//set up shared memory
	error_state setupSharedMemory();
	//thread function
	void threadFunction();
	//process heartbeats
	error_state processHeartbeats();
	//shutdown threads
	void shutdownThreads();
	//get shutdown flag
	bool getShutdownFlag();

	//while loop conditional on while loop ebing shut off
	 

private:

	SM_ThreadManagement^ SM_TM_;
	SM_Laser^ SM_Laser_;
	SM_GNSS^ SM_GNSS_;
	array<Stopwatch^>^ StopwatchList;
	array<Thread^>^ ThreadList;
	array<ThreadProperties^>^ ThreadPropertiesList;


};