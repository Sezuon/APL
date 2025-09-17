#define _APL_Common_Header
#ifdef __WIN64__
#include <Windows.h>
#undef VOID
typedef void VOID;
#endif

#ifdef __linux__
#include <cstdlib>
#include <sys/socket.h>
#include <sys/un.h>
typedef void VOID;
typedef char CHAR;
typedef wchar_t WCHAR;
typedef int INT;
typedef unsigned int UINT;
typedef char* LPCHAR;
#endif
