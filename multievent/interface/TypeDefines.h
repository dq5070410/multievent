/**
* define base data type
* 
* TypeDefines.h
* 
* Author
*
*
* History
*	2016/11/30		Created
*/

#ifndef __TYPE_DEFINES_H_
#define __TYPE_DEFINES_H_

#include "Platform.h"
#include "CommonInclude.h"

/*
* winxp以下版本常用数据类型定义有些细小的差别，
* 由于能力问题，或者说人比较懒，为了省事，目前，
* 就先不支持了
*/
#if ( defined(HIK_WIN) && (!defined(_WIN32_WINNT) || (_WIN32_WINNT <= 0x0501)) )
	#error 在win系统操作系统上, HikUtility只支持win xp或以上版本
#endif	// ( !defined(_WIN32_WINNT) && !(_WIN32_WINNT >= 0x0501) )

/* HikUtility使用的返回值类型定义 */
typedef int HikResult;

/*
 * linux下，基本数据类型的定义
 * 
 * 还是觉得windows的命名风格更合适一些
 * 所以以下，把数据类型的定义按照win的风格
 * 重新定义
 */
#ifdef HIK_LINUX
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
#endif	// HIK_LINUX

#ifdef HIK_WIN
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

#elif defined( HIK_LINUX )
	#define EWOULDBLOCK				EAGAIN
	#ifndef SOCKET_ERROR
		#define SOCKET_ERROR		-1
	#endif	// SOCKET_ERROR

#endif	// SC_WIN

/**
* 无论win32还是linux，统一用iovec存储数据
* 结构名以及成员变量名保持与linux一致，
* 但由于为了与win32的WSABUF结构保持一致
* (在使用WSA系列函数读取数据时，必须这么做)，
* 成员变量命名顺序必须是iov_len首先声明
*/
#ifdef HIK_WIN
	struct iovec
	{
		DWORD iov_len;	
		char* iov_base;
	};

	#define HIK_IOV_MAX 64

#elif defined( HIK_LINUX )
	#ifndef IOV_MAX
		#define IOV_MAX 16
	#endif	// IOV_MAX
	#define HIK_IOV_MAX IOV_MAX
#endif	// HIK_WIN

/**
* 句柄相关
*/
#ifdef HIK_WIN
	typedef HANDLE HIK_HANDLE;
	typedef SOCKET HIK_SOCKET;
	typedef int HIK_SOCK_LEN;

	#define HIK_INVALID_HANDLE		INVALID_HANDLE_VALUE
	#define HIK_SD_RECEIVE			SD_RECEIVE
	#define HIK_SD_SEND				SD_SEND
	#define HIK_SD_BOTH				SD_BOTH

	#define HIK_INFINITE			INFINITE

#elif defined( HIK_LINUX )
	typedef int HIK_HANDLE;
	typedef int HIK_SOCKET;
	typedef socklen_t HIK_SOCK_LEN;

	#define HIK_INVALID_HANDLE		-1
	#define HIK_SD_RECEIVE			0
	#define HIK_SD_SEND				1
	#define HIK_SD_BOTH				2

	#define HIK_INFINITE			-1
#endif	// HIK_WIN

#ifdef HIK_SUPPORT_IPV6
	typedef sockaddr_in6 HIK_SOCK_ADDR;
#else
	typedef sockaddr_in HIK_SOCK_ADDR;
#endif	// HIK_SUPPORT_IPV6

/* 2013.9.16 added by 韦珂 */
/**
* 异步IO操作结果
* 在win32环境下为OVERLAPPED，linux环境为重新定义
*/
#ifdef HIK_WIN
	typedef OVERLAPPED HIK_OVERLAPPED;

#elif defined( HIK_LINUX )
	/* 直接从winbase.h中拷贝 */
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

		HIK_HANDLE  hEvent;
	}HIK_OVERLAPPED;

#endif	// HIK_WIN

/**
* inline的重定义
* 在HikPlatform.h当中已经定义了vc 2005和gcc 4.4以上版本才能
* 编译HikUtility，与inline相关的东西应该能够编译通过，但是，
* 为了避免某种不可预知的原因还是会让inline的东西编译不通过，
* 才搞了底下这个宏 -- 一旦发生这种事情，果断的定义HIK_NO_INLINE吧
*/
#ifndef HIK_NO_INLINE
	#ifndef HIK_INLINE
		#define HIK_INLINE inline
	#endif	// HIK_INLINE
#else	// // HIK_INLINE
	#define HIK_INLINE
#endif	// HIK_IN_LINE

/**
* 函数调用约定的重定义
*/
#ifndef HIK_STDCALL
	#ifdef HIK_WIN
		#define HIK_STDCALL __stdcall
	#elif defined( HIK_LINUX )
		#define HIK_STDCALL
	#endif	// HIK_WIN
#endif	// HIK_STDCALL

/**
* 时间存储结构
*/
typedef struct tagTimeValue
{
	LONGLONG llSecond;	// 秒
	LONG lMicrosecond;	// 微秒
}HIK_TIME_VALUE;

#ifndef HIK_TIMER_NO_LIMIT
	#define HIK_TIMER_LIMIT 1
#else
	#define HIK_TIMER_LIMIT 0
#endif	// HIK_TIMER_NO_LIMIT

/**
* 线程、锁相关定义
*/
#ifdef HIK_WIN
	/* 线程ID */
	typedef UINT HIK_THREAD_ID;

	/* 线程句柄 */
	typedef HIK_HANDLE HIK_THREAD_HANDLE;

	/* 信号灯 */
	typedef HIK_HANDLE HIK_SEMAPHONE;

	/* 互斥量 */
	typedef HIK_HANDLE HIK_MUTEX;

	/* 事件 */
	typedef HIK_HANDLE HIK_EVENT;

	/* 关键代码段 */
	typedef CRITICAL_SECTION HIK_THREAD_MUTEX;

	/* win32版的条件变量，先留着，暂不实现，只为编译通过 */
	typedef struct tagConditionVariable
	{
		LONG lWaiters;
		HIK_THREAD_MUTEX htmMutexWaiter;	// 用于对<lWaiters>操作时的保护锁
		HIK_SEMAPHONE hsSemaphoreWaiter;	// 记录有多少个线程在等待条件变量有信号
		HIK_EVENT heEventWaiterDone;		// 用于，在当前线程把条件变量置为有信号或broadcast之后，通知其他等待的线程
		BOOL bBroadcast;					// 条件变量是否处于broadcast状态
	} HIK_CONDITION_VARIABLE;

	/* 线程状态类型 */
	typedef DWORD HIK_THREAD_STAT;
	
	/* 线程函数返回值类型 */
	typedef UINT HIK_THREAD_RETURN_TYPE;

#elif defined( HIK_LINUX )
	/* 线程ID */
	typedef pthread_t HIK_THREAD_ID;

	/* 线程句柄 */
	typedef HIK_THREAD_ID HIK_THREAD_HANDLE;

	/* 信号灯 */
	typedef sem_t* HIK_SEMAPHONE;

	/* 事件 */
	typedef HIK_HANDLE HIK_EVENT;

	/* 互斥量 */
	typedef pthread_mutex_t HIK_THREAD_MUTEX;

	/* 互斥量 */
	typedef pthread_mutex_t HIK_MUTEX;
	
	/* 条件变量 */
	typedef pthread_cond_t HIK_CONDITION_VARIABLE;

	/* 线程状态类型 */
	typedef void* HIK_THREAD_STAT;

	/* 线程函数返回值类型 */
	typedef void* HIK_THREAD_RETURN_TYPE;

#endif	// HIK_WIN

typedef struct tagHikMutex
{
	enum
	{
		TYPE_PROCESS,	// 进程间同步类型
		TYPE_THREAD,	// 进程内(即线程)同步类型
	};

	INT iType;

	union
	{
		HIK_THREAD_MUTEX htmLock;	// win下是CRITICAL_SECTION，linux下是mutex
		HIK_MUTEX hmLock;			// win和linux下都是mutex
	};
} HIK_MUTEX_CS;

/* 线程函数类型 */
typedef HIK_THREAD_RETURN_TYPE (HIK_STDCALL *HIK_THREAD_FUNC)(void *);

typedef std::string CHikString;

#endif	// __HIK_TYPE_DEFINES_H_


