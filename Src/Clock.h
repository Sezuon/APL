#define _APL_Clock_Header

#ifndef _APL_Common_Header
#include "Common.h"
#endif

#ifdef __WINDOWS__
//This is global instead of being inside of the G3DClock class because only one copy of this is needed
//and there is no point in calling QueryPerformanceFrequency for every G3DClock object.
LARGE_INTEGER _APL_Clock_Frequency;

namespace APL
{
	class Clock
	{
		LARGE_INTEGER Start{};
		LARGE_INTEGER Stop{};
		LARGE_INTEGER Elapsed{};
	public:
		Clock()
		{

		}
		Clock(BOOL StartNow)
		{
			QueryPerformanceCounter(&Start);
		}
		VOID Reset()
		{
			QueryPerformanceCounter(&Start);
		}
		UINT64 EpochTime()
		{
			QueryPerformanceCounter(&Stop);
			Elapsed.QuadPart = Stop.QuadPart - Start.QuadPart;
			Elapsed.QuadPart *= 1000000;
			Elapsed.QuadPart /= _APL_Clock_Frequency.QuadPart;
			return Elapsed.QuadPart;
		}
		UINT64 LapTime()
		{
			QueryPerformanceCounter(&Stop);
			Elapsed.QuadPart = Stop.QuadPart - Start.QuadPart;
			Elapsed.QuadPart *= 1000000;
			Elapsed.QuadPart /= _APL_Clock_Frequency.QuadPart;
			Start.QuadPart = Stop.QuadPart;
			return Elapsed.QuadPart;
		}
	};
}
#endif

#ifdef __linux__

#endif
