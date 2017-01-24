/**
* define base data type
* 
* TypeDefines.h
* 
* Author
*
*
* History
*	1/30/2016		Created
*/

#ifndef __TYPE_DEFINES_H_
#define __TYPE_DEFINES_H_

#include "Platform.h"
#include "CommonInclude.h"

#if ( defined(ME_WIN) && (!defined(_ME_WIN32_ME_WINNT) || (_ME_WIN32_ME_WINNT <= 0x0501)) )
	#error only support ME_WINxp and above
#endif	// ( !defined(_ME_WIN32_ME_WINNT) && !(_ME_WIN32_ME_WINNT >= 0x0501) )

/* multievent return value */
typedef int ME_Result;

/*
 * ME_LINUX base data
 * 
 */
#ifdef ME_LINUX

	typedef long long				LONGLONG;
	typedef unsigned long			ULONG;
	typedef unsigned long long		ULONGLONG;
	typedef short					SHORT;
	typedef double					DOUBLE;
	//typedef unsigned long			DWORD;
	typedef long					LONG;
	typedef long int				LONGINT;
	typedef unsigned long int		ULONGINT;
	typedef int						BOOL;
	typedef unsigned char			BYTE;
	typedef unsigned short			WORD;
	typedef float					FLOAT;
	typedef int						INT;
	typedef unsigned int			UINT;
	typedef UINT					DWORD;
	typedef FLOAT*					PFLOAT;
	typedef BOOL*					LPBOOL;
	typedef int*					LPINT;
	typedef WORD*					LPWORD;
	typedef long*					LPLONG;
	typedef DWORD*					LPDWORD;
	typedef unsigned int*			LPUINT;
	typedef void					VOID;
	typedef void*					LPVOID;
	typedef const void*				LPCVOID;
	typedef char					CHAR;
	typedef char					TCHAR;
	typedef unsigned short			WCHAR;
	typedef const char*				LPCSTR;
	typedef char*					LPSTR;
	typedef const unsigned short*	LPCWSTR;
	typedef unsigned short*			LPWSTR;
	typedef BYTE*					LPBYTE;
	typedef const BYTE*				LPCBYTE;

	#ifndef FALSE
		#define FALSE 0
	#endif // FALSE

	#ifndef TRUE
		#define TRUE 1
	#endif // TRUE
#endif	// ME_LINUX

#ifdef ME_WIN


	#define EWOULDBLOCK             WSAEWOULDBLOCK
	#define EINPROGRESS             WSAEINPROGRESS
	#define EALREADY                WSAEALREADY
	#define ENOTSOCK                WSAENOTSOCK
	#define EDESTADDRREQ            WSAEDESTADDRREQ
	#define EMSGSIZE                WSAEMSGSIZE
	#define EPROTOTYPE              WSAEPROTOTYPE
	#define ENOPROTOOPT             WSAENOPROTOOPT
	#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
	#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
	#define EOPNOTSUPP              WSAEOPNOTSUPP
	#define EPFNOSUPPORT            WSAEPFNOSUPPORT
	#define EAFNOSUPPORT            WSAEAFNOSUPPORT
	#define EADDRINUSE              WSAEADDRINUSE
	#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
	#define ENETDOWN                WSAENETDOWN
	#define ENETUNREACH             WSAENETUNREACH
	#define ENETRESET               WSAENETRESET
	#define ECONNABORTED            WSAECONNABORTED
	#define ECONNRESET              WSAECONNRESET
	#define ENOBUFS                 WSAENOBUFS
	#define EISCONN                 WSAEISCONN
	#define ENOTCONN                WSAENOTCONN
	#define ESHUTDOWN               WSAESHUTDOWN
	#define ETOOMANYREFS            WSAETOOMANYREFS
	#define ETIMEDOUT               WSAETIMEDOUT
	#define ECONNREFUSED            WSAECONNREFUSED
	#define ELOOP                   WSAELOOP
	#define EHOSTDOWN               WSAEHOSTDOWN
	#define EHOSTUNREACH            WSAEHOSTUNREACH
	#define EPROCLIM                WSAEPROCLIM
	#define EUSERS                  WSAEUSERS
	#define EDQUOT                  WSAEDQUOT
	#define ESTALE                  WSAESTALE
	#define EREMOTE                 WSAEREMOTE

#elif defined( ME_LINUX )
	#define EWOULDBLOCK				EAGAIN
	#ifndef SOCKET_ERROR
		#define SOCKET_ERROR		-1
	#endif	// SOCKET_ERROR

#endif	// SC_ME_WIN

/**
* 无论win32还是linux，统一用iovec存储数据
* 结构名以及成员变量名保持与linux一致，
* 但由于为了与win32的WSABUF结构保持一致
* (在使用WSA系列函数读取数据时，必须这么做)，
* 成员变量命名顺序必须是iov_len首先声明
*/
#ifdef ME_WIN
	struct iovec
	{
		DWORD iov_len;	
		char* iov_base;
	};

	#define ME_IOV_MAX 64

#elif defined( ME_LINUX )
	#ifndef IOV_MAX
		#define IOV_MAX 16
	#endif	// IOV_MAX
	#define ME_IOV_MAX IOV_MAX
#endif	// ME_WIN

/**
* handle
*/
#ifdef ME_WIN
	typedef HANDLE ME_HANDLE;
	typedef SOCKET ME_SOCKET;
	typedef int ME_SOCK_LEN;
	#define ME_INVALID_HANDLE		INVALID_HANDLE_VALUE
	#define ME_SD_RECEIVE			SD_RECEIVE
	#define ME_SD_SEND				SD_SEND
	#define ME_SD_BOTH				SD_BOTH

	#define ME_INFINITE			INFINITE

#elif defined( ME_LINUX )
	typedef int ME_HANDLE;
	typedef int ME_SOCKET;
	typedef socklen_t ME_SOCK_LEN;

	#define ME_INVALID_HANDLE		-1
	#define ME_SD_RECEIVE			0
	#define ME_SD_SEND				1
	#define ME_SD_BOTH				2

	#define ME_INFINITE			-1
#endif	// ME_WIN

#ifdef SUPPORT_IPV6
	typedef sockaddr_in6 ME_SOCK_ADDR;
#else
	typedef sockaddr_in ME_SOCK_ADDR;
#endif	// SUPPORT_IPV6


#ifdef ME_WIN
	typedef OVERLAPPED ME_OVERLAPPED;

#elif defined( ME_LINUX )
	/*  copy from ME_WINbase.h*/

	typedef struct tagOVERLAPPED 
	{
		volatile ULONG Internal;
		volatile ULONG InternalHigh;
		union 
		{
			struct 
			{
				DWORD Offset;
				DWORD OffsetHigh;
			} DUMMYSTRUCTNAME;
			void* Pointer;
		} DUMMYUNIONNAME;

		ME_HANDLE  hEvent;
	}ME_OVERLAPPED;

#endif


#ifndef _NO_INLINE
	#ifndef ME_INLINE
		#define ME_INLINE inline
	#endif	// _INLINE
#else	// // _INLINE
	#define ME_INLINE
#endif	// _IN_LINE

/**
* 函数调用约定的重定义
*/
#ifndef ME_STDCALL
	#ifdef ME_WIN
		#define ME_STDCALL __stdcall
	#elif defined( ME_LINUX )
		#define ME_STDCALL
	#endif	// ME_WIN
#endif	// ME_STDCALL


/**
* 时间存储结构
*/
typedef struct tagTimeValue
{
	LONGLONG llSecond;	// 秒
	LONG lMicrosecond;	// 微秒
}ME_TIME_VALUE;

#ifndef ME_TIMER_NO_LIMIT
	#define ME_TIMER_LIMIT 1
#else
	#define ME_TIMER_LIMIT 0
#endif	// C_TIMER_NO_LIMIT


/**
* 线程、锁相关定义
*/
#ifdef ME_WIN
	/* thread ID */
	typedef UINT THREAD_ID;

	/* thread handle */
	typedef C_HANDLE ME_THREAD_HANDLE;

	/* SEMAPHONE */
	typedef C_HANDLE ME_SEMAPHONE;

	/* MUTEX */
	typedef C_HANDLE ME_MUTEX;

	/* EVENT */
	typedef C_HANDLE ME_EVENT;

	/* CS */
	typedef CRITICAL_SECTION ME_THREAD_MUTEX;

	/* ME_WIN32 condition variable */
	typedef struct tagConditionVariable
	{
		LONG lWaiters;
		C_THREAD_MUTEX htmMutexWaiter;	// 用于对<lWaiters>操作时的保护锁
		C_SEMAPHONE hsSemaphoreWaiter;	// 记录有多少个线程在等待条件变量有信号
		C_EVENT heEventWaiterDone;		// 用于，在当前线程把条件变量置为有信号或broadcast之后，通知其他等待的线程
		BOOL bBroadcast;					// 条件变量是否处于broadcast状态
	} ME_CONDITION_VARIABLE;

	/* 线程状态类型 */
	typedef DWORD ME_THREAD_STAT;
	
	/* 线程函数返回值类型 */
	typedef UINT ME_THREAD_RETURN_TYPE;

#elif defined( ME_LINUX )
	/* 线程ID */
	typedef pthread_t ME_THREAD_ID;

	/* 线程句柄 */
	typedef ME_THREAD_ID ME_THREAD_HANDLE;

	/* 信号灯 */
	typedef sem_t* ME_SEMAPHONE;

	/* 事件 */
	typedef ME_HANDLE ME_EVENT;

	/* 互斥量 */
	typedef pthread_mutex_t ME_THREAD_MUTEX;

	/* 互斥量 */
	typedef pthread_mutex_t ME_MUTEX;
	
	/* 条件变量 */
	typedef pthread_cond_t ME_CONDITION_VARIABLE;

	/* 线程状态类型 */
	typedef void* ME_THREAD_STAT;

	/* 线程函数返回值类型 */
	typedef void* ME_THREAD_RETURN_TYPE;

#endif	// C_ME_WIN

typedef struct tagMutex

{
	enum
	{
		TYPE_PROCESS,	// 进程间同步类型
		TYPE_THREAD,	// 进程内(即线程)同步类型
	};

	INT iType;

	union
	{
		ME_THREAD_MUTEX htmLock;	// ME_WIN下是CRITICAL_SECTION，ME_LINUX下是mutex
		ME_MUTEX hmLock;			// ME_WIN和ME_LINUX下都是mutex
	};
} ME_MUTEX_CS;

/* 线程函数类型 */
typedef ME_THREAD_RETURN_TYPE (ME_STDCALL *ME_THREAD_FUNC)(void *);

typedef std::string CMEString;

#endif	// __TYPE_DEFINES_H_

