#define _APL_Common_Header

#ifdef _WIN32
#include <Windows.h>
#undef VOID
typedef void VOID;
typedef VOID* LPVOID;
typedef CHAR* LPCHAR;
typedef WCHAR* LPWCHAR;
typedef INT8* LPINT8;
typedef INT16* LPINT16;
typedef INT32* LPINT32;
typedef INT64* LPINT64;
typedef UINT8* LPUINT8;
typedef UINT16* LPUINT16;
typedef UINT32* LPUINT32;
typedef UINT64* LPUINT64;
typedef FLOAT* LPFLOAT;
typedef DOUBLE* LPDOUBLE;
#endif

#ifdef __linux__
#include <cstdlib>
#include <sys/socket.h>
#include <sys/un.h>
#include <Vulkan.h>
typedef void VOID;
typedef char CHAR;
typedef wchar_t WCHAR;
typedef short SHORT;
typedef int INT;
typedef long LONG;
typedef long long LONGLONG;
typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef unsigned long long ULONGLONG;
typedef CHAR INT8;
typedef SHORT INT16;
typedef INT INT32;
typedef LONGLONG INT64;
typedef UCHAR UINT8;
typedef unsigned short UINT16;
typedef UINT UINT32;
typedef ULONGLONG UINT64;
typedef float FLOAT;
typedef double DOUBLE;
typedef VOID* LPVOID;
typedef CHAR* LPCHAR;
typedef INT8* LPINT8;
typedef INT16* LPINT16;
typedef INT32* LPINT32;
typedef INT64* LPINT64;
typedef UINT8* LPUINT8;
typedef UINT16* LPUINT16;
typedef UINT32* LPUINT32;
typedef UINT64* LPUINT64;
typedef FLOAT* LPFLOAT;
typedef DOUBLE* LPDOUBLE;
#endif
