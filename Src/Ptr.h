#define _APL_Memory

#ifndef _APL_Common
#include "Common.h"
#endif

namespace APL
{
	template <typename Type>
	struct Ptr
	{
		Type* pT{};
		Ptr(LPVOID pV)
		{
			pT = (Type*)pV;
		}
		~Ptr()
		{
			VirtualFree(pT, 0, MEM_RELEASE);
		}
		/*bool operator == (Ptr a)
		{
			if (a.pT == pT)
				return 1;

			return 0;
		}*/
		operator LPVOID()
		{
			return (LPVOID)pT;
		}
		Type* operator +(Ptr a)
		{
			return pT + a.pT;
		}
		Type* operator +(UINT64 a)
		{
			return pT + a;
		}
	};

	LPVOID New(UINT64 Size, LPVOID pMem)
	{
		return VirtualAlloc(pMem, Size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	}
}