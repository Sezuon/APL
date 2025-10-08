#include <dcomp.h>
#include <dxgi1_3.h>
#include <d3d12.h>
#define IIDPPV IID_PPV_ARGS
#undef GetMonitorInfo

//A "Smart" COM ptr used by APL.
//Release is only ever called once in the destructor. So if you recreate one of these objects, call the destructor first.
template <typename _APL_InterfaceType>
struct _ComPtr
{
	_APL_InterfaceType* pT{};
	~_ComPtr()
	{
		if (pT)
			pT->Release();
	}
	operator _APL_InterfaceType* ()
	{
		return pT;
	}
	_APL_InterfaceType* operator ->()
	{
		return pT;
	}
	_APL_InterfaceType** operator &()
	{
		return &pT;
	}
};

const UINT _APL_TotalBackBuffers = 4;
const UINT _APL_TotalCommandLists = 2;

struct _APL_Adapter
{
	_ComPtr<IDXGIAdapter> pA;
};
struct _APL_Device
{
	_ComPtr<ID3D12Device> pDevice;
	_ComPtr<IDCompositionDevice> pCompDevice;
	_ComPtr<ID3D12CommandQueue> pCommandQueue;
	_ComPtr<ID3D12Fence> pCommandQueueFence;
	HANDLE CommandQueueFenceEvent{};
	UINT64 CommandQueueFenceValue{};
	_ComPtr<ID3D12CommandAllocator> pAllocator;
	_ComPtr<ID3D12GraphicsCommandList> pCommandList;//This is just a copy CommandList.
	UINT CBVIncrementSize{};
	UINT SamplerIncrementSize{};
};
struct _APL_UploadBuffer
{
	_ComPtr<ID3D12Resource> pResource;
};
struct _APL_VertexBuffer
{
	_ComPtr<ID3D12Resource> pResource;
	D3D12_VERTEX_BUFFER_VIEW vbv{};
	D3D_PRIMITIVE_TOPOLOGY Topology{};
	UINT32 TotalVertices{};
};
struct _APL_ConstantBuffer
{
	_ComPtr<ID3D12Resource> pConstantBuffer;
};
struct _APL_ImageBuffer
{
	_ComPtr<ID3D12Resource> pResource;
	D3D12_GPU_VIRTUAL_ADDRESS Ptr{};
};
struct _APL_DescriptorBuffer
{
	_ComPtr<ID3D12DescriptorHeap> pDescHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE pCpuDescHandle{};
	D3D12_GPU_DESCRIPTOR_HANDLE pGpuDescHandle{};
};
struct _APL_ShaderPack
{
	_ComPtr<ID3D12RootSignature> pRootSignature;
	_ComPtr<ID3D12PipelineState> pPipelineState;
};
struct _APL_GraphicsContext
{
	ID3D12GraphicsCommandList* pCommandList;
	ID3D12Resource* pBB;
	D3D12_CPU_DESCRIPTOR_HANDLE pRTV;
	UINT CBVIncrementSize;
	UINT SamplerIncrementSize;
};
struct _APL_Window
{
	HWND Hwnd;
	HANDLE RendererThreadHandle;
	RECT Rect;
};

namespace APL
{
	typedef RECT Rect;

	enum VertexType
	{
		Vert16,
		Vert24,
		Vert32,
	};
	struct Vertex16
	{
		FLOAT Pos[4];
	};
	struct Vertex24
	{
		FLOAT Pos[4];
		FLOAT UV[2];
	};
	class UploadBuffer : _APL_UploadBuffer
	{

	};
	class VertexBuffer : _APL_VertexBuffer
	{
	public:
		UINT64 Size()
		{
			return vbv.SizeInBytes;
		}
	};
	class ConstantBuffer : _APL_ConstantBuffer
	{

	};
	class ImageBuffer : _APL_ImageBuffer
	{
	};
	//These buffers hold descriptions of either your constant buffers or samplers.
	class DescriptorBuffer : _APL_DescriptorBuffer
	{

	};

	enum TopologyType
	{
		Point = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
		Line = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
		Triangle = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
	};
	enum TopologyLayout
	{
		PointList = D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
		LineList = D3D_PRIMITIVE_TOPOLOGY_LINELIST,
		LineStrip = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
		TriangleList = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		TriangleStrip = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	};
	typedef D3D12_SHADER_BYTECODE ShaderByteCode;
	struct ShaderPackDesc
	{
		ShaderByteCode VS;
		ShaderByteCode PS;
		ShaderByteCode DS;
		ShaderByteCode HS;
		ShaderByteCode GS;
	};
	class ShaderPack : _APL_ShaderPack
	{

	};
	struct ViewPort
	{
		INT32 xPos;
		INT32 yPos;
		INT32 Width;
		INT32 Height;
		INT32 MinDepth;
		INT32 MaxDepth;
	};
	class GraphicsContext : _APL_GraphicsContext
	{
	public:
		VOID ClearFrame(FLOAT Color[4], Rect Rect)
		{
			pCommandList->ClearRenderTargetView(pRTV, Color, 1, &Rect);
		}
		VOID CopyToRenderTarget(ImageBuffer* pSrc, UINT Width, UINT Height)
		{
			//_APL_Resource* dst = (_APL_Resource*)pDst;
			_APL_ImageBuffer* src = (_APL_ImageBuffer*)pSrc;

			D3D12_RESOURCE_BARRIER rb[2]{};
			rb->Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			rb->Transition.pResource = src->pResource;
			rb->Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
			rb->Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
			rb[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			rb[1].Transition.pResource = pBB;
			rb[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			rb[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

			//D3D12_SUBRESOURCE_FOOTPRINT dfp{ DXGI_FORMAT_R16G16B16A16_FLOAT, Width, Height, 1, Width * 8 };
			//D3D12_PLACED_SUBRESOURCE_FOOTPRINT dpfp{ 0, dfp};
			D3D12_TEXTURE_COPY_LOCATION dcl{ pBB, D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, 0 };

			//D3D12_SUBRESOURCE_FOOTPRINT sfp{ DXGI_FORMAT_R8G8B8A8_UNORM, Width, Height, 1, Width * 4 };
			//D3D12_PLACED_SUBRESOURCE_FOOTPRINT spfp{ 0, sfp };
			D3D12_TEXTURE_COPY_LOCATION scl{ src->pResource, D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, 0 };

			pCommandList->ResourceBarrier(2, rb);
			pCommandList->CopyTextureRegion(&dcl, 0, 0, 0, &scl, 0);

			rb->Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
			rb->Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
			rb[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			rb[1].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			pCommandList->ResourceBarrier(2, rb);
		}
		VOID SetViewPort(ViewPort* pViewPort, UINT TotalViewPorts)
		{
			D3D12_VIEWPORT vp;
			memcpy(&vp, pViewPort, sizeof(vp));
			pCommandList->RSSetViewports(TotalViewPorts, &vp);
		}
		VOID SetScissorRect(Rect* pRects, UINT TotalRects)
		{
			pCommandList->RSSetScissorRects(TotalRects, pRects);
		}
		VOID SetShaderPack(ShaderPack* pShaders)
		{
			_APL_ShaderPack* ShaderPack = (_APL_ShaderPack*)pShaders;
			pCommandList->SetGraphicsRootSignature(ShaderPack->pRootSignature);
			pCommandList->SetPipelineState((ID3D12PipelineState*)ShaderPack->pPipelineState);
		}
		//You can set up to two DescriptorBuffer's. One of each type.
		VOID SetDescriptorBuffers(DescriptorBuffer** pDescArrayPtrs, UINT TotalDescs)
		{
			_APL_DescriptorBuffer** pDesc = (_APL_DescriptorBuffer**)pDescArrayPtrs;
			ID3D12DescriptorHeap* p[2]{ pDesc[0]->pDescHeap, pDesc[1]->pDescHeap };
			pCommandList->SetDescriptorHeaps(TotalDescs, p);
		}
		VOID SetConstantDescriptorRoot(DescriptorBuffer* pDescBuffer, UINT DescriptorSlot, UINT RootSlot)
		{
			_APL_DescriptorBuffer* pDesc = (_APL_DescriptorBuffer*)pDescBuffer;
			D3D12_GPU_DESCRIPTOR_HANDLE Temp = pDesc->pGpuDescHandle;
			Temp.ptr += DescriptorSlot * CBVIncrementSize;
			pCommandList->SetGraphicsRootDescriptorTable(RootSlot, Temp);
		}
		VOID SetSamplerDescriptorRoot(DescriptorBuffer* pDescBuffer, UINT DescriptorSlot, UINT RootSlot)
		{
			_APL_DescriptorBuffer* pDesc = (_APL_DescriptorBuffer*)pDescBuffer;
			D3D12_GPU_DESCRIPTOR_HANDLE Temp = pDesc->pGpuDescHandle;
			Temp.ptr += DescriptorSlot * SamplerIncrementSize;
			pCommandList->SetGraphicsRootDescriptorTable(RootSlot, Temp);
		}
		VOID Draw(VertexBuffer* pObject, UINT TotalObjects)
		{
			_APL_VertexBuffer* Object = (_APL_VertexBuffer*)pObject;

			for(UINT16 a{}; a < TotalObjects; a++)
			{
				pCommandList->IASetPrimitiveTopology(Object[a].Topology);
				pCommandList->IASetVertexBuffers(0, 1, &Object[a].vbv);
				pCommandList->DrawInstanced(Object[a].TotalVertices, 1, 0, 0);
			}
		}
	};
	
	//typedef INT(*WindowSetupProc)();
	typedef VOID(*DrawProc)(GraphicsContext);
	
	class Window;
	struct WindowReference
	{
		HWND Hwnd;
		bool operator==(Window w);
	};

	//You MUST declare this variable AFTER all resource variables it uses. See example for more info.
	class Window : _APL_Window
	{
	public:
		~Window()
		{
			PostThreadMessage(GetThreadId(RendererThreadHandle), WM_APP, 0, 0);
			WaitForSingleObject(RendererThreadHandle, INFINITE);
		}
		VOID Show(UINT ShowCmd)
		{
			ShowWindow(Hwnd, ShowCmd);
			SetForegroundWindow(Hwnd);
		}
		VOID Hide(UINT ShowCmd)
		{
			ShowWindow(Hwnd, ShowCmd);
		}
		VOID SetTitle(LPCWSTR Title)
		{
			SetWindowText(Hwnd, Title);
		}
		RECT GetRect()
		{
			return Rect;
		}
		operator WindowReference()
		{
			return WindowReference{this->Hwnd};
		}
		bool operator==(WindowReference wr)
		{
			if(wr.Hwnd == this->Hwnd)
				return true;
			return false;
		}
	};
	bool WindowReference::operator==(Window w)
	{
		_APL_Window& W = (_APL_Window&)w;
		if(W.Hwnd == this->Hwnd)
			return true;
		return false;
	}
	struct WindowDesc
	{
		Rect Rect;
		WindowReference ParentWindow;
		DrawProc pDrawProc;
	};
	
	enum InputType
	{
		Keyboard, Mouse, Close
	};
	enum MouseFlags
	{
		LMBDown = 0x1,
		LMBUp = 0x2,
	};
	struct InputData
	{
		InputType Type;
		union
		{
			struct
			{
				USHORT MakeCode;
				USHORT Flags;
			}Keyboard;
			struct
			{
				USHORT ButtonFlags;
				USHORT ButtonData;
				USHORT LastX;
				USHORT LastY;
			}Mouse;
			WindowReference Window;
		};
	};
}

///////////////////////////////////////////////////////////////////////////

HWND _APL_MsgQueueHwnd;
HANDLE _APL_InputSentEvent;
HANDLE _APL_InputReceivedEvent;

_ComPtr<IDXGIFactory2> _APL_pFactory;

_ComPtr<ID3D12Debug> pDebug;

D3D12_INPUT_ELEMENT_DESC _Vert16_IED[1]{ {"Position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} };
D3D12_INPUT_ELEMENT_DESC _Vert24_IED[2]{ {"Position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
											{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} };
D3D12_INPUT_ELEMENT_DESC _Vert32_IED[2]{ {"Position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
											{"Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} };
D3D12_INPUT_LAYOUT_DESC _APL_LayoutDescs[]{ {_Vert16_IED, 1}, { _Vert24_IED, 2 }, { _Vert32_IED, 2 } };

UINT _APL_VertexStrides[]{ 16, 24, 32 };

APL::InputData _APL_Input;

LRESULT WINAPI _APL_WindowProc(HWND Hwnd, UINT Msg, WPARAM Wp, LPARAM Lp)
{
	switch (Msg)
	{
	case WM_INPUT:
	{
		RAWINPUT RawInput;
		UINT RawInputSize = sizeof(RAWINPUT);
		if (GetRawInputData((HRAWINPUT)Lp, RID_INPUT, &RawInput, &RawInputSize, sizeof(RAWINPUTHEADER)) == -1)
			return DefWindowProc(Hwnd, Msg, Wp, Lp);

		APL::InputData InputData{};
		if (RawInput.header.dwType)
		{
			InputData.Type = APL::InputType::Keyboard;
			memcpy(&InputData.Keyboard, &RawInput.data.keyboard.MakeCode, sizeof(APL::InputData::Keyboard));
		}
		else
		{
			InputData.Type = APL::InputType::Mouse;
			memcpy(&InputData.Mouse.ButtonFlags, &RawInput.data.mouse.ulButtons, 4);
			memcpy(&InputData.Mouse.LastX, &RawInput.data.mouse.lLastX, 4);
		}

		WaitForSingleObject(_APL_InputReceivedEvent, INFINITE);
		memcpy(&_APL_Input, &InputData, sizeof(InputData));
		SetEvent(_APL_InputSentEvent);

		return DefWindowProc(Hwnd, Msg, Wp, Lp);
	}
	case WM_CLOSE:
	{
		APL::InputData Input{};
		Input.Type = APL::InputType::Close;
		Input.Window.Hwnd = Hwnd;

		WaitForSingleObject(_APL_InputReceivedEvent, INFINITE);
		memcpy(&_APL_Input, &Input, sizeof(Input));
		SetEvent(_APL_InputSentEvent);

		return 1;
	}
	case WM_APP:
	{
		APL::WindowDesc* pDesc = (APL::WindowDesc*)Wp;
		return (LRESULT)CreateWindowEx(WS_EX_LAYERED, L"wc", 0, WS_POPUP, pDesc->Rect.left, pDesc->Rect.top, pDesc->Rect.right - pDesc->Rect.left, pDesc->Rect.bottom - pDesc->Rect.top, pDesc->ParentWindow.Hwnd, 0, 0, 0);
	}
	case WM_APP + 1:
	{
		DestroyWindow((HWND)Wp);
		return 0;
	}
	case WM_NCHITTEST:
	{
		return HTCAPTION;
	}
	default:
		return DefWindowProc(Hwnd, Msg, Wp, Lp);
	}
}
DWORD WINAPI _APL_MsgThread(LPVOID pV)
{
	WNDCLASS wc{};
	wc.lpszClassName = L"wc";
	wc.lpfnWndProc = _APL_WindowProc;
	if (RegisterClass(&wc) == 0)
		goto Error;

	if((_APL_MsgQueueHwnd = CreateWindow(L"wc", 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0)) == 0)
		goto Error;

	SetEvent(_APL_InputSentEvent);
	goto MainLoop;

Error:
	//not calling CloseHandle because app is closing.
	_APL_InputReceivedEvent = 0;
	SetEvent(_APL_InputSentEvent);
	return 1;

	MSG Msg;
MainLoop:
	GetMessage(&Msg, 0, 0, 0);
	DispatchMessage(&Msg);
	goto MainLoop;

	return 0;
}
DWORD WINAPI _APL_RendererProc(LPVOID pV)
{
	struct STACK
	{
		_APL_Device* pDevice;
		APL::WindowDesc* pDesc;
		_APL_Window* pWindow;
		HANDLE Event;
	}*Stack = (STACK*)pV;

	ID3D12Device* pDevice = Stack->pDevice->pDevice;
	IDCompositionDevice* pCompDevice = Stack->pDevice->pCompDevice;
	APL::DrawProc DrawProc = Stack->pDesc->pDrawProc;

	_ComPtr<IDCompositionTarget> pTarget;
	_ComPtr<IDCompositionVisual> pVisual;

	_ComPtr<ID3D12CommandQueue> pCommandQueue;
	_ComPtr<ID3D12Fence> pCommandQueueFence;
	HANDLE CommandQueueFenceEvent;
	UINT64 CommandQueueFenceValue{};

	_ComPtr<IDXGISwapChain1> pSwapChain;
	_ComPtr<ID3D12DescriptorHeap> pDescHeap;
	_ComPtr<ID3D12Resource> pBB[_APL_TotalBackBuffers];
	D3D12_CPU_DESCRIPTOR_HANDLE pRTV[_APL_TotalBackBuffers];

	_ComPtr<ID3D12CommandAllocator> pCommandAllocator[_APL_TotalCommandLists];
	_ComPtr<ID3D12GraphicsCommandList> pCommandList[_APL_TotalCommandLists];

	UINT CbvIncrementSize = Stack->pDevice->CBVIncrementSize;
	UINT SamplerIncrementSize = Stack->pDevice->SamplerIncrementSize;

	UINT CurrentBackBuffer{};
	UINT CurrentCommandList{};

	D3D12_COMMAND_QUEUE_DESC cqd{ D3D12_COMMAND_LIST_TYPE_DIRECT };
	DXGI_SWAP_CHAIN_DESC1 scd{};
	scd.BufferCount = _APL_TotalBackBuffers;
	scd.SampleDesc.Count = 1;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scd.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
	scd.Width = Stack->pDesc->Rect.right - Stack->pDesc->Rect.left;
	scd.Height = Stack->pDesc->Rect.bottom - Stack->pDesc->Rect.top;
	scd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
	D3D12_DESCRIPTOR_HEAP_DESC dhd{ D3D12_DESCRIPTOR_HEAP_TYPE_RTV, _APL_TotalBackBuffers };
	D3D12_RENDER_TARGET_VIEW_DESC rtvd{ DXGI_FORMAT_UNKNOWN, D3D12_RTV_DIMENSION_TEXTURE2D };
	D3D12_CPU_DESCRIPTOR_HANDLE pHeap;
	UINT IncrementSize;

	HWND TempHwnd;//This is to get rid of a stupid warning;
	{
		if ((TempHwnd = (HWND)SendMessage(_APL_MsgQueueHwnd, WM_APP, (WPARAM)Stack->pDesc, 0)) == 0)
		{
			Stack->pDevice = 0;
			SetEvent(Stack->Event);
			return 0;
		}

		Stack->pWindow->Hwnd = TempHwnd;

		if (pCompDevice->CreateTargetForHwnd(Stack->pWindow->Hwnd, 1, &pTarget) != S_OK)
			goto Error;

		if (pCompDevice->CreateVisual(&pVisual) != S_OK)
			goto Error;

		if (pDevice->CreateCommandQueue(&cqd, IIDPPV(&pCommandQueue)) != S_OK)
			goto Error;

		if (pDevice->CreateFence(CommandQueueFenceValue++, D3D12_FENCE_FLAG_NONE, IIDPPV(&pCommandQueueFence)) != S_OK)
			goto Error;

		if ((CommandQueueFenceEvent = CreateEvent(0, 0, 1, 0)) == 0)
			goto Error;

		/*if (_pFactory->CreateSwapChainForHwnd(pCommandQueue, pWindow->Hwnd,&scd, 0, 0, &pSwapChain) != S_OK)
			goto Error;*/

		if (_APL_pFactory->CreateSwapChainForComposition(pCommandQueue, &scd, 0, &pSwapChain) != S_OK)
			goto Error;

		if (pDevice->CreateDescriptorHeap(&dhd, IIDPPV(&pDescHeap)) != S_OK)
			goto Error;

		pHeap = pDescHeap->GetCPUDescriptorHandleForHeapStart();
		IncrementSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		for (UINT a{}; a < _APL_TotalBackBuffers; a++)
		{
			if (pSwapChain->GetBuffer(a, IIDPPV(&pBB[a])) != S_OK)
				goto Error;

			pRTV[a].ptr = pHeap.ptr + (IncrementSize * a);
			pDevice->CreateRenderTargetView(pBB[a], &rtvd, pRTV[a]);
		}

		for (UINT a{}; a < _APL_TotalCommandLists; a++)
		{
			if (pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IIDPPV(&pCommandAllocator[a])) != S_OK)
				goto Error;

			if (pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator[a], 0, IIDPPV(&pCommandList[a])) != S_OK)
				goto Error;

			pCommandList[a]->Close();
		}

		if (pVisual->SetContent(pSwapChain) != S_OK)
			goto Error;

		if (pTarget->SetRoot(pVisual) != S_OK)
			goto Error;

		if (pCompDevice->Commit() != S_OK)
			goto Error;

		Stack->pWindow->Rect = Stack->pDesc->Rect;

		MSG NotUsed;
		PeekMessage(&NotUsed, 0, 0, 0, PM_NOREMOVE);

		SetEvent(Stack->Event);
		goto MainSection;
	}

Error:
	Stack->pDevice = 0;
	DestroyWindow(TempHwnd);
	SetEvent(Stack->Event);
	return 0;

	MSG Msg;
MainSection:

	if (PeekMessage(&Msg, 0, 0, 0, PM_REMOVE))
	{
		if (Msg.message == WM_APP)
		{
			WaitForSingleObject(CommandQueueFenceEvent, INFINITE);
			SendMessage(_APL_MsgQueueHwnd, WM_APP + 1, (WPARAM)TempHwnd, 0);
			return 1;
		}
		Msg = MSG{};
	}

	APL::GraphicsContext GC{};
	_APL_GraphicsContext* pGC = (_APL_GraphicsContext*)&GC;
	*pGC = { pCommandList[CurrentCommandList], pBB[CurrentBackBuffer], pRTV[CurrentBackBuffer], CbvIncrementSize, CbvIncrementSize};
	pCommandAllocator[CurrentCommandList]->Reset();
	pCommandList[CurrentCommandList]->Reset(pCommandAllocator[CurrentCommandList], 0);

	D3D12_RESOURCE_BARRIER rb{ D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, pBB[CurrentBackBuffer], 0, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET };
	pCommandList[CurrentCommandList]->ResourceBarrier(1, &rb);

	pCommandList[CurrentCommandList]->OMSetRenderTargets(1, &pRTV[CurrentBackBuffer], 0, 0);
	DrawProc(GC);

	rb.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	rb.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	pCommandList[CurrentCommandList]->ResourceBarrier(1, &rb);

	if(pCommandList[CurrentCommandList]->Close() != S_OK)
		OutputDebugString(L"pCommandList->Close() Failed.\n");

	WaitForSingleObject(CommandQueueFenceEvent, INFINITE);

	pCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList**)&pCommandList[CurrentCommandList]);
	if (pSwapChain->Present(0, 0) != S_OK)
		OutputDebugString(L"Present Failed.\n");

	pCommandQueue->Signal(pCommandQueueFence, CommandQueueFenceValue);
	pCommandQueueFence->SetEventOnCompletion(CommandQueueFenceValue++, CommandQueueFenceEvent);

	CurrentBackBuffer++;
	CurrentCommandList++;

	if (CurrentBackBuffer == _APL_TotalBackBuffers)
		CurrentBackBuffer = 0;

	if (CurrentCommandList == _APL_TotalCommandLists)
		CurrentCommandList = 0;

	goto MainSection;
}
INT _APL_GraphicsSetup()
{
	/*if (!LoadLibrary(L"C:/Program Files/Microsoft PIX/2412.12/WinPixGpuCapturer"))
		return 0;*/

	if ((_APL_InputSentEvent = CreateEvent(0, 0, 0, 0)) == 0 ||
		(_APL_InputReceivedEvent = CreateEvent(0, 0, 1, 0)) == 0)
		return 0;

	if (!CreateThread(0, 0, _APL_MsgThread, 0, 0, 0))
		return 0;

	if (CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IIDPPV(&_APL_pFactory)) != S_OK)
		return 0;

	if (D3D12GetDebugInterface(IIDPPV(&pDebug)) != S_OK)
		return 0;

	pDebug->EnableDebugLayer();

	RAWINPUTDEVICE rid[2]{ {0x1, 0x2, 0/*RIDEV_NOLEGACY*/, 0}, {0x1, 0x6, 0/*RIDEV_NOLEGACY*/, 0} };
	if (!RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE)))
		return 0;

	WaitForSingleObject(_APL_InputSentEvent, INFINITE);
	if (_APL_InputReceivedEvent == 0)
		return 0;

	return 1;
}

///////////////////////////////////////////////////////////////////////////

namespace APL
{
	struct AdapterInfo : DXGI_ADAPTER_DESC
	{

	};
	struct MonitorInfo
	{
		WCHAR Description[32];
		Rect MonitorRect;
		Rect WorkRect;
	};
	class Device : _APL_Device
	{
	public:
		INT NewUploadBuffer(UINT64 Size, UploadBuffer* pUpload)
		{
			_APL_UploadBuffer* Upload = (_APL_UploadBuffer*)pUpload;
			D3D12_HEAP_PROPERTIES hp{ D3D12_HEAP_TYPE_UPLOAD };
			D3D12_RESOURCE_DESC rd{};
			rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			rd.MipLevels = 1;
			rd.DepthOrArraySize = 1;
			rd.SampleDesc.Count = 1;
			rd.Width = Size;
			rd.Height = 1;
			rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			if (pDevice->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_COMMON, 0, IIDPPV(&Upload->pResource)) != S_OK)
				return 0;

			return 1;
		}
		INT NewObjectBuffer(VertexType Vertex, TopologyLayout Layout, UINT64 TotalVertices, VertexBuffer* pObject)
		{
			_APL_VertexBuffer* Object = (_APL_VertexBuffer*)pObject;
			D3D12_HEAP_PROPERTIES hp{ D3D12_HEAP_TYPE_DEFAULT };
			D3D12_RESOURCE_DESC rd{};
			rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			rd.MipLevels = 1;
			rd.DepthOrArraySize = 1;
			rd.SampleDesc.Count = 1;
			rd.Width = _APL_VertexStrides[Vertex] * TotalVertices;
			rd.Height = 1;
			rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			if (pDevice->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_COMMON, 0, IIDPPV(&Object->pResource)) != S_OK)
				return 0;

			Object->vbv.BufferLocation = Object->pResource->GetGPUVirtualAddress();
			Object->vbv.SizeInBytes = rd.Width;
			Object->vbv.StrideInBytes = _APL_VertexStrides[Vertex];
			Object->Topology = (D3D_PRIMITIVE_TOPOLOGY)Layout;
			Object->TotalVertices = TotalVertices;

			return 1;
		}
		INT NewConstantBuffer(UINT64 Size, ConstantBuffer* pConstant)
		{

			return 1;
		}
		INT NewImageBuffer(UINT32 Width, UINT32 Height, ImageBuffer* pImage)
		{
			_APL_ImageBuffer* Image = (_APL_ImageBuffer*)pImage;

			D3D12_HEAP_PROPERTIES hp{ hp.Type = D3D12_HEAP_TYPE_DEFAULT };
			D3D12_RESOURCE_DESC rd{};
			rd.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			rd.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			rd.MipLevels = 1;
			rd.DepthOrArraySize = 1;
			rd.SampleDesc.Count = 1;
			rd.Width = Width;
			rd.Height = Height;
			if (pDevice->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_COMMON, 0, IIDPPV(&Image->pResource)) != S_OK)
				return 0;

			return 1;
		}
		INT NewConstantDescriptorBuffer(UINT TotalElements, DescriptorBuffer* pDesc)
		{
			_APL_DescriptorBuffer* Desc = (_APL_DescriptorBuffer*)pDesc;

			D3D12_DESCRIPTOR_HEAP_DESC dhd{};
			dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			dhd.NumDescriptors = TotalElements;
			dhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			if (pDevice->CreateDescriptorHeap(&dhd, IIDPPV(&Desc->pDescHeap)) != S_OK)
				return 0;

			Desc->pCpuDescHandle = Desc->pDescHeap->GetCPUDescriptorHandleForHeapStart();
			Desc->pGpuDescHandle = Desc->pDescHeap->GetGPUDescriptorHandleForHeapStart();
			return 1;
		}
		INT NewSamplerDescriptorBuffer(UINT TotalElements, DescriptorBuffer* pDesc)
		{
			_APL_DescriptorBuffer* Desc = (_APL_DescriptorBuffer*)pDesc;

			D3D12_DESCRIPTOR_HEAP_DESC dhd{};
			dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			dhd.NumDescriptors = TotalElements;
			dhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			if (pDevice->CreateDescriptorHeap(&dhd, IIDPPV(&Desc->pDescHeap)) != S_OK)
				return 0;

			Desc->pCpuDescHandle = Desc->pDescHeap->GetCPUDescriptorHandleForHeapStart();
			Desc->pGpuDescHandle = Desc->pDescHeap->GetGPUDescriptorHandleForHeapStart();

			return 1;
		}
		INT NewShaderPack(ShaderPackDesc* pDesc, VertexType Vertex, TopologyType Topology, ShaderPack* pPack)
		{
			_APL_ShaderPack* Pack = (_APL_ShaderPack*)pPack;
			
			if (pDevice->CreateRootSignature(0, pDesc->VS.pShaderBytecode, pDesc->VS.BytecodeLength, IIDPPV(&Pack->pRootSignature)) != S_OK)
				return 0;

			D3D12_BLEND_DESC bd{ 0, 0, 0, 0, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_LOGIC_OP_NOOP, D3D12_COLOR_WRITE_ENABLE_ALL };
			D3D12_RASTERIZER_DESC rd{};
			rd.FillMode = D3D12_FILL_MODE_SOLID;
			rd.CullMode = D3D12_CULL_MODE_BACK;
			D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsd{};
			//gpsd.pRootSignature = Pack->pRootSignature;
			gpsd.VS = pDesc->VS;
			gpsd.PS = pDesc->PS;
			gpsd.DS = pDesc->DS;
			gpsd.HS = pDesc->HS;
			gpsd.GS = pDesc->GS;
			gpsd.BlendState = bd;
			gpsd.SampleMask = 0xffffffff;
			gpsd.RasterizerState = rd;
			gpsd.InputLayout = _APL_LayoutDescs[Vertex];
			gpsd.PrimitiveTopologyType = (D3D12_PRIMITIVE_TOPOLOGY_TYPE)Topology;
			gpsd.NumRenderTargets = 1;
			gpsd.RTVFormats[0] = DXGI_FORMAT_R10G10B10A2_UNORM;
			gpsd.SampleDesc.Count = 1;
			if (pDevice->CreateGraphicsPipelineState(&gpsd, IIDPPV(&Pack->pPipelineState)) != S_OK)
				return 0;

			return 1;
		}
		
		//Pay close attention to the function being used. HANDLE is synonomous to LPVOID so be careful passing them in.
		INT CopyPtrToUpload(UploadBuffer* pDst, LPVOID pSrc, UINT64 Size)
		{
			_APL_UploadBuffer* Upload = (_APL_UploadBuffer*)pDst;
			LPVOID pBuffer{};

			if (Upload->pResource->Map(0, 0, &pBuffer) != S_OK)
				return 0;

			memcpy(pBuffer, pSrc, Size);

			Upload->pResource->Unmap(0, 0);

			return 1;
		}
		//Pay close attention to the function being used. HANDLE is synonomous to LPVOID so be careful passing them in.
		INT CopyFileToUpload(UploadBuffer* pDst, HANDLE hFile, UINT32 Size)
		{
			_APL_UploadBuffer* Upload = (_APL_UploadBuffer*)pDst;
			LPVOID pBuffer{};

			if (Upload->pResource->Map(0, 0, &pBuffer) != S_OK)
				return 0;

			if (!ReadFile((HANDLE)hFile, pBuffer, Size, 0, 0))
				return 0;

			Upload->pResource->Unmap(0, 0);

			return 1;
		}
		INT CopyFileToUpload(UploadBuffer* pDst, HANDLE hFile, UINT32 Size, UINT64 Offset)
		{
			_APL_UploadBuffer* Upload = (_APL_UploadBuffer*)pDst;
			LPVOID pBuffer{};

			if (Upload->pResource->Map(0, 0, &pBuffer) != S_OK)
				return 0;
			
			OVERLAPPED ol{};
			PUINT64 pOffset = (PUINT64)&ol.Offset;
			*pOffset = Offset;
			if (!ReadFile((HANDLE)hFile, pBuffer, Size, 0, &ol))
				return 0;

			Upload->pResource->Unmap(0, 0);

			return 1;
		}
		INT CopyToObject(VertexBuffer* pDst, UINT64 DstOffset, UploadBuffer* pSrc, UINT64 SrcOffset, UINT32 TotalVertices)
		{
			_APL_VertexBuffer* dst = (_APL_VertexBuffer*)pDst;
			_APL_UploadBuffer* src = (_APL_UploadBuffer*)pSrc;

			D3D12_RESOURCE_BARRIER rb[2]{};
			rb[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			rb[0].Transition.pResource = dst->pResource;
			rb[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
			rb[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
			rb[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			rb[1].Transition.pResource = src->pResource;
			rb[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
			rb[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
			pCommandList->ResourceBarrier(2, rb);

			pCommandList->CopyBufferRegion(dst->pResource, DstOffset, src->pResource, SrcOffset, TotalVertices * dst->vbv.StrideInBytes);

			rb[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			rb[0].Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
			rb[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
			rb[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
			pCommandList->ResourceBarrier(2, rb);

			return 1;
		}
		INT CopyToConstant(UploadBuffer* pDst, UINT64 DstOffset, UploadBuffer* pSrc, UINT64 SrcOffset, UINT64 Size)
		{
			_APL_UploadBuffer* dst = (_APL_UploadBuffer*)pDst;
			_APL_UploadBuffer* src = (_APL_UploadBuffer*)pSrc;

			pCommandList->CopyBufferRegion(dst->pResource, DstOffset, src->pResource, SrcOffset, Size);

			return 1;
		}
		INT CopyToImage(ImageBuffer* pDst, ImageBuffer* pSrc, UINT Width, UINT Height, UINT xDst, UINT yDst)
		{
			_APL_ImageBuffer* dst = (_APL_ImageBuffer*)pDst;
			_APL_ImageBuffer* src = (_APL_ImageBuffer*)pSrc;

			//D3D12_SUBRESOURCE_FOOTPRINT dfp{ DXGI_FORMAT_R16G16B16A16_FLOAT, Width, Height, 1, Width * 8 };
			//D3D12_PLACED_SUBRESOURCE_FOOTPRINT dpfp{ 0, dfp};
			D3D12_TEXTURE_COPY_LOCATION dcl{ dst->pResource, D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, 0 };

			D3D12_SUBRESOURCE_FOOTPRINT sfp{ DXGI_FORMAT_R16G16B16A16_FLOAT, Width, Height, 1, Width * 8 };
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT spfp{0, sfp};
			D3D12_TEXTURE_COPY_LOCATION scl{src->pResource, D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT, spfp};
			D3D12_RESOURCE_BARRIER rb{};
			rb.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			rb.Transition.pResource = dst->pResource;
			rb.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
			rb.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
			pCommandList->ResourceBarrier(1, &rb);

			pCommandList->CopyTextureRegion(&dcl, xDst, yDst, 0, &scl, 0);

			rb.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			rb.Transition.StateAfter = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
			pCommandList->ResourceBarrier(1, &rb);

			return 1;
		}
		INT CopyToImage(ImageBuffer* pDst, UploadBuffer* pSrc, UINT Width, UINT Height, UINT xDst, UINT yDst)
		{
			_APL_ImageBuffer* dst = (_APL_ImageBuffer*)pDst;
			_APL_UploadBuffer* src = (_APL_UploadBuffer*)pSrc;

			//D3D12_SUBRESOURCE_FOOTPRINT dfp{ DXGI_FORMAT_R16G16B16A16_FLOAT, Width, Height, 1, Width * 8 };
			//D3D12_PLACED_SUBRESOURCE_FOOTPRINT dpfp{ 0, dfp};
			D3D12_TEXTURE_COPY_LOCATION dcl{ dst->pResource, D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, 0 };

			D3D12_SUBRESOURCE_FOOTPRINT sfp{ DXGI_FORMAT_R16G16B16A16_FLOAT, Width, Height, 1, Width * 8 };
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT spfp{ 0, sfp };
			D3D12_TEXTURE_COPY_LOCATION scl{ src->pResource, D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT, spfp };

			D3D12_RESOURCE_BARRIER rb{};
			rb.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			rb.Transition.pResource = dst->pResource;
			rb.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
			rb.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
			pCommandList->ResourceBarrier(1, &rb);

			pCommandList->CopyTextureRegion(&dcl, xDst, yDst, 0, &scl, 0);

			rb.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			rb.Transition.StateAfter = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
			pCommandList->ResourceBarrier(1, &rb);

			return 1;
		}
		INT ExecuteCopyCommands()
		{
			if (pCommandList->Close() != S_OK)
				return 0;

			pCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList**)&pCommandList);
			pCommandQueue->Signal(pCommandQueueFence, CommandQueueFenceValue);
			pCommandQueueFence->SetEventOnCompletion(CommandQueueFenceValue++, CommandQueueFenceEvent);
			WaitForSingleObject(CommandQueueFenceEvent, INFINITE);

			if (pAllocator->Reset() != S_OK)
				return 0;

			if (pCommandList->Reset(pAllocator, 0) != S_OK)
				return 0;

			return 1;
		}

		VOID NewConstantDescriptor(ImageBuffer* pImage, UINT Element, DescriptorBuffer* pDesc)
		{
			_APL_ImageBuffer* Image = (_APL_ImageBuffer*)pImage;
			_APL_DescriptorBuffer* Desc = (_APL_DescriptorBuffer*)pDesc;
			D3D12_SHADER_RESOURCE_VIEW_DESC srvd{};
			srvd.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvd.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			srvd.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvd.Texture2D.MipLevels = 1;

			D3D12_CPU_DESCRIPTOR_HANDLE Temp = Desc->pCpuDescHandle;
			Temp.ptr += Element * CBVIncrementSize;
			pDevice->CreateShaderResourceView(Image->pResource, &srvd, Temp);
		}
		VOID NewSamplerDescriptor(UINT Element, DescriptorBuffer* pDesc)
		{
			_APL_DescriptorBuffer* Desc = (_APL_DescriptorBuffer*)pDesc;
			
			D3D12_SAMPLER_DESC sd{};
			sd.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sd.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sd.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sd.MinLOD = 1024;
			sd.MaxLOD = 1024;
			//sd.Filter = D3D12_FILTER_ANISOTROPIC;
			//sd.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

			D3D12_CPU_DESCRIPTOR_HANDLE Temp = Desc->pCpuDescHandle;
			Temp.ptr += Element * CBVIncrementSize;

			pDevice->CreateSampler(&sd, Temp);
		}

		INT NewWindow(WindowDesc* pDesc, Window* pWindow)
		{
			_APL_Window* w = (_APL_Window*)pWindow;
			HANDLE Event = CreateEvent(0, 0, 0, 0);
			if (!Event)
				return 0;

			struct STACK
			{
				Device* pDevice;
				WindowDesc* pDesc;
				Window* pWindow;
				HANDLE Event;
			}Stack{ this, pDesc, pWindow, Event };

			w->RendererThreadHandle = CreateThread(0, 0, _APL_RendererProc, &Stack, 0, 0);
			if (!w->RendererThreadHandle)
			{
				CloseHandle(Event);
				return 0;
			}

			WaitForSingleObject(Event, INFINITE);
			if (!Stack.pDevice)
			{
				CloseHandle(Event);
				return 0;
			}

			return 1;
		}

		~Device()
		{
			
		}
	};
	class Adapter : _APL_Adapter
	{
	public:
		INT GetAdapterInfo(AdapterInfo* pInfo)
		{
			pA->GetDesc(pInfo);
			return 1;
		}
		INT GetMonitorInfo(UINT Monitor, MonitorInfo* pInfo)
		{
			IDXGIOutput* pO;
			if (pA->EnumOutputs(Monitor, &pO) == S_OK)
			{
				DXGI_OUTPUT_DESC od;
				pO->GetDesc(&od);

				if (od.AttachedToDesktop)
				{
					MONITORINFO mi;
					mi.cbSize = sizeof(MONITORINFO);
					GetMonitorInfoW(od.Monitor, &mi);
					memcpy(pInfo, od.DeviceName, sizeof(od.DeviceName));
					memcpy(&pInfo->MonitorRect, &mi.rcMonitor, sizeof(Rect) * 2);
					pO->Release();
					return 1;
				}

				pO->Release();
			}

			return 0;//Failure
		}
		INT NewDevice(Device* pDevice)
		{
			_APL_Device* pgc = (_APL_Device*)pDevice;
			if (D3D12CreateDevice(pA, D3D_FEATURE_LEVEL_11_1, IIDPPV(&pgc->pDevice)) != S_OK)
				return 0;

			_ComPtr<IDXGIDevice> dxgi;
			if (DCompositionCreateDevice(dxgi, IIDPPV(&pgc->pCompDevice)) != S_OK)
				return 0;

			D3D12_COMMAND_QUEUE_DESC cqd{ D3D12_COMMAND_LIST_TYPE_DIRECT };
			if(pgc->pDevice->CreateCommandQueue(&cqd, IIDPPV(&pgc->pCommandQueue)) != S_OK)
				return 0;

			if (pgc->pDevice->CreateFence(pgc->CommandQueueFenceValue++, D3D12_FENCE_FLAG_NONE, IIDPPV(&pgc->pCommandQueueFence)) != S_OK)
				return 0;

			if ((pgc->CommandQueueFenceEvent = CreateEvent(0, 0, 0, 0)) == 0)
				return 0;

			if (pgc->pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IIDPPV(&pgc->pAllocator)) != S_OK)
				return 0;

			if (pgc->pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pgc->pAllocator, 0, IIDPPV(&pgc->pCommandList)) != S_OK)
				return 0;

			pgc->CBVIncrementSize = pgc->pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			pgc->SamplerIncrementSize = pgc->pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

			return 1;
		}
		~Adapter()
		{

		}
	};
	INT NewAdapter(UINT AdapterIndex, Adapter* pAdapter)
	{
		_APL_Adapter* pA = (_APL_Adapter*)pAdapter;
		pA->pA.~_ComPtr();

		if (_APL_pFactory->EnumAdapters(AdapterIndex, &pA->pA) != S_OK)
			return 0;

		return 1;
	}
	VOID GetInput(InputData* pInput)
	{
		WaitForSingleObject(_APL_InputSentEvent, INFINITE);
		memcpy(pInput, &_APL_Input, sizeof(InputData));
		SetEvent(_APL_InputReceivedEvent);
	}
}