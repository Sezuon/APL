#define _APL_Graphics

#ifndef _APL_Common
#include "Common.h"
#endif

#ifdef _WIN32
#include <dcomp.h>
#include <dxgi1_3.h>
#include <d3d12.h>
#else
#include <cstdlib>
#include <semaphore>
#include <xcb/xcb.h>
#define VK_USE_PLATFORM_XCB_KHR
#define VK_USE_PLATFORM_WAYLAND_KHR
#include <wayland-client.h>
#include <vulkan/vulkan.h>
#endif

const UINT _APL_TotalBackBuffers = 4;
const UINT _APL_TotalCommandLists = 2;

namespace APL
{
    struct Rect
    {
        INT16 Left, Top, Right, Bottom;
    };

	struct DeviceInfo
	{

	};
	struct MonitorInfo
	{
		Rect MonitorRect, WorkRect;
	};

	struct ViewPort
	{
		INT16 Left, Top, Right, Bottom, MinDepth, MaxDepth;
		ViewPort(Rect Rect_t, INT MinDepth, INT MaxDepth)
		{
			*(Rect*)this = Rect_t;
			this->MinDepth = MinDepth;
			this->MaxDepth = MaxDepth;
		}
	};

	class GraphicsContext;
	typedef VOID (*RenderProc)(GraphicsContext);

    struct Vertex24
	{
		FLOAT Pos[4];
		FLOAT UV[2];
	};
	struct Vertex32
	{
		FLOAT Pos[4];
		FLOAT Color[4];
	};
}

#ifdef _WIN32
#else
#include "VulkanGraphics.h"
#endif
