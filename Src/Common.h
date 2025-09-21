#define _APL_Common_Header

#ifdef _WIN32
#include <Windows.h>
#undef VOID
typedef void VOID;
typedef unsigned wchar_t UWCHAR;
#endif

#ifdef __linux__
#include <cstdlib>
#include <sys/socket.h>
#include <sys/un.h>
typedef void VOID;
typedef char CHAR;
typedef unsigned char UCHAR;
typedef wchar_t WCHAR;
typedef unsigned wchar_t UWCHAR;
typedef long INT;
typedef unsigned long UINT;
typedef INT LONG;
typedef UINT ULONG;
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;
typedef CHAR INT8;
typedef UCHAR UINT8;
typedef WCHAR INT16;
typedef UWCHAR UINT16;
typedef INT INT32;
typedef UINT UINT32;
typedef LONGLONG INT64;
typedef ULONGLONG UINT64;
#endif
