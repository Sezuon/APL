template <typename T>
struct _APL_COM
{
	T* pT{};
	~_APL_COM()
	{
		if (pT)
			pT->Release();
		pT = 0;
	}
	operator T* ()
	{
		return pT;
	}
	T** operator &()
	{
		return &pT;
	}
	T* operator ->()
	{
		return pT;
	}
};

struct _APL_Device
{
	_APL_COM<IDXGIAdapter> pAdapter;
};

struct _APL_Core3D
{
	_APL_COM<ID3D12Device> pDevice;
	_APL_COM<ID3D12CommandQueue> pCommandQueue;
	_APL_COM<ID3D12Fence> pCommandQueueFence;
	UINT64 CommandQueueFenceValue{};
	HANDLE CommandQueueFenceEvent;

	_APL_COM<ID3D12CommandAllocator> pCommandAllocator;
	_APL_COM<ID3D12GraphicsCommandList> pCommandList;
	UINT64 CbvIncrementSize;
	UINT64 SamplerIncrementSize;
};

struct _APL_Window
{
	HWND Hwnd;
	HANDLE RenderThread;
};

struct _APL_GraphicsContext
{
	ID3D12Resource* pBB;
	D3D12_CPU_DESCRIPTOR_HANDLE pRTV;
	ID3D12GraphicsCommandList* pCommandList;
};

struct _APL_Buffer
{
	_APL_COM<ID3D12Resource> pBuffer;
};

struct _APL_BufferAddress
{
	D3D12_GPU_VIRTUAL_ADDRESS pBuffer;
};

struct _APL_VertexBufferView
{
	D3D12_VERTEX_BUFFER_VIEW vbv;
};

struct _APL_ShaderPack
{
	_APL_COM<ID3D12PipelineState> pPipelineState;
	_APL_COM<ID3D12RootSignature> pRootSignature;
};

struct _APL_DescriptorHeap
{
	_APL_COM<ID3D12DescriptorHeap> pDescHeap;
};

struct _APL_DescriptorHandle
{
	D3D12_GPU_DESCRIPTOR_HANDLE Handle;
};

//struct _APL_Descriptor
//{
//	D3D12_GPU_DESCRIPTOR_HANDLE pCBV;
//};

namespace APL
{
	
	class Window;

	struct WindowReference
	{
		HWND Hwnd;
		bool operator = (Window a);
		operator HWND()
		{
			return Hwnd;
		}
	};

	struct WindowInfo
	{
		Rect Rect;
		Window* ParentWindow;
		RenderProc RenderProc;
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

	enum InputLayout;

	struct ShaderPackDesc
	{
		D3D12_SHADER_BYTECODE VS;
		D3D12_SHADER_BYTECODE PS;
		D3D12_SHADER_BYTECODE HS;
		D3D12_SHADER_BYTECODE DS;
		D3D12_SHADER_BYTECODE GS;
	};
	class ShaderPack : _APL_ShaderPack
	{
	public:
	};

	struct ViewPort : D3D12_VIEWPORT
	{
		ViewPort(Rect Rect, UINT MinDepth, UINT MaxDepth)
		{
			TopLeftX = Rect.top;
			TopLeftY = Rect.left;
			Width = Rect.right;
			Height = Rect.bottom;
			MinDepth = MinDepth;
			MaxDepth = MaxDepth;
		}
		VOID Init(Rect Rect, UINT MinDepth, UINT MaxDepth)
		{
			TopLeftX = Rect.top;
			TopLeftY = Rect.left;
			Width = Rect.right;
			Height = Rect.bottom;
			MinDepth = MinDepth;
			MaxDepth = MaxDepth;
		}
	};


	class GraphicsContext : _APL_GraphicsContext
	{
	public:
		//Cant use a texture buffer!
		VOID CopyImageToWindow(Buffer* Image)
		{
			_APL_Buffer* Buffer = (_APL_Buffer*)Image;

			D3D12_RESOURCE_BARRIER rb{};
			rb.Transition.pResource = pBB;
			rb.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			rb.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
			pCommandList->ResourceBarrier(1, &rb);

			D3D12_TEXTURE_COPY_LOCATION Dst{};
			Dst.pResource = pBB;
			Dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

			D3D12_TEXTURE_COPY_LOCATION Src{};
			Src.pResource = Buffer->pBuffer;
			Src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			Src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			Src.PlacedFootprint.Footprint.Width = 1920;
			Src.PlacedFootprint.Footprint.Height = 1080;
			Src.PlacedFootprint.Footprint.RowPitch = 8 * 1920;
			Src.PlacedFootprint.Footprint.Depth = 1;
			pCommandList->CopyTextureRegion(&Dst, 0, 0, 0, &Src, 0);

			rb.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			rb.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			pCommandList->ResourceBarrier(1, &rb);
		}
		VOID ClearWindow(FLOAT Color[4])
		{
			pCommandList->ClearRenderTargetView(pRTV, Color, 0, 0);
		}
		VOID ClearWindow(FLOAT Color[4], Rect* pRects, UINT NumRects)
		{
			pCommandList->ClearRenderTargetView(pRTV, Color, NumRects, pRects);
		}
		VOID SetViewPort(ViewPort* pRect, UINT TotalViewPorts)
		{
			pCommandList->RSSetViewports(TotalViewPorts, pRect);
		}
		VOID SetScissorRect(Rect* pRect, UINT TotalRects)
		{
			pCommandList->RSSetScissorRects(TotalRects, pRect);
		}
		VOID SetShaderPack(ShaderPack* pPack)
		{
			_APL_ShaderPack* Pack = (_APL_ShaderPack*)pPack;
			pCommandList->SetPipelineState(Pack->pPipelineState);
			pCommandList->SetGraphicsRootSignature(Pack->pRootSignature);
		}
		VOID SetTopology(D3D12_PRIMITIVE_TOPOLOGY Type)
		{
			pCommandList->IASetPrimitiveTopology(Type);
		}
		/*VOID SetConstantBuffer(UINT Index, BufferAddress* pBuffer)
		{
			pCommandList->SetGraphicsRootShaderResourceView(Index, *(D3D12_GPU_VIRTUAL_ADDRESS*)pBuffer);
		}*/
		VOID SetDescriptorHeaps(DescriptorHeap* pDescHeap, UINT TotalDescHeaps)
		{
			pCommandList->SetDescriptorHeaps(TotalDescHeaps, (ID3D12DescriptorHeap**)pDescHeap);
		}
		VOID SetDescriptorHandle(UINT RootIndex, DescriptorHandle* pDescHeapHandle)
		{
			pCommandList->SetGraphicsRootDescriptorTable(RootIndex, *(D3D12_GPU_DESCRIPTOR_HANDLE*)pDescHeapHandle);
		}
		VOID Draw(VertexBufferView* pDescs, UINT TotalDescs, UINT VertexCount, UINT InstanceCount)
		{
			D3D12_VERTEX_BUFFER_VIEW* pvbv = (D3D12_VERTEX_BUFFER_VIEW*)pDescs;
			pCommandList->IASetVertexBuffers(0, TotalDescs, pvbv);
			pCommandList->DrawInstanced(VertexCount, InstanceCount, 0, 0);
		}
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_APL_COM<IDXGIFactory3> _APL_pFactory;
_APL_COM<ID3D12Debug> _APL_pDebug;

HWND _APL_WindowProcHwnd;

LRESULT WINAPI _APL_MsgProc(HWND Hwnd, UINT Msg, WPARAM Wp, LPARAM Lp)
{
	switch (Msg)
	{
	case WM_CLOSE:
	{
		return 1;
	}
	case WM_INPUT:
	{

	}
	case WM_APP + 0x8000:
	{
		APL::WindowInfo* Desc = (APL::WindowInfo*)Wp;
		return (LRESULT)CreateWindowEx(WS_EX_LAYERED, L"wc", 0, WS_POPUP, Desc->Rect.left, Desc->Rect.top, Desc->Rect.right - Desc->Rect.left, Desc->Rect.bottom - Desc->Rect.top, *(HWND*)Desc->ParentWindow, 0, 0, 0);
	}
	default:
		return DefWindowProc(Hwnd, Msg, Wp, Lp);
	}
}
DWORD WINAPI _APL_WindowProc(LPVOID pV)
{
	WNDCLASS wc{};
	wc.lpszClassName = L"wc";
	wc.lpfnWndProc = _APL_MsgProc;
	RegisterClass(&wc);

	_APL_WindowProcHwnd = CreateWindowW(L"wc", 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);

	SetEvent(pV);
	if (!_APL_WindowProcHwnd)
		return 0;

	MSG Msg{};
MainLoop:
	GetMessage(&Msg, 0, 0, 0);
	DispatchMessage(&Msg);

	goto MainLoop;
}

DWORD WINAPI _APL_RenderProc(LPVOID pV)
{
	struct STACK
	{
		HANDLE Event;
		APL::WindowInfo* pInfo;
		_APL_Window* pWindow;
		ID3D12Device* pDevice;
	}*pStack{(STACK*)pV};//Can't use after SetEvent()!!!

	_APL_COM<IDCompositionDevice> pCompDevice;
	_APL_COM<IDCompositionTarget> pTarget;
	_APL_COM<IDCompositionVisual> pVisual;
	 
	ID3D12Device*& pDevice = pStack->pDevice;//Can't use after SetEvent()!!!

	_APL_COM<ID3D12CommandQueue> pCommandQueue;
	_APL_COM<ID3D12Fence> pCommandQueueFence;
	UINT64 CommandQueueFenceValue{};
	HANDLE CommandQueueFenceEvent = pStack->Event;//Hopefully works out;

	_APL_COM<IDXGISwapChain3> pSwapChain;
	_APL_COM<ID3D12DescriptorHeap> pRtvDescHeap;
	_APL_COM<ID3D12Resource> pBB[TotalBackBuffers];
	D3D12_CPU_DESCRIPTOR_HANDLE pRTV[TotalBackBuffers];

	_APL_COM<ID3D12CommandAllocator> pCommandAllocator[TotalCommandLists];
	_APL_COM<ID3D12GraphicsCommandList> pCommandList[TotalCommandLists];

	UINT64 IncrementSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	UINT CurrentBackBuffer{};
	UINT CurrentCommandList{};

	APL::RenderProc RenderProc = pStack->pInfo->RenderProc;

	/////////////////////////////

	{
		D3D12_COMMAND_QUEUE_DESC cqd{ D3D12_COMMAND_LIST_TYPE_DIRECT };

		DXGI_SWAP_CHAIN_DESC1 scd{};
		scd.BufferCount = TotalBackBuffers;
		scd.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		scd.Width = 1920;
		scd.Height = 1080;
		scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		scd.SampleDesc.Count = 1;
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		scd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

		D3D12_DESCRIPTOR_HEAP_DESC dhd{ D3D12_DESCRIPTOR_HEAP_TYPE_RTV, TotalBackBuffers };
		D3D12_RENDER_TARGET_VIEW_DESC rtvd{ DXGI_FORMAT_UNKNOWN, D3D12_RTV_DIMENSION_TEXTURE2D };

		MSG Msg{};
		PeekMessage(&Msg, 0, 0, 0, 0);
		PostThreadMessage(GetCurrentThreadId(), WM_APP + 0x8000, 0, 0);
		PeekMessage(&Msg, 0, 0, 0, PM_REMOVE);
		if (Msg.message != WM_APP + 0x8000)
		{
			SetEvent(pStack->Event);
			return 1;
		}

		if (!(pStack->pWindow->Hwnd = (HWND)SendMessage(_APL_WindowProcHwnd, WM_APP + 0x8000, (WPARAM)pStack->pInfo, (LPARAM)pStack->pWindow)))
		{
			SetEvent(pStack->Event);
			return 1;
		}

		if (DCompositionCreateDevice(0, IID_PPV_ARGS(&pCompDevice)) != S_OK)
			goto Error;

		if (pCompDevice->CreateTargetForHwnd(pStack->pWindow->Hwnd, 0, &pTarget) != S_OK)
			goto Error;

		if (pCompDevice->CreateVisual(&pVisual) != S_OK)
			goto Error;

		if (pDevice->CreateCommandQueue(&cqd, IID_PPV_ARGS(&pCommandQueue)) != S_OK)
			goto Error;

		if (pDevice->CreateFence(CommandQueueFenceValue++, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pCommandQueueFence)) != S_OK)
			goto Error;

		if (_APL_pFactory->CreateSwapChainForComposition(pCommandQueue, &scd, 0, (IDXGISwapChain1**)&pSwapChain) != S_OK)
			goto Error;

		if (pSwapChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709) != S_OK)
			goto Error;

		if (pDevice->CreateDescriptorHeap(&dhd, IID_PPV_ARGS(&pRtvDescHeap)) != S_OK)
			goto Error;

		for (UINT a{}; a < TotalBackBuffers; a++)
		{
			if (pSwapChain->GetBuffer(a, IID_PPV_ARGS(&pBB[a])) != S_OK)
				goto Error;

			pRTV[0] = pRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
			pRTV[a].ptr = pRTV[0].ptr + IncrementSize * a;

			pDevice->CreateRenderTargetView(pBB[a], &rtvd, pRTV[a]);
		}

		for (UINT a{}; a < TotalCommandLists; a++)
		{
			if (pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandAllocator[a])) != S_OK)
				goto Error;

			if (pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator[a], 0, IID_PPV_ARGS(&pCommandList[a])) != S_OK)
				goto Error;

			pCommandList[a]->Close();
		}

		if (pVisual->SetContent(pSwapChain) != S_OK)
			goto Error;

		if (pTarget->SetRoot(pVisual) != S_OK)
			goto Error;

		if (pCompDevice->Commit() != S_OK)
			goto Error;

		SetEvent(pStack->Event);
		SetEvent(pStack->Event);

		goto MainLoop;
	}

Error:
	DestroyWindow(pStack->pWindow->Hwnd);
	pStack->pWindow->Hwnd = 0;
	SetEvent(pStack->Event);
	return 1;

	MSG Msg;
MainLoop:
	Msg = MSG{};
	PeekMessage(&Msg, 0, 0, 0, PM_REMOVE);
	if (Msg.message == WM_APP + 0x8000)
	{
		WaitForSingleObject(CommandQueueFenceEvent, INFINITE);
		return 1;
	}

	pCommandAllocator[CurrentCommandList]->Reset();
	pCommandList[CurrentCommandList]->Reset(pCommandAllocator[CurrentCommandList], 0);

	D3D12_RESOURCE_BARRIER rb{};
	rb.Transition.pResource = pBB[CurrentBackBuffer];
	rb.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	rb.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	pCommandList[CurrentCommandList]->ResourceBarrier(1, &rb);

	ID3D12GraphicsCommandList*& pContext = pCommandList[CurrentCommandList].pT;
	pContext->OMSetRenderTargets(1, &pRTV[CurrentBackBuffer], 0, 0);

	_APL_GraphicsContext GraphicsContext{ pBB[CurrentBackBuffer],pRTV[CurrentBackBuffer], pCommandList[CurrentCommandList] };
	RenderProc(*(APL::GraphicsContext*)&GraphicsContext);

	rb.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	rb.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
	pCommandList[CurrentCommandList]->ResourceBarrier(1, &rb);

	pCommandList[CurrentCommandList]->Close();

	WaitForSingleObject(CommandQueueFenceEvent, INFINITE);

	pCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList**)&pCommandList[CurrentCommandList]);
	pSwapChain->Present(0, 0);
	pCommandQueue->Signal(pCommandQueueFence, CommandQueueFenceValue);
	pCommandQueueFence->SetEventOnCompletion(CommandQueueFenceValue++, CommandQueueFenceEvent);

	CurrentBackBuffer++;
	if (CurrentBackBuffer == TotalBackBuffers)
		CurrentBackBuffer = 0;

	CurrentCommandList++;
	if (CurrentCommandList == TotalCommandLists)
		CurrentCommandList = 0;

	goto MainLoop;
	return 0;
}

INT _APL_GraphicsSetup()
{
	if (CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&_APL_pFactory)) != S_OK)
		return 0;

	if (D3D12GetDebugInterface(IID_PPV_ARGS(&_APL_pDebug)) != S_OK)
		return 0;

	_APL_pDebug->EnableDebugLayer();

	HANDLE Event = CreateEvent(0, 0, 0, 0);
	if (!Event)
		return 0;

	if (!CreateThread(0, 0, _APL_WindowProc, Event, 0, 0))
		return 0;

	WaitForSingleObject(Event, INFINITE);
	if (!_APL_WindowProcHwnd)
		return 0;

	CloseHandle(Event);

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace APL
{
	class Window : _APL_Window
	{
	public:
		~Window()
		{
			PostThreadMessage(GetThreadId(RenderThread), WM_APP + 0x8000, 0, 0);
			WaitForSingleObject(RenderThread, INFINITE);
		}
		VOID SetTitle(LPCWSTR Title)
		{
			SetWindowText(Hwnd, Title);
		}
		VOID Show()
		{
			ShowWindow(Hwnd, 1);
		}
		VOID Hide()
		{
			ShowWindow(Hwnd, 0);
		}
		bool operator = (WindowReference a)
		{
			if (a.Hwnd == Hwnd)
				return 1;

			return 0;
		}
	};
	bool WindowReference::operator = (Window a)
	{
		_APL_Window* A = (_APL_Window*)&a;
		if (A->Hwnd == Hwnd)
			return 1;

		return 0;
	}

	class Buffer : _APL_Buffer
	{
	public:

	};

	class Core3D : _APL_Core3D
	{
	public:
		INT CreateUploadBuffer(UINT64 Size, Buffer* pBuffer)
		{
			_APL_Buffer* Buffer = (_APL_Buffer*)pBuffer;

			D3D12_HEAP_PROPERTIES hp{};
			hp.Type = D3D12_HEAP_TYPE_UPLOAD;
			D3D12_RESOURCE_DESC rd{};
			rd.Height = 1;
			rd.DepthOrArraySize = 1;
			rd.SampleDesc.Count = 1;
			rd.MipLevels = 1;
			rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			rd.Width = Size;
			rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			if (pDevice->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_COMMON, 0, IID_PPV_ARGS(&Buffer->pBuffer)) != S_OK)
				return 0;

			return 1;
		}
		INT CreateVertexBuffer(UINT Size, UINT Stride, Buffer* pBuffer, VertexBufferView* pView)
		{
			_APL_Buffer* Buffer = (_APL_Buffer*)pBuffer;
			_APL_VertexBufferView* View = (_APL_VertexBufferView*)pView;

			D3D12_HEAP_PROPERTIES hp{};
			hp.Type = D3D12_HEAP_TYPE_DEFAULT;
			D3D12_RESOURCE_DESC rd{};
			rd.Height = 1;
			rd.DepthOrArraySize = 1;
			rd.SampleDesc.Count = 1;
			rd.MipLevels = 1;
			rd.Width = Size;
			rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			if (pDevice->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_COMMON, 0, IID_PPV_ARGS(&Buffer->pBuffer)) != S_OK)
				return 0;

			View->vbv.BufferLocation = Buffer->pBuffer->GetGPUVirtualAddress();
			View->vbv.SizeInBytes = Size;
			View->vbv.StrideInBytes = Stride;

			return 1;
		}
		//Must have 256 byte alignment.
		INT CreateConstantBuffer(UINT64 Size, Buffer* pBuffer)
		{
			_APL_Buffer* Buffer = (_APL_Buffer*)pBuffer;

			D3D12_HEAP_PROPERTIES hp{};
			hp.Type = D3D12_HEAP_TYPE_DEFAULT;
			D3D12_RESOURCE_DESC rd{};
			rd.Height = 1;
			rd.DepthOrArraySize = 1;
			rd.SampleDesc.Count = 1;
			rd.MipLevels = 1;
			rd.Width = Size;
			rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			if (pDevice->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_COMMON, 0, IID_PPV_ARGS(&Buffer->pBuffer)) != S_OK)
				return 0;

			return 1;
		}
		INT CreateTextureBuffer(UINT32 Width, UINT32 Height, DXGI_FORMAT Format, Buffer* pBuffer)
		{
			_APL_Buffer* Buffer = (_APL_Buffer*)pBuffer;

			D3D12_HEAP_PROPERTIES hp{};
			hp.Type = D3D12_HEAP_TYPE_DEFAULT;
			D3D12_RESOURCE_DESC rd{};
			rd.DepthOrArraySize = 1;
			rd.SampleDesc.Count = 1;
			rd.MipLevels = 1;
			rd.Width = Width;
			rd.Height = Height;
			rd.Format = Format;
			rd.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			rd.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			if (pDevice->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, 0, IID_PPV_ARGS(&Buffer->pBuffer)) != S_OK)
				return 0;

			return 1;
		}

		//Creates a DescriptorHeap that stores Constant, Texture, and Unordered descriptors.
		INT CreateConstantDescHeap(UINT32 TotalDescs, DescriptorHeap* pDescHeap)
		{
			D3D12_DESCRIPTOR_HEAP_DESC dhd{};
			dhd.NumDescriptors = TotalDescs;
			dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			dhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			if (pDevice->CreateDescriptorHeap(&dhd, IID_PPV_ARGS((ID3D12DescriptorHeap**)pDescHeap)) != S_OK)
				return 0;

			return 1;
		}

		//Creates a DescriptorHeap that stores Sampler descriptors.
		INT CreateSamplerDescHeap(UINT32 TotalDescs, DescriptorHeap* pSamplerHeap)
		{
			D3D12_DESCRIPTOR_HEAP_DESC dhd{};
			dhd.NumDescriptors = TotalDescs;
			dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			dhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			if (pDevice->CreateDescriptorHeap(&dhd, IID_PPV_ARGS((ID3D12DescriptorHeap**)pSamplerHeap)) != S_OK)
				return 0;

			return 1;
		}

		//Creates a Constant descriptor in a Constant DescriptorHeap.
		VOID CreateConstantDesc(Buffer* pBuffer, UINT32 BufferSize, UINT32 HeapIndex, DescriptorHeap* pDescHeap)
		{
			ID3D12Resource* Buffer = *(ID3D12Resource**)pBuffer;
			ID3D12DescriptorHeap*& pHeap = *(ID3D12DescriptorHeap**)pDescHeap;
			D3D12_CPU_DESCRIPTOR_HANDLE Handle = pHeap->GetCPUDescriptorHandleForHeapStart();

			Handle.ptr += HeapIndex * CbvIncrementSize;

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvd{};
			cbvd.BufferLocation = Buffer->GetGPUVirtualAddress();
			cbvd.SizeInBytes = BufferSize;

			pDevice->CreateConstantBufferView(&cbvd, Handle);
		}

		//Creates a Texture descriptor in a Constant DescriptorHeap.
		VOID CreateTextureDesc(Buffer* pBuffer, UINT32 HeapIndex, DescriptorHeap* pDescHeap)
		{
			ID3D12Resource* Buffer = *(ID3D12Resource**)pBuffer;
			ID3D12DescriptorHeap*& pHeap = *(ID3D12DescriptorHeap**)pDescHeap;
			D3D12_CPU_DESCRIPTOR_HANDLE Handle = pHeap->GetCPUDescriptorHandleForHeapStart();

			Handle.ptr += HeapIndex * CbvIncrementSize;

			D3D12_SHADER_RESOURCE_VIEW_DESC srvd{};
			srvd.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvd.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvd.Texture2D.MipLevels = 1;
			pDevice->CreateShaderResourceView(Buffer, &srvd, Handle);
		}

		//Creates a Sampler descriptor in a Sampler DescriptorHeap.
		VOID CreateSamplerDesc(UINT32 Index, DescriptorHeap* pDescHeap)
		{
			ID3D12DescriptorHeap* DescHeap = *(ID3D12DescriptorHeap**)pDescHeap;
			D3D12_CPU_DESCRIPTOR_HANDLE Handle = DescHeap->GetCPUDescriptorHandleForHeapStart();
			Handle.ptr += Index * SamplerIncrementSize;

			D3D12_SAMPLER_DESC sd{};
			sd.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			sd.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sd.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sd.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sd.MinLOD = 0.0f;
			sd.MaxLOD = D3D12_FLOAT32_MAX;
			sd.MipLODBias = 0.0f;
			sd.MaxAnisotropy = 1;
			sd.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			sd.BorderColor[0] = 0.0f;
			sd.BorderColor[1] = 0.0f;
			sd.BorderColor[2] = 0.0f;
			sd.BorderColor[3] = 0.0f;
			pDevice->CreateSampler(&sd, Handle);
		}

		//Creates a DescriptorHandle for Constant, Texture, and Unordered descriptors.
		VOID CreateConstantDescriptorHandle(DescriptorHeap* pConstantDescHeap, UINT32 Index, DescriptorHandle* pHandle)
		{
			ID3D12DescriptorHeap* Heap = *(ID3D12DescriptorHeap**)pConstantDescHeap;
			D3D12_GPU_DESCRIPTOR_HANDLE Handle = Heap->GetGPUDescriptorHandleForHeapStart();
			Handle.ptr += CbvIncrementSize * Index;

			*pHandle = *(DescriptorHandle*)&Handle;
		}

		//Creates a DescriptorHandle for Sampler descriptors.
		VOID CreateSamplerDescriptorHandle(DescriptorHeap* pSamplerDescHeap, UINT32 Index, DescriptorHandle* pHandle)
		{
			ID3D12DescriptorHeap* Heap = *(ID3D12DescriptorHeap**)pSamplerDescHeap;
			D3D12_GPU_DESCRIPTOR_HANDLE Handle = Heap->GetGPUDescriptorHandleForHeapStart();
			Handle.ptr += SamplerIncrementSize * Index;

			*pHandle = *(DescriptorHandle*)&Handle;
		}

		INT CreateShaderPack(ShaderPackDesc* pDesc, ShaderPack* pPack)
		{
			_APL_ShaderPack* Pack = (_APL_ShaderPack*)pPack;

			if (pDevice->CreateRootSignature(0, pDesc->VS.pShaderBytecode, pDesc->VS.BytecodeLength, IID_PPV_ARGS(&Pack->pRootSignature)) != S_OK)
				return 0;

			D3D12_BLEND_DESC bd;
			bd.AlphaToCoverageEnable = FALSE;
			bd.IndependentBlendEnable = FALSE;
			bd.RenderTarget[0].BlendEnable = FALSE;
			bd.RenderTarget[0].LogicOpEnable = FALSE;
			bd.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
			bd.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
			bd.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			bd.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
			bd.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			bd.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			
			D3D12_INPUT_ELEMENT_DESC ied[2]{ {"Position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
												{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} };

			D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsd{};
			gpsd.VS = pDesc->VS;
			gpsd.PS = pDesc->PS;
			gpsd.DS = pDesc->DS;
			gpsd.HS = pDesc->HS;
			gpsd.GS = pDesc->GS;
			gpsd.BlendState = bd;
			gpsd.InputLayout;
			gpsd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			gpsd.NumRenderTargets = 1;
			gpsd.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
			gpsd.SampleDesc.Count = 1;
			gpsd.SampleMask = 0xffffffff;
			gpsd.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
			gpsd.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
			gpsd.InputLayout.NumElements = 2;
			gpsd.InputLayout.pInputElementDescs = ied;
			if (pDevice->CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(&Pack->pPipelineState)) != S_OK)
				return 0;

			return 1;
		}

		INT CopyDataToUpload(Buffer* pDst, LPVOID pSrc, UINT64 Size)
		{
			_APL_Buffer* Buffer = (_APL_Buffer*)pDst;

			LPVOID Dst;
			if (Buffer->pBuffer->Map(0, 0, &Dst) != S_OK)
				return 0;

			memcpy(Dst, pSrc, Size);

			Buffer->pBuffer->Unmap(0, 0);

			return 1;
		}
		
		//Pass in the address of a APL::File only.
		INT CopyFileToUpload(Buffer* pDst, LPVOID pSrc, UINT32 Size)
		{
			_APL_Buffer* Buffer = (_APL_Buffer*)pDst;
			LPVOID Dst;
			if (Buffer->pBuffer->Map(0, 0, &Dst) != S_OK)
				return 0;

			if (!ReadFile(*(LPVOID*)pSrc, Dst, Size, 0, 0))
				return 0;

			Buffer->pBuffer->Unmap(0, 0);

			return 1;
		}
		
		INT CopyFileToUpload(Buffer* pDst, LPVOID pSrc, UINT32 Size, UINT64 Offset)
		{
			_APL_Buffer* Buffer = (_APL_Buffer*)pDst;
			LPVOID Dst;
			if (Buffer->pBuffer->Map(0, 0, &Dst) != S_OK)
				return 0;

			OVERLAPPED ol{};
			UINT64* pOffset = (UINT64*)&ol.Offset;
			*pOffset = Offset;

			if (!ReadFile(*(LPVOID*)pSrc, Dst, 0, 0, &ol))
				return 0;

			Buffer->pBuffer->Unmap(0, 0);

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

			D3D12_RESOURCE_BARRIER rb[2]{};
			rb->Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			rb->Transition.pResource = Dst->pBuffer;
			rb->Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
			rb->Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
			rb[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			rb[1].Transition.pResource = Src->pBuffer;
			rb[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
			rb[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
			pCommandList->ResourceBarrier(2, rb);

			pCommandList->CopyBufferRegion(Dst->pBuffer, DstOffset, Src->pBuffer, SrcOffset, Size);

			rb->Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			rb->Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
			rb[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
			rb[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
			pCommandList->ResourceBarrier(2, rb);
		}
		
		VOID CopyUploadToTexture(Buffer* pDstTexture, UINT xPos, UINT yPos, UINT zPos, Buffer* pSrcUpload, UINT32 Width, UINT32 Height, DXGI_FORMAT Format, UINT32 RowPitch)
		{
			_APL_Buffer* Dst = (_APL_Buffer*)pDstTexture;
			_APL_Buffer* Src = (_APL_Buffer*)pSrcUpload;

			D3D12_RESOURCE_BARRIER rb[2]{};
			rb->Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			rb->Transition.pResource = Dst->pBuffer;
			rb->Transition.StateBefore = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
			rb->Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
			rb[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			rb[1].Transition.pResource = Src->pBuffer;
			rb[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
			rb[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
			pCommandList->ResourceBarrier(2, rb);

			D3D12_TEXTURE_COPY_LOCATION DstDesc{};
			DstDesc.pResource = Dst->pBuffer;
			DstDesc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

			D3D12_TEXTURE_COPY_LOCATION SrcDesc{};
			SrcDesc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			SrcDesc.PlacedFootprint.Footprint.Format = Format;
			SrcDesc.pResource = Src->pBuffer;
			SrcDesc.PlacedFootprint.Footprint.Width = Width;
			SrcDesc.PlacedFootprint.Footprint.Height = Height;
			SrcDesc.PlacedFootprint.Footprint.RowPitch = RowPitch;
			SrcDesc.PlacedFootprint.Footprint.Depth = 1;
			pCommandList->CopyTextureRegion(&DstDesc, xPos, yPos, zPos, &SrcDesc, 0);

			rb->Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			rb->Transition.StateAfter = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
			rb[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
			rb[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
			pCommandList->ResourceBarrier(2, rb);
		}
		/*VOID CopyTextureToTexture()
		{

		}*/

		INT ExecuteCopyCommands()
		{
			if (pCommandList->Close() != S_OK)
				return 0;

			pCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList**)&pCommandList);
			pCommandQueue->Signal(pCommandQueueFence, CommandQueueFenceValue);
			pCommandQueueFence->SetEventOnCompletion(CommandQueueFenceValue++, CommandQueueFenceEvent);
			WaitForSingleObject(CommandQueueFenceEvent, INFINITE);
			pCommandAllocator->Reset();
			pCommandList->Reset(pCommandAllocator, 0);

			return 1;
		}

		INT CreateWindow(WindowInfo* pInfo, Window* pWindow)
		{
			_APL_Window* Window = (_APL_Window*)pWindow;

			struct STACK
			{
				HANDLE Event;
				WindowInfo* pInfo;
				_APL_Window* pWindow;
				ID3D12Device* pDevice;
			}Stack{ CreateEvent(0, 0, 0, 0), pInfo, (_APL_Window*)pWindow, pDevice };
			if (!Stack.Event)
				return 0;

			if (!(Window->RenderThread = CreateThread(0, 0, _APL_RenderProc, &Stack, 0, 0)))
			{
				CloseHandle(Stack.Event);
				return 0;
			}

			WaitForSingleObject(Stack.Event, INFINITE);
			if (!Stack.pWindow->Hwnd)
			{
				CloseHandle(Stack.Event);
				return 0;
			}

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
			pAdapter->GetDesc(pInfo);
		}
		INT GetMonitorInfo(UINT Monitor, MonitorInfo* pInfo)
		{
			_APL_COM<IDXGIOutput> pOutput;
			if (pAdapter->EnumOutputs(Monitor, &pOutput) != S_OK)
				return 0;

			DXGI_OUTPUT_DESC od{};
			pOutput->GetDesc(&od);
			if (od.AttachedToDesktop)
			{
				pInfo->cbSize = sizeof(MONITORINFOEX);
				if (!GetMonitorInfoW(od.Monitor, pInfo))
					return 0;

				/*DISPLAY_DEVICE dd{};
				dd.cb = sizeof(DISPLAY_DEVICE);
				if (!EnumDisplayDevices(od.DeviceName, 0, &dd, 0))
					return 0;

				memcpy(od.DeviceName, dd.DeviceName, 64);*/
			}

			return 1;
		}
		INT CreateCore3D(Core3D* pCore)
		{
			_APL_Core3D* Core = (_APL_Core3D*)pCore;
			if (D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_1, IID_PPV_ARGS(&Core->pDevice)) != S_OK)
				return 0;

			ID3D12Device*& pDevice = Core->pDevice.pT;
			D3D12_COMMAND_QUEUE_DESC cqd{ D3D12_COMMAND_LIST_TYPE_DIRECT };
			if (pDevice->CreateCommandQueue(&cqd, IID_PPV_ARGS(&Core->pCommandQueue)) != S_OK)
				return 0;

			if (pDevice->CreateFence(Core->CommandQueueFenceValue++, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Core->pCommandQueueFence)) != S_OK)
				return 0;

			if (!(Core->CommandQueueFenceEvent = CreateEvent(0, 0, 0, 0)))
				return 0;

			if (pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&Core->pCommandAllocator)) != S_OK)
				return 0;

			if (pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, Core->pCommandAllocator, 0, IID_PPV_ARGS(&Core->pCommandList)) != S_OK)
				return 0;

			Core->CbvIncrementSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			Core->SamplerIncrementSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

			return 1;
		}
	};

	INT GetDevice(UINT Device, APL::Device* pDevice)
	{
		pDevice->~Device();
		_APL_Device* AlsoDevice = (_APL_Device*)pDevice;

		if (_APL_pFactory->EnumAdapters(Device, &AlsoDevice->pAdapter) != S_OK)
			return 0;

		return 1;
	}
}