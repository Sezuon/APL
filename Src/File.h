#ifndef _APL_Common
#include "Common.h"
#endif

namespace APL
{
	class File
	{
		HANDLE hFile;
	public:
		File(LPCWSTR File, DWORD Access, DWORD Disposition)
		{
			hFile = CreateFile(File, Access, 0, 0, Disposition, 0, 0);
		}
		~File()
		{
			CloseHandle(hFile);
		}

		INT Open()
		{
			if (hFile == INVALID_HANDLE_VALUE)
				return 0;

			return 1;
		}
		UINT64 Size()
		{
			LARGE_INTEGER li{};
			GetFileSizeEx(hFile, &li);
			return (UINT64)li.QuadPart;
		}
		INT Read(LPVOID pBuffer, UINT32 Size)
		{
			return ReadFile(hFile, pBuffer, Size, 0, 0);
		}
		INT Read(LPVOID pBuffer, UINT32 Size, UINT64 Offset)
		{
			OVERLAPPED ol{};
			UINT64* pOffset = (UINT64*)&ol.Offset;
			*pOffset = Offset;

			return ReadFile(hFile, pBuffer, Size, 0, &ol);
		}
		INT Write(LPVOID pBuffer, UINT32 Size)
		{
			return WriteFile(hFile, pBuffer, Size, 0, 0);
		}
		INT Write(LPVOID pBuffer, UINT32 Size, UINT64 Offset)
		{
			OVERLAPPED ol{};
			UINT64* pOffset = (UINT64*)&ol.Offset;
			*pOffset = Offset;

			return WriteFile(hFile, pBuffer, Size, 0, &ol);
		}
	};
}