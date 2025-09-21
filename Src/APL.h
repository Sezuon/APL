#ifdef _WIN32
VOID Main();
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ INT)
{
#ifdef _APL_Clock_Header
	QueryPerformanceFrequency(&_APL_Clock_Frequency);
#endif

#ifdef _G3D_Memory_Header
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	APL::NewGranularity = si.dwAllocationGranularity;
#endif

#ifdef _APL_Graphics_Header
	if (!_APL_GraphicsSetup())
		return 0;
#endif

#ifdef _G3D_Network_header
	if (!_G3DNetworkSetup())
		return 0;-
#endif

	Main();

#ifdef _G3D_Network_header
	WSACleanup();
#endif

	return 0;
}
#endif

//Need to handle WM_DISPLAYCHANGE.
//G3DNew needs allocate memory in multiples of the Alloc.
//Dont forget about the DXGI/D3D debug layer.
//In the docs mention the need for -mwindows -municode.

#ifdef __linux__
VOID Main();
INT main()
{
	if(!_APL_GraphicsSetup())
		return 1;
	
	Main();

	return 0;
}
#endif
