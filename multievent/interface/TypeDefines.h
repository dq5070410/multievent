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
* winxp���°汾�����������Ͷ�����ЩϸС�Ĳ��
* �����������⣬����˵�˱Ƚ�����Ϊ��ʡ�£�Ŀǰ��
* ���Ȳ�֧����
*/
#if ( defined(HIK_WIN) && (!defined(_WIN32_WINNT) || (_WIN32_WINNT <= 0x0501)) )
	#error ��winϵͳ����ϵͳ��, HikUtilityֻ֧��win xp�����ϰ汾
#endif	// ( !defined(_WIN32_WINNT) && !(_WIN32_WINNT >= 0x0501) )

/* HikUtilityʹ�õķ���ֵ���Ͷ��� */
typedef int HikResult;

/*
 * linux�£������������͵Ķ���
 * 
 * ���Ǿ���windows��������������һЩ
 * �������£����������͵Ķ��尴��win�ķ��
 * ���¶���
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
* ����win32����linux��ͳһ��iovec�洢����
* �ṹ���Լ���Ա������������linuxһ�£�
* ������Ϊ����win32��WSABUF�ṹ����һ��
* (��ʹ��WSAϵ�к�����ȡ����ʱ��������ô��)��
* ��Ա��������˳�������iov_len��������
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
* ������
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

/* 2013.9.16 added by Τ�� */
/**
* �첽IO�������
* ��win32������ΪOVERLAPPED��linux����Ϊ���¶���
*/
#ifdef HIK_WIN
	typedef OVERLAPPED HIK_OVERLAPPED;

#elif defined( HIK_LINUX )
	/* ֱ�Ӵ�winbase.h�п��� */
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
* inline���ض���
* ��HikPlatform.h�����Ѿ�������vc 2005��gcc 4.4���ϰ汾����
* ����HikUtility����inline��صĶ���Ӧ���ܹ�����ͨ�������ǣ�
* Ϊ�˱���ĳ�ֲ���Ԥ֪��ԭ���ǻ���inline�Ķ������벻ͨ����
* �Ÿ��˵�������� -- һ�������������飬���ϵĶ���HIK_NO_INLINE��
*/
#ifndef HIK_NO_INLINE
	#ifndef HIK_INLINE
		#define HIK_INLINE inline
	#endif	// HIK_INLINE
#else	// // HIK_INLINE
	#define HIK_INLINE
#endif	// HIK_IN_LINE

/**
* ��������Լ�����ض���
*/
#ifndef HIK_STDCALL
	#ifdef HIK_WIN
		#define HIK_STDCALL __stdcall
	#elif defined( HIK_LINUX )
		#define HIK_STDCALL
	#endif	// HIK_WIN
#endif	// HIK_STDCALL

/**
* ʱ��洢�ṹ
*/
typedef struct tagTimeValue
{
	LONGLONG llSecond;	// ��
	LONG lMicrosecond;	// ΢��
}HIK_TIME_VALUE;

#ifndef HIK_TIMER_NO_LIMIT
	#define HIK_TIMER_LIMIT 1
#else
	#define HIK_TIMER_LIMIT 0
#endif	// HIK_TIMER_NO_LIMIT

/**
* �̡߳�����ض���
*/
#ifdef HIK_WIN
	/* �߳�ID */
	typedef UINT HIK_THREAD_ID;

	/* �߳̾�� */
	typedef HIK_HANDLE HIK_THREAD_HANDLE;

	/* �źŵ� */
	typedef HIK_HANDLE HIK_SEMAPHONE;

	/* ������ */
	typedef HIK_HANDLE HIK_MUTEX;

	/* �¼� */
	typedef HIK_HANDLE HIK_EVENT;

	/* �ؼ������ */
	typedef CRITICAL_SECTION HIK_THREAD_MUTEX;

	/* win32������������������ţ��ݲ�ʵ�֣�ֻΪ����ͨ�� */
	typedef struct tagConditionVariable
	{
		LONG lWaiters;
		HIK_THREAD_MUTEX htmMutexWaiter;	// ���ڶ�<lWaiters>����ʱ�ı�����
		HIK_SEMAPHONE hsSemaphoreWaiter;	// ��¼�ж��ٸ��߳��ڵȴ������������ź�
		HIK_EVENT heEventWaiterDone;		// ���ڣ��ڵ�ǰ�̰߳�����������Ϊ���źŻ�broadcast֮��֪ͨ�����ȴ����߳�
		BOOL bBroadcast;					// ���������Ƿ���broadcast״̬
	} HIK_CONDITION_VARIABLE;

	/* �߳�״̬���� */
	typedef DWORD HIK_THREAD_STAT;
	
	/* �̺߳�������ֵ���� */
	typedef UINT HIK_THREAD_RETURN_TYPE;

#elif defined( HIK_LINUX )
	/* �߳�ID */
	typedef pthread_t HIK_THREAD_ID;

	/* �߳̾�� */
	typedef HIK_THREAD_ID HIK_THREAD_HANDLE;

	/* �źŵ� */
	typedef sem_t* HIK_SEMAPHONE;

	/* �¼� */
	typedef HIK_HANDLE HIK_EVENT;

	/* ������ */
	typedef pthread_mutex_t HIK_THREAD_MUTEX;

	/* ������ */
	typedef pthread_mutex_t HIK_MUTEX;
	
	/* �������� */
	typedef pthread_cond_t HIK_CONDITION_VARIABLE;

	/* �߳�״̬���� */
	typedef void* HIK_THREAD_STAT;

	/* �̺߳�������ֵ���� */
	typedef void* HIK_THREAD_RETURN_TYPE;

#endif	// HIK_WIN

typedef struct tagHikMutex
{
	enum
	{
		TYPE_PROCESS,	// ���̼�ͬ������
		TYPE_THREAD,	// ������(���߳�)ͬ������
	};

	INT iType;

	union
	{
		HIK_THREAD_MUTEX htmLock;	// win����CRITICAL_SECTION��linux����mutex
		HIK_MUTEX hmLock;			// win��linux�¶���mutex
	};
} HIK_MUTEX_CS;

/* �̺߳������� */
typedef HIK_THREAD_RETURN_TYPE (HIK_STDCALL *HIK_THREAD_FUNC)(void *);

typedef std::string CHikString;

#endif	// __HIK_TYPE_DEFINES_H_


