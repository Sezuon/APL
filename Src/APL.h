
VOID Main();
INT WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, INT)
{
#ifdef _APL_Graphics_Header
    if (!_APL_GraphicsSetup())
        return 1;
#endif

    Main();

    return 0;
}