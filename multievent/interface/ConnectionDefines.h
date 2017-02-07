/**
* 与连接相关的一些定义
* 
*
* History
*	12/14/2016		Created
*/

#ifndef __CONNECTION_DEFINES_H_
#define __CONNECTION_DEFINES_H_

#include "Base.h"
#include "LockMutex.h"

ME_NAME_SPACE_BEGIN

/**
* 是否屏蔽naggle算法
*
* 1：屏蔽；0：开启
*/
#ifndef ME_SOCKET_ENABLE_NAGGLE
	const int ME_SOCKET_NAGGLE = 1;
#else
	const int ME_SOCKET_NAGGLE = 0;
#endif	// ME_SOCKET_NO_NAGGLE

/* 在IPv4中 */
#ifndef ME_SUPPORT_IPV6
	/* IPv4 IP头长度为20 */
	const DWORD ME_IP_HEADER_LENGTH = 20;

	/* IPv4 UDP头长度为8 */
	const DWORD ME_UDP_HEADER_LENGTH = 8;
#endif	// ME_SUPPORT_IPV6

const DWORD ME_CONNECTION_SEND_BUF_SIZE = 64 * 1024;	// 64K
const DWORD ME_CONNECTION_RECV_BUF_SIZE = 192 * 1024;	// 192K，recv buff大是因为尽可能避免多次拼包

const DWORD ME_CONNECTION_READ_MAX_SIZE = 192 * 1024;	// 192K，一次read动作最多读取多少数据

const DWORD ME_CONNECTION_UDP_SEND_MAX_SIZE = ME_CONNECTION_SEND_BUF_SIZE - ME_IP_HEADER_LENGTH - ME_UDP_HEADER_LENGTH;

const DWORD ME_WFMO_MAX_WAIT_NUM = 64;					// 64，使用WaitForMultipleObjects最多能够等待的对象数量
const DWORD ME_SELECT_MAX_WAIT_NUM = FD_SETSIZE;		// 2048，使用select最多能够等待的对象数量

/**
* 用在连接相关处理的锁的类型定义
* 所有的相关类的锁类型都引申自这个类
*/
class CMEConnDummy
{
public:
	typedef CMELockMutex MutexType;
};

template <class T>
class CMEMutexTypeTraits
{
public:
	typedef typename T::MutexType MutexType;
};


enum CONNECTION_OPTION_TYPE
{
    CONNECTION_OPTION_TYPE_NONE = 100,

    CONNECTION_OPTION_TYPE_SSL_TYPE,

    CONNECTION_OPTION_TYPE_CERTIFICATE_FILE,

    CONNECTION_OPTION_TYPE_PRIVATEKEY_FILE,

    CONNECTION_OPTION_TYPE_PRIVATEKEY_PASSWORD,
};


ME_NAME_SPACE_END

#endif	// __ME_CONNECTION_DEFINES_H_

