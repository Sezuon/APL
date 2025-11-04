#define _APL_Graphics

#ifndef _APL_Common
#include "Common.h"
#endif

#ifdef _WIN32
#include <dcomp.h>
#include <dxgi1_3.h>
#include <d3d12.h>
#include "D3D12Graphics.h"
#else
#include "VulkanGraphics.h"
#endif
