#define _APL_Graphics_Header

#ifndef _APL_Common_Header
#include "Common.h"
#endif

#ifdef _WIN32
#include "Win32_Graphics.h"
#else
#include "Linux_Graphics.h"
#endif
