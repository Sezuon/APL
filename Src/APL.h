VOID Main();
INT WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ INT)
{
#ifdef _APL_Graphics
	if (!_APL_GraphicsSetup())
		return 1;
#endif

	Main();
	return 0;
}

//Don't forget to remove DXGI/D3D12 debug layer.
//Get thread stack sizes as small as possible.
//Does GetMonitorInfo() need to check if display is attached?
//GetMonitorInfo SUUUUUCKS.
//Research if passing null as the IDXGIDevice into DCompositonCreateDevice is problomatic.