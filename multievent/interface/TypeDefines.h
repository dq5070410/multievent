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

#if ( defined(WIN) && (!defined(_WIN32_WINNT) || (_WIN32_WINNT <= 0x0501)) )
	#error only support winxp and above
#endif	// ( !defined(_WIN32_WINNT) && !(_WIN32_WINNT >= 0x0501) )

/* multievent return value */
typedef int Result;

/*
 * linux base data
 * 
 */
#ifdef LINUX

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
#endif	// LINUX

#ifdef WIN


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

#elif defined( LINUX )
	#define EWOULDBLOCK				EAGAIN
	#ifndef SOCKET_ERROR
		#define SOCKET_ERROR		-1
	#endif	// SOCKET_ERROR

#endif	// SC_WIN

	struct iovec
	{
		DWORD iov_len;	
		char* iov_base;
	};

	#define IOV_MAX 64

#elif defined( LINUX )
	#ifndef IOV_MAX
		#define IOV_MAX 16
	#endif	// IOV_MAX
	#define IOV_MAX IOV_MAX
#endif	// WIN

/**
* handle
*/
#ifdef WIN
	typedef HANDLE ME_HANDLE;
	typedef SOCKET ME_SOCKET;
	typedef int ME_SOCK_LEN;
	#define ME_INVALID_HANDLE		INVALID_HANDLE_VALUE
	#define ME_SD_RECEIVE			SD_RECEIVE
	#define ME_SD_SEND				SD_SEND
	#define ME_SD_BOTH				SD_BOTH

	#define ME_INFINITE			INFINITE

#elif defined( LINUX )
	typedef int ME_HANDLE;
	typedef int ME_SOCKET;
	typedef socklen_t ME_SOCK_LEN;

	#define ME_INVALID_HANDLE		-1
	#define ME_SD_RECEIVE			0
	#define ME_SD_SEND				1
	#define ME_SD_BOTH				2

	#define ME_INFINITE			-1
#endif	// WIN

#ifdef SUPPORT_IPV6
	typedef sockaddr_in6 ME_SOCK_ADDR;
#else
	typedef sockaddr_in ME_SOCK_ADDR;
#endif	// SUPPORT_IPV6


#ifdef WIN
	typedef OVERLAPPED ME_OVERLAPPED;

#elif defined( LINUX )
	/*  copy from winbase.h*/

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




#ifndef _NO_INLINE
	#ifndef _INLINE
		#define _INLINE inline
	#endif	// _INLINE
#else	// // _INLINE
	#define _INLINE
#endif	// _IN_LINE

/**
* ��������Լ�����ض���
*/
#ifndef C_STDCALL
	#ifdef WIN
		#define C_STDCALL __stdcall
	#elif defined( LINUX )
		#define C_STDCALL
	#endif	// WIN
#endif	// C_STDCALL


/**
* ʱ��洢�ṹ
*/
typedef struct tagTimeValue
{
	LONGLONG llSecond;	// ��
	LONG lMicrosecond;	// ΢��
}C_TIME_VALUE;

#ifndef C_TIMER_NO_LIMIT
	#define C_TIMER_LIMIT 1
#else
	#define C_TIMER_LIMIT 0
#endif	// C_TIMER_NO_LIMIT
}C_TIME_VALUE;


/**
* �̡߳�����ض���
*/
#ifdef WIN
	/* thread ID */
	typedef UINT THREAD_ID;

	/* thread handle */
	typedef C_HANDLE C_THREAD_HANDLE;

	/* SEMAPHONE */
	typedef C_HANDLE C_SEMAPHONE;

	/* MUTEX */
	typedef C_HANDLE C_MUTEX;

	/* EVENT */
	typedef C_HANDLE C_EVENT;

	/* CS */
	typedef CRITICAL_SECTION C_THREAD_MUTEX;

	/* win32 condition variable */
	typedef struct tagConditionVariable
	{
		LONG lWaiters;
		C_THREAD_MUTEX htmMutexWaiter;	// ���ڶ�<lWaiters>����ʱ�ı�����
		C_SEMAPHONE hsSemaphoreWaiter;	// ��¼�ж��ٸ��߳��ڵȴ������������ź�
		C_EVENT heEventWaiterDone;		// ���ڣ��ڵ�ǰ�̰߳�����������Ϊ���źŻ�broadcast֮��֪ͨ�����ȴ����߳�
		BOOL bBroadcast;					// ���������Ƿ���broadcast״̬
	} HIK_CONDITION_VARIABLE;

	/* �߳�״̬���� */
	typedef DWORD C_THREAD_STAT;
	
	/* �̺߳�������ֵ���� */
	typedef UINT C_THREAD_RETURN_TYPE;

#elif defined( LINUX )
	/* �߳�ID */
	typedef pthread_t C_THREAD_ID;

	/* �߳̾�� */
	typedef HIK_THREAD_ID C_THREAD_HANDLE;

	/* �źŵ� */
	typedef sem_t* C_SEMAPHONE;

	/* �¼� */
	typedef HIK_HANDLE C_EVENT;

	/* ������ */
	typedef pthread_mutex_t C_THREAD_MUTEX;

	/* ������ */
	typedef pthread_mutex_t C_MUTEX;
	
	/* �������� */
	typedef pthread_cond_t C_CONDITION_VARIABLE;

	/* �߳�״̬���� */
	typedef void* C_THREAD_STAT;

	/* �̺߳�������ֵ���� */
	typedef void* C_THREAD_RETURN_TYPE;

#endif	// C_WIN

typedef struct tagMutex

{
	enum
	{
		TYPE_PROCESS,	// ���̼�ͬ������
		TYPE_THREAD,	// ������(���߳�)ͬ������
	};

	INT iType;

	union
	{
		C_THREAD_MUTEX htmLock;	// win����CRITICAL_SECTION��linux����mutex
		C_MUTEX hmLock;			// win��linux�¶���mutex
	};
} C_MUTEX_CS;

/* �̺߳������� */
typedef C_THREAD_RETURN_TYPE (C_STDCALL *C_THREAD_FUNC)(void *);

typedef std::string String;

#endif	// __TYPE_DEFINES_H_

