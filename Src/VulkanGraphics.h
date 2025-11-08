#include <iostream>

struct _APL_Device
{
	VkPhysicalDevice pDevice;
};

struct _APL_Core3D
{
	VkDevice pDevice;
	VkQueue pCommandQueue;
};

struct _APL_Window
{
	UINT32 Id;
	VkSurfaceKHR Surface;
};

struct _APL_GraphicsContext
{

};

struct _APL_Buffer
{

};

struct _APL_VertexBufferView
{
	
};

struct _APL_ShaderPack
{
	
};

struct _APL_DescriptorHeap
{

};

struct _APL_DescriptorHandle
{

};

namespace APL
{
	class Window;

	struct WindowReference
	{

	};

	struct WindowInfo
	{
		Rect Rect_t;
		Window* ParentWindow;
		RenderProc RenderProc_t;
	};
	
	class VertexBufferView : _APL_VertexBufferView
	{
	public:
	};

	class DescriptorHandle : _APL_DescriptorHandle
	{

	};
	
	class DescriptorHeap : _APL_DescriptorHeap
	{
	public:
	};

	struct ShaderPackDesc
	{
		struct
		{
			LPVOID pShader;
			UINT64 ShaderSize;
		}VS, PS, HS, DS, GS;
	};

	class ShaderPack : _APL_ShaderPack
	{
	public:
	};

	enum Primitive
	{
		Point, Line, Triangle
	};

	class Buffer;

	struct GraphicsContext : _APL_GraphicsContext
	{
	public:
		VOID CopyImageToWindow(Buffer* Image)
		{
			
		}
		VOID ClearWindow(FLOAT Color[4])
		{
			
		}
		VOID ClearWindow(FLOAT Color[4], Rect* pRects, UINT NumRects)
		{
			
		}
		VOID SetViewPort(ViewPort* pRect, UINT TotalViewPorts)
		{
			
		}
		VOID SetScissorRect(Rect* pRect, UINT TotalRects)
		{
			
		}
		VOID SetShaderPack(ShaderPack* pPack)
		{
			
		}
		VOID SetTopology(Primitive Type)
		{
			
		}
		/*VOID SetConstantBuffer(UINT Index, BufferAddress* pBuffer)
		{
			pCommandList->SetGraphicsRootShaderResourceView(Index, *(D3D12_GPU_VIRTUAL_ADDRESS*)pBuffer);
		}*/
		VOID SetDescriptorHeaps(DescriptorHeap* pDescHeap, UINT TotalDescHeaps)
		{
			
		}
		VOID SetDescriptorHandle(UINT RootIndex, DescriptorHandle* pDescHeapHandle)
		{
			
		}
		VOID Draw(VertexBufferView* pDescs, UINT TotalDescs, UINT VertexCount, UINT InstanceCount)
		{

		}
	};
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _APL_XConnectonType
{
	xcb_connection_t* XConnection;
	~_APL_XConnectonType()
	{
		if(XConnection)
			xcb_disconnect(XConnection);
	}
	operator xcb_connection_t*()
	{
		return XConnection;
	}
	void operator = (xcb_connection_t* a)
	{
		XConnection = a;
	}
	xcb_connection_t** operator &()
	{
		return &XConnection;
	}
}_APL_XConnection;

struct _APL_InstanceType
{
	VkInstance Instance;
	~_APL_InstanceType()
	{
		if(Instance)
			vkDestroyInstance(Instance, 0);
	}
	operator VkInstance()
	{
		return Instance;
	}
	VkInstance* operator &()
	{
		return &Instance;
	}
}_APL_Instance;

xcb_screen_t* _APL_Screen;

LPVOID(*_APL_RenderProc)(LPVOID);

LPVOID _APL_WaylandRenderProc(LPVOID pV)
{

	return 0;
}

LPVOID _APL_XcbRenderProc(LPVOID pV)
{
	struct STACK
	{
		APL::WindowInfo* pInfo;
		_APL_Window* pWindow;
		VkDevice pDevice;
		std::binary_semaphore Event;
	}*pStack{(STACK*)pV};

	VkSwapchainKHR pSwapChain;

	VkXcbSurfaceCreateInfoKHR si{};
	VkSwapchainCreateInfoKHR scd{};
	UINT32 QueueIndex{};

	VkQueue pCommandQueue;

	if((pStack->pWindow->Id = xcb_generate_id(_APL_XConnection)) == -1)
	{
		pStack = 0;
		pStack->Event.release();
		return 0;
	}

	if(xcb_request_check(_APL_XConnection, xcb_create_window(_APL_XConnection, XCB_COPY_FROM_PARENT, pStack->pWindow->Id, _APL_Screen->root, pStack->pInfo->Rect_t.Left, pStack->pInfo->Rect_t.Top, pStack->pInfo->Rect_t.Right, pStack->pInfo->Rect_t.Bottom, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, _APL_Screen->root_visual, 0, 0)))
	{
		pStack = 0;
		pStack->Event.release();
		return 0;
	}

	si.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
	si.connection = _APL_XConnection;
	si.window = pStack->pWindow->Id;
	if(vkCreateXcbSurfaceKHR(_APL_Instance, &si, 0, &pStack->pWindow->Surface) != VK_SUCCESS)
		goto Error;

	scd.minImageCount = _APL_TotalBackBuffers;
	scd.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
	scd.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	scd.pQueueFamilyIndices = &QueueIndex;
	scd.queueFamilyIndexCount = 1;
	scd.surface = pStack->pWindow->Surface;
	scd.imageColorSpace = VK_COLOR_SPACE_HDR10_HLG_EXT;
	scd.imageArrayLayers = 1;
	if(vkCreateSwapchainKHR(pStack->pDevice, &scd, 0, &pSwapChain) != VK_SUCCESS)
	{
		std::wcout << L"Failed to create swapchain\n";
		goto Error;
	}

	vkGetDeviceQueue(pStack->pDevice, 0, 1, &pCommandQueue);
	if(!pCommandQueue)
		goto Error;

	xcb_flush(_APL_XConnection);

	//Move VkSurface to this stack?

	pStack->Event.release();
	goto MainLoop;

Error:
	xcb_destroy_window(_APL_XConnection, pStack->pWindow->Id);
	pStack->pWindow->Id = 0;
	pStack->Event.release();
	return 0;
MainLoop:

	goto MainLoop;
	return 0;
}

INT _APL_GraphicsSetup()
{
	LPCHAR XDG_SESSION_TYPE = std::getenv("XDG_SESSION_TYPE");
	
	const char* DumbAssShit[] = {
    "VK_KHR_surface",
    "VK_KHR_xcb_surface"
	};

	VkInstanceCreateInfo InstanceInfo{};

	InstanceInfo.ppEnabledExtensionNames = DumbAssShit;
	InstanceInfo.enabledExtensionCount = 2;

	if(vkCreateInstance(&InstanceInfo, 0, &_APL_Instance) != VK_SUCCESS)
		return 0;

	if(*XDG_SESSION_TYPE == *"x")
	{
		_APL_RenderProc = _APL_XcbRenderProc;

		_APL_XConnection = xcb_connect(0, 0);

		if((xcb_connection_has_error(_APL_XConnection)) > 0)
			return 0;
		
		xcb_screen_iterator_t Iter = xcb_setup_roots_iterator(xcb_get_setup(_APL_XConnection));
		_APL_Screen = Iter.data;

	}
	else if(*XDG_SESSION_TYPE == *"w")
	{
		_APL_RenderProc = _APL_WaylandRenderProc;

	}
	else
		return 0;

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace APL
{
	class Buffer : _APL_Buffer
	{

	};
	class Window : _APL_Window
	{
	public:
		VOID SetTitle(LPCWSTR Title)
		{

		}
		VOID Show()
		{
			xcb_map_window(_APL_XConnection, Id);
			xcb_flush(_APL_XConnection);
		}
		VOID Hide()
		{
			xcb_unmap_window(_APL_XConnection, Id);
			xcb_flush(_APL_XConnection);
		}
	};
	class Core3D : _APL_Core3D
		{
		public:
		~Core3D()
		{
			if(pDevice)
				vkDestroyDevice(pDevice, 0);
			pDevice = 0;
		}
		INT CreateUploadBuffer(UINT64 Size, Buffer* pBuffer)
			{
				_APL_Buffer* Buffer = (_APL_Buffer*)pBuffer;

				

				return 1;
			}
		INT CreateVertexBuffer(UINT Size, UINT Stride, Buffer* pBuffer, VertexBufferView* pView)
		{
			_APL_Buffer* Buffer = (_APL_Buffer*)pBuffer;
			_APL_VertexBufferView* View = (_APL_VertexBufferView*)pView;

			return 1;
		}
		//Must have 256 byte alignment.
		INT CreateConstantBuffer(UINT64 Size, Buffer* pBuffer)
		{
			_APL_Buffer* Buffer = (_APL_Buffer*)pBuffer;

			
			return 1;
		}
		INT CreateTextureBuffer(UINT32 Width, UINT32 Height, INT Format, Buffer* pBuffer)
		{
			_APL_Buffer* Buffer = (_APL_Buffer*)pBuffer;


			return 1;
		}

		//Creates a DescriptorHeap that stores Constant, Texture, and Unordered descriptors.
		INT CreateConstantDescHeap(UINT32 TotalDescs, DescriptorHeap* pDescHeap)
		{
			

			return 1;
		}

		//Creates a DescriptorHeap that stores Sampler descriptors.
		INT CreateSamplerDescHeap(UINT32 TotalDescs, DescriptorHeap* pSamplerHeap)
		{
			

			return 1;
		}

		//Creates a Constant descriptor in a Constant DescriptorHeap.
		VOID CreateConstantDesc(Buffer* pBuffer, UINT32 BufferSize, UINT32 HeapIndex, DescriptorHeap* pDescHeap)
		{
			
		}

		//Creates a Texture descriptor in a Constant DescriptorHeap.
		VOID CreateTextureDesc(Buffer* pBuffer, UINT32 HeapIndex, DescriptorHeap* pDescHeap)
		{
			
		}

		//Creates a Sampler descriptor in a Sampler DescriptorHeap.
		VOID CreateSamplerDesc(UINT32 Index, DescriptorHeap* pDescHeap)
		{
			
			
		}

		//Creates a DescriptorHandle for Constant, Texture, and Unordered descriptors.
		VOID CreateConstantDescriptorHandle(DescriptorHeap* pConstantDescHeap, UINT32 Index, DescriptorHandle* pHandle)
		{
			
		}

		//Creates a DescriptorHandle for Sampler descriptors.
		VOID CreateSamplerDescriptorHandle(DescriptorHeap* pSamplerDescHeap, UINT32 Index, DescriptorHandle* pHandle)
		{
			
		}

		INT CreateShaderPack(ShaderPackDesc* pDesc, ShaderPack* pPack)
		{
			_APL_ShaderPack* Pack = (_APL_ShaderPack*)pPack;

			

			return 1;
		}

		INT CopyDataToUpload(Buffer* pDst, LPVOID pSrc, UINT64 Size)
		{
			_APL_Buffer* Buffer = (_APL_Buffer*)pDst;

			

			return 1;
		}
		
		//Pass in the address of a APL::File only.
		INT CopyFileToUpload(Buffer* pDst, LPVOID pSrc, UINT32 Size)
		{
			_APL_Buffer* Buffer = (_APL_Buffer*)pDst;
			

			return 1;
		}
		
		INT CopyFileToUpload(Buffer* pDst, LPVOID pSrc, UINT32 Size, UINT64 Offset)
		{
			_APL_Buffer* Buffer = (_APL_Buffer*)pDst;
			

			return 1;
		}

		/// <summary>
		/// pDst can be a vertex, constant, or image buffer.
		/// pSrc must be an upload buffer.
		/// </summary>
		VOID CopyUploadToConstant(Buffer* pDstConstant, UINT64 DstOffset, Buffer* pSrcUpload, UINT64 SrcOffset, UINT64 Size)
		{
			_APL_Buffer* Dst = (_APL_Buffer*)pDstConstant;
			_APL_Buffer* Src = (_APL_Buffer*)pSrcUpload;

		}
		
		VOID CopyUploadToTexture(Buffer* pDstTexture, UINT xPos, UINT yPos, UINT zPos, Buffer* pSrcUpload, UINT32 Width, UINT32 Height, INT Format, UINT32 RowPitch)
		{
			_APL_Buffer* Dst = (_APL_Buffer*)pDstTexture;
			_APL_Buffer* Src = (_APL_Buffer*)pSrcUpload;

			
		}
		/*VOID CopyTextureToTexture()
		{

		}*/

		INT ExecuteCopyCommands()
		{

			return 1;
		}
		INT CreateWindow(WindowInfo* pInfo, Window* pWindow)
		{	
			struct 
			{
				WindowInfo* pInfo;
				_APL_Window* pWindow;
				VkDevice pDevice;
				std::binary_semaphore Event;
			}Stack{pInfo, (_APL_Window*)pWindow, this->pDevice, std::binary_semaphore(0)};

			pthread_t Handle;
			if(pthread_create(&Handle, 0, _APL_RenderProc, &Stack))
				return 0;

			Stack.Event.acquire();
			if(!Stack.pWindow->Id)
				return 0;

			return 1;
		}
};
	class Device : _APL_Device
	{
	public:
		~Device()
		{

		}
		VOID GetDeviceInfo(DeviceInfo* pInfo)
		{
			
		}
		INT GetMonitorInfo(UINT Monitor, MonitorInfo* pInfo)
		{

			return 1;
		}
		INT CreateCore3D(Core3D* pCore)
		{
			_APL_Core3D* Core = (_APL_Core3D*)pCore;

			VkQueueFamilyProperties QueueProperties{};
			UINT32 Count = sizeof(QueueProperties) / sizeof(VkQueueFamilyProperties);
			vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &Count, &QueueProperties);

			if(QueueProperties.queueFlags & !VK_QUEUE_GRAPHICS_BIT)
				return 0;

			const CHAR* Extensions[]{"VK_KHR_swapchain"};

			VkDeviceQueueCreateInfo QueueInfo{};
			QueueInfo.queueCount = 2;
			VkDeviceCreateInfo Info{};
			Info.pQueueCreateInfos = &QueueInfo;
			Info.queueCreateInfoCount = 1;
			Info.ppEnabledExtensionNames = Extensions;
			Info.enabledExtensionCount = 1;
			if(vkCreateDevice(pDevice, &Info, 0, &Core->pDevice) != VK_SUCCESS)
				return 0;

			vkGetDeviceQueue(Core->pDevice, 0, 0, &Core->pCommandQueue);
			if(!Core->pCommandQueue)
				return 0;
				
			return 1;
		}
	};

	INT GetDevice(UINT32 Device, APL::Device* pDevice)
	{
		pDevice->~Device();
		Device++;
		_APL_Device* AlsoDevice = (_APL_Device*)pDevice;

		VkResult Ret = vkEnumeratePhysicalDevices(_APL_Instance, &Device, &AlsoDevice->pDevice);
		if(Ret != VK_SUCCESS && Ret != VK_INCOMPLETE)
			return 0;
		
		return 1;
	}
}