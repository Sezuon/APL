#define _APL_Graphics_Header
#ifndef _APL_Common_Header
#include "Common.h"
#endif

#ifdef _WIN32
#include "D3D12Graphics.h"
#else
#include "VulkanGraphics.h"
#endif
