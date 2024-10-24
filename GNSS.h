#pragma once

#using <System.dll>
#include "SMObjects.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;


ref class GNSS
{
public:
	GNSS(SM_ThreadManagement^ SM_TM, SM_GNSS^ SM_GNSS);
	error_state setupSharedMemory();
	void threadFunction();
	error_state processHeartbeats();
	void shutdownThreads();
	bool getShutdownFlag();
	error_state processSharedMemory();
	error_state communicate();
	error_state checkData();
	~GNSS();

private:

	SM_ThreadManagement^ SM_TM_;
	SM_GNSS^ SM_GNSS_;
	Stopwatch^ Watch;

};