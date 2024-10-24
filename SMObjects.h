#pragma once
#include <cstdint>

using namespace System;
using namespace System::Threading;
using namespace System::Diagnostics;

#define STANDARD_LASER_LENGTH 361
#define CRASH_LIMIT 1000

constexpr uint8_t bit_TM = 0b00000001;
constexpr uint8_t bit_LASER = 0b00000010;
constexpr uint8_t bit_GNSS = 0b00000100;

enum error_state
{

	SUCCESS,
	ERR_SM,
	ERR_CRITICAL_PROCESS_FAILURE, 
	ERR_NONCRITICAL_PROCESS_FAILURE,
	ERR_TMT_FAILURE
};

ref class SM_ThreadManagement
{
public:
	uint8_t shutdown;
	uint8_t heartbeats;
	Barrier^ ThreadBarrier;

};

ref class SM_Laser
{

public:
	array<double>^ x;
	array<double>^ y;

	SM_Laser()
	{

		x = gcnew array<double>(STANDARD_LASER_LENGTH);
	    y = gcnew array<double>(STANDARD_LASER_LENGTH);
		
	}

};

ref class SM_GNSS
{
	public:
		double Easting;
		double Northing;
		double Height;

};
