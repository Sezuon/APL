#define _APL_File_Header
#ifndef _APL_Common_Header
#include "Common.h"
#endif

#ifdef __WINDOWS__
namespace APL
{
	class File
	{
		HANDLE hFile;
	public:
		File(LPCWSTR FileName, DWORD Access, DWORD Disposition)
		{
			hFile = CreateFile(FileName, Access, 0, 0, Disposition, 0, 0);
		}
		~File()
		{
			CloseHandle(hFile);
		}
		INT IsOpen()
		{
			if (hFile == INVALID_HANDLE_VALUE)
				return 0;

			return 1;
		}
		INT Read(LPVOID pBuffer)
		{
			return ReadFile(hFile, pBuffer, (DWORD)Size(), 0, 0);
		}
		INT Read(LPVOID pBuffer, DWORD BytesToRead)
		{
			return ReadFile(hFile, pBuffer, BytesToRead, 0, 0);
		}
		INT Write(LPVOID pBuffer, DWORD BytesToWrite)
		{
			return WriteFile(hFile, pBuffer, BytesToWrite, 0, 0);
		}
		UINT64 Size()
		{
			LARGE_INTEGER li{};
			GetFileSizeEx(hFile, &li);
			return (UINT64)li.QuadPart;
		}
		//This is analogus to a LPVOID so be careful.
		operator HANDLE()
		{
			return hFile;
		}
	};
}
#endif

#ifdef __linux__

#endif
