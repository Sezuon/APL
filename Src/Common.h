#define _APL_Common

#ifdef _WIN32
#include <Windows.h>
#undef CreateWindow
#undef GetMonitorInfo
#undef VOID
typedef void VOID;
#else

#endif