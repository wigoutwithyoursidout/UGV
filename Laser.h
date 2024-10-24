#pragma once

#using <System.dll>
#include "SMObjects.h"

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;


ref class Laser
{
public:

	Laser(SM_ThreadManagement^ SM_TM, SM_Laser^ SM_Laser);
	error_state setupSharedMemory();
	void threadFunction();
	error_state processHeartbeats();
	void shutdownThreads();
	bool getShutdownFlag();
	error_state processSharedMemory();
	error_state communicate();
	error_state checkData();
	~Laser();

private:

	SM_ThreadManagement^ SM_TM_;
	SM_Laser^ SM_Laser_;
	Stopwatch^ Watch;

};