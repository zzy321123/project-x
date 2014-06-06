#ifndef _BASE_HEADER_
#define _BASE_HEADER_

/************************************************************************/
/* 平台编译定义                                                          */
/************************************************************************/
#if defined(WIN32) || defined(_WIN32)
#define PLATFORM_WINDOWS
#else
#define PLATFORM_LINUX
#endif

//统一内存池
#define _MALLOC

//引入Mysql
#define UTIL_MYSQL

#define WIN32_API

#define FRAME_API

/************************************************************************/
/* 统一头文件										                    */
/************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cassert>
#include <csignal>
#include <cmath>
#include <cstdarg>
#include <cerrno>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>

/************************************************************************/
/* STL库包含文件                                                        */
/************************************************************************/
#include <vector>
#include <map>
#include <list>
#include <string>
#include <set>
#include <queue>
#include <ostream>
#include <istream>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

/************************************************************************/
/* WINDOWS平台特殊定义													*/
/************************************************************************/
#ifdef PLATFORM_WINDOWS
//防windows.h和winsock2冲突
#define WIN32_LEAN_AND_MEAN
#define _fseek64   _fseeki64
#define _ftell64   _ftelli64
//头文件
#include <io.h>
#include <tchar.h>
#include <direct.h>
#include <conio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mmsystem.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <shlwapi.h>
//库导入
#pragma  comment(lib, "winmm.lib")
#pragma  comment(lib, "ws2_32.lib")
#pragma  comment(lib, "shlwapi.lib")
#pragma  comment(lib, "dbghelp.lib")
#pragma  comment(lib, "psapi.lib")
//屏蔽特定警告
#pragma warning (disable:4290)
#pragma warning (disable:4996)
#pragma warning (disable:4793)
#endif

/************************************************************************/
/* socket操作错误码检查													*/
/************************************************************************/
#ifdef PLATFORM_WINDOWS
#define CHECK_ERROR(e)					(WSAGetLastError() == WSA##(e))
#define CHECK_ERROR_RW_RETRIABLE(e)		((e) == WSAEWOULDBLOCK || (e) == WSAEINTR)
#define CHECK_ERROR_CONNECT_RETRIABLE(e) ((e) == WSAEWOULDBLOCK || (e) == WSAEINTR || (e) == WSAEINPROGRESS || (e) == WSAEINVAL)
#define CHECK_ERROR_ACCEPT_RETRIABLE(e)  CHECK_ERROR_RW_RETRIABLE(e)
#define CHECK_ERROR_CONNECT_REFUSED(e)	((e) == WSAECONNREFUSED)
#else
#define CHECK_ERROR(e)					(errno == (e))
#define CHECK_ERROR_RW_RETRIABLE(e)		((e) == EINTR || (e) == EAGAIN)
#define CHECK_ERROR_CONNECT_RETRIABLE(e) ((e) == EINTR || (e) == EINPROGRESS)
#define CHECK_ERROR_ACCEPT_RETRIABLE(e)  ((e) == EINTR || (e) == EAGAIN || (e) == ECONNABORTED)
#define CHECK_ERROR_CONNECT_REFUSED(e)   ((e) == ECONNREFUSED)
#endif

/************************************************************************/
/* LINUX平台特殊定义                                                     */
/************************************************************************/
#ifdef PLATFORM_LINUX
//平台差异化类型定义
#define SOCKET int
#define HANDLE void*
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (-1)
#define closesocket(s) close(s)
#define _fseeki64 fseeko64
#define _ftelli64 ftello64
#define _chdir chdir
#define _getcwd	getcwd
#define _getch getch
#define _chsize_s ftruncate64
#define _chsize ftruncate
#define _snprintf snprintf
#define _vsnprintf vsnprintf
#define _atoi64 atoll
//头文件
#include <unistd.h>
#include <signal.h> 
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <sys/io.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <sys/syscall.h>
#include <sys/sysinfo.h>
#endif

/************************************************************************/
/* 常量定义																*/
/************************************************************************/
#ifndef UTIL_API
#define UTIL_API
#endif

#ifndef GATE_API
#define GATE_API
#endif

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

#ifndef LOG_DEFAULT_SIZE
#define LOG_DEFAULT_SIZE PAGE_SIZE
#endif

#ifndef IPV4_MAX_LENGTH
#define IPV4_MAX_LENGTH	15
#endif

#ifndef MINI_BUF_SIZE
#define MINI_BUF_SIZE	32
#endif

#ifndef DEFAULT_SLEEP
#define DEFAULT_SLEEP	10
#endif

#ifndef UTIL_ERROR
#define UTIL_ERROR -1
#endif

#ifndef UTIL_OK
#define UTIL_OK	0
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef MEGABYTE
#define MEGABYTE			(1024 * 1024)
#endif

#ifndef MAX_SENDBUF_SIZE
#define MAX_SENDBUF_SIZE	(256 * MEGABYTE)
#endif

#ifndef MAX_SESSION
#define MAX_SESSION			4096
#endif

#ifndef DEFAULT_SIZE
#define DEFAULT_SIZE		1024
#endif

#ifndef RCVBUF_SIZE
#define RCVBUF_SIZE			8192
#endif

#ifndef SNDBUF_SIZE
#define SNDBUF_SIZE			8192
#endif

#ifndef SQL_DEFAULT_SIZE
#define SQL_DEFAULT_SIZE	PAGE_SIZE
#endif

#ifndef IPV_LENGTH
#define IPV_LENGTH			16
#endif

#ifndef SD_RECEIVE
#define SD_RECEIVE 			0
#endif

#ifndef SD_SEND
#define SD_SEND 			1
#endif

#ifndef SD_BOTH
#define SD_BOTH 			2
#endif

#ifndef HEVENT_START
#define HEVENT_START		0x01
#endif

#ifndef HEVENT_CLOSE
#define HEVENT_CLOSE		0x02
#endif

#ifndef HEVENT_READ
#define HEVENT_READ			0x01
#endif

#ifndef HEVENT_WRITE
#define HEVENT_WRITE		0x02
#endif

#ifndef HEVENT_EXCEPT
#define HEVENT_EXCEPT		0x04
#endif

#ifndef DEFAULT_TIMEOUT
#define DEFAULT_TIMEOUT		5000
#endif

#ifdef PLATFORM_WINDOWS
#define INLINE		__forceinline
#define FASTCALL	__fastcall
#define RESTRICT	__restrict
#else
#define INLINE		__inline__ __attribute__ ((always_inline))
#define FASTCALL
#define RESTRICT	__restrict
#endif

/************************************************************************/
/* 简易函数定义															*/
/************************************************************************/
#define DEF_DELETE(p)			{ if(p){ delete (p);     (p) = 0; } }
#define DEF_DELETE_ARRAY(p)		{ if(p){ delete[] (p);   (p) = 0; } }
#define DEF_RELEASE(p)			{ if(p){ (p)->Release(); (p) = 0; } }

#define MAKE_UINT64(h_32,l_32)	( ((UInt64)(h_32) << 32) | (l_32) )
#define HIGH_U32(u_64)			( ((u_64) >> 32) & 0xFFFFFFFF )

/************************************************************************/
/* 管理器类定义宏                                                        */
/************************************************************************/
//单例类申明
#define SINGLETON_DECL(name)						\
	protected:										\
	static C##name* m_p##name;						\
	public:											\
	static Bool InitInstance();						\
	static C##name* GetInstance();					\
	static Bool ExitInstance()

//单例类定义
#define SINGLETON_IMPL(name)						\
	C##name* C##name::m_p##name = 0;				\
	Bool C##name::InitInstance()					\
{													\
	if(!m_p##name)									\
{													\
	m_p##name = new C##name;						\
	return true;									\
}													\
	return false;									\
}													\
	C##name* C##name::GetInstance()					\
{													\
	A_Exception(m_p##name);							\
	return m_p##name;								\
}													\
	Bool C##name::ExitInstance()					\
{													\
	if(m_p##name)									\
{													\
	DEF_DELETE(m_p##name);							\
	return true;									\
}													\
	return false;									\
}

/************************************************************************/
/* 属性定义宏                                                           */
/************************************************************************/
//类属性字段申明和Get,Set操作定义
#define DEFINE_PROPERTY(var_type,var,func_suffix)			\
	protected:												\
	var_type var;											\
	public:													\
	inline const var_type& Get##func_suffix() const			\
		{													\
		return var;											\
		};													\
		inline void Set##func_suffix(const var_type& value)	\
		{													\
		var = value;										\
		}

//类属性字段申明和Get,Set操作定义
#define DEFINE_PUBLIC_PROPERTY(var_type,var,func_suffix)	\
	public:													\
	var_type var;											\
	public:													\
	inline const var_type& Get##func_suffix() const			\
		{													\
		return var;											\
		};													\
		inline void Set##func_suffix(const var_type& value)	\
		{													\
		var = value;										\
		}

/************************************************************************/
/* MyProject常使用的的基础类型定义                                      */
/************************************************************************/
namespace Util
{
	typedef bool Bool;
	typedef char Char;
	typedef unsigned char UChar;
	typedef unsigned char Utf8;
	typedef unsigned char byte;
	typedef short Short;
	typedef unsigned short UShort;
	typedef wchar_t WChar;
#ifdef PLATFORM_WINDOWS
	typedef __int8 Int8;
	typedef unsigned __int8	UInt8;
	typedef __int16 Int16;
	typedef unsigned __int16 UInt16;
	typedef __int32 Int32;
	typedef unsigned __int32 UInt32;
	typedef __int64	Int64;
	typedef unsigned __int64 UInt64;	
#else
	typedef int8_t Int8;
	typedef u_int8_t UInt8;
	typedef int16_t	Int16;
	typedef u_int16_t UInt16;
	typedef int32_t	Int32;
	typedef u_int32_t UInt32;
	typedef int64_t	Int64;
	typedef u_int64_t UInt64;
#endif	
	typedef float Float;
	typedef double Double;
	typedef size_t Size_t;
	typedef long Long;
	typedef unsigned long ULong;
	typedef long long LongLong;
	typedef unsigned long long ULongLong;
	typedef void* PVoid;
	typedef UInt32					SID;
	typedef UInt16					ProtoType;
	typedef UInt16					ProtoSize;
	typedef UInt32					ProtoCrc;
	typedef basic_string<Char>		AString;
	typedef basic_string<Utf8>		UString;
	typedef basic_string<WChar>		WString;
	typedef vector<AString>			AStringVector;
	typedef vector<UString>			UStringVector;
	typedef vector<WString>			WStringVector;
	typedef map<AString,AString>	AStringMap;	
	typedef map<UString,UString>	UStringMap;		
	typedef map<WString,WString>	WStringMap;
}

#endif //_BASE_HEADER_