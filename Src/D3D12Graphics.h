struct _APL_Window
{
	DWORD RendererThreadId;
	HWND Hwnd;
};

namespace APL
{
	typedef RECT Rect;

	class Window;

	typedef VOID(*RenderProc)();

	struct WindowDesc
	{
		Rect Rect;
		Window* ParentWindow;
		RenderProc RenderProc;
	};

	enum InputType
	{
		Keyboard,
		Mouse,
		Close
	};
	struct Input
	{
		InputType Type;
		union
		{
			struct Keyboard
			{

			};
			struct Mouse
			{

			};
			Window* Window;
		};
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HWND _APL_WindowThreadHwnd;

LRESULT WINAPI _APL_MessageProc(HWND Hwnd, UINT Msg, WPARAM Wp, LPARAM Lp)
{
	switch (Msg)
	{
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 1;
	}
	case WM_APP:
	{
		APL::WindowDesc* pDesc = (APL::WindowDesc*)Wp;
		return (LRESULT)CreateWindow(L"wc", 0, WS_OVERLAPPEDWINDOW, pDesc->Rect.left, pDesc->Rect.top, pDesc->Rect.right - pDesc->Rect.left, pDesc->Rect.bottom - pDesc->Rect.top, 0, 0, 0, 0);
	}
	case WM_APP + 1:
	{
		_APL_Window* pWindow = (_APL_Window*)Wp;
		DestroyWindow(pWindow->Hwnd);
		PostThreadMessage(pWindow->RendererThreadId, WM_APP, 0, 0);
		return 0;
	}
	default:
		return DefWindowProc(Hwnd, Msg, Wp, Lp);
	}
}
DWORD WINAPI _APL_WindowThreadProc(LPVOID pV)
{
	WNDCLASS wc{};
	wc.lpszClassName = L"wc";
	wc.lpfnWndProc = _APL_MessageProc;
	RegisterClass(&wc);

	_APL_WindowThreadHwnd = CreateWindow(L"wc", 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);

	SetEvent(pV);
	if (!_APL_WindowThreadHwnd)
		return 0;

	MSG Msg{};
	while (1)
	{
		GetMessage(&Msg, 0, 0, 0);
		DispatchMessage(&Msg);
	}

	return 0;
}

DWORD WINAPI _APL_RendererProc(LPVOID pV)
{
	struct STACK
	{
		HANDLE Event;
		APL::WindowDesc* pDesc;
		_APL_Window* pWindow;
	}Stack{*(STACK*)pV};

	

	MSG TempMsg{};
	PostThreadMessage(GetCurrentThreadId(), WM_APP, 0, 0);
	PeekMessage(&TempMsg, 0, 0, 0, PM_REMOVE);
	if (TempMsg.message != WM_APP)
	{
		SetEvent(Stack.Event);
		return 0;
	}

	Stack.pWindow->Hwnd = (HWND)SendMessage(_APL_WindowThreadHwnd, WM_APP, (WPARAM)Stack.pDesc, 0);
	if (!Stack.pWindow->Hwnd)
	{
		SetEvent(Stack.Event);
		return 0;
	}
	


	SetEvent(Stack.Event);
	goto MainLoop;

Error:
	DestroyWindow(Stack.pWindow->Hwnd);
	Stack.pWindow->Hwnd = 0;
	SetEvent(Stack.Event);
	return 0;

MainLoop:
	MSG Msg{};
	PeekMessage(&Msg, 0, 0, 0, PM_REMOVE);

	if (Msg.message == WM_APP)
	{
		CloseHandle(Stack.Event);
		return 0;
	}


	goto MainLoop;
}

INT _APL_GraphicsSetup()
{
	HANDLE Event;
	if ((Event = CreateEvent(0, 0, 0, 0)) == 0)
		return 0;

	if (!CreateThread(0, 0, _APL_WindowThreadProc, Event, 0, 0))
		return 0;

	WaitForSingleObject(Event, INFINITE);
	if (!_APL_WindowThreadHwnd)
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
		VOID Show(UINT ShowCommand)
		{
			ShowWindow(Hwnd, ShowCommand);
		}
		~Window()
		{
			SendMessage(_APL_WindowThreadHwnd, WM_APP + 1, (WPARAM)this, 0);
		}
	};
	INT NewWindow(WindowDesc* pDesc, Window* pWindow)
	{
		_APL_Window* AmbiguisWindow = (_APL_Window*)pWindow;

		struct
		{
			HANDLE Event;
			WindowDesc* pDesc;
			_APL_Window* pWindow;
		}Stack{ CreateEvent(0, 0, 0, 0), pDesc, AmbiguisWindow};
		if (!Stack.Event)
			return 0;

		if (!CreateThread(0, 0, _APL_RendererProc, &Stack, 0, &AmbiguisWindow->RendererThreadId))
		{
			CloseHandle(Stack.Event);
			return 0;
		}

		WaitForSingleObject(Stack.Event, INFINITE);
		if (!AmbiguisWindow->Hwnd)
		{
			CloseHandle(Stack.Event);
			return 0;
		}

		return 1;
	}

	VOID GetInput(Input& pInput)
	{

	}
}