#define _APL_Common

#ifdef _WIN32
#include <Windows.h>
#undef CreateWindow
#undef GetMonitorInfo
#undef VOID
typedef void VOID;
#else
#include <sys/syscall.h>
typedef void VOID;
typedef char CHAR;
typedef char BYTE;
typedef int INT;
typedef char INT8;
typedef wchar_t INT16;
typedef int INT32;
typedef long long INT64;

typedef unsigned int UINT;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef float FLOAT;

typedef VOID* LPVOID;
typedef CHAR* LPCHAR;

typedef const char* LPCSTR;
typedef const wchar_t* LPCWSTR;
#endif