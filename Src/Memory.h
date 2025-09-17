#define _G3D_Memory_Header

#ifndef _APL_Common_Header
#include "Common.h"
#endif

#ifdef _WIN32
namespace APL
{
	DWORD NewGranularity;
	LPVOID New(LPVOID pAddress, UINT64 Size)
	{
		return VirtualAlloc(pAddress, Size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	}
	VOID Delete(LPVOID pV)
	{
		VirtualFree(pV, 0, MEM_RELEASE);
	}

	template <typename Type>
	class Ptr
	{
		Type* pT{};
	public:
		Ptr(LPVOID Ptr)
		{
			pT = (Type*)Ptr;
		}
		~Ptr()
		{
			Delete(pT);
		}
		operator Type*()
		{
			return pT;
		}
		Type* operator->()
		{
			return pT;
		}
		Type** operator&()
		{
			return &pT;
		}
		Type operator [](UINT64 a)
		{
			return pT[a];
		}
	};
}
#endif

#ifdef __linux__

#endif
