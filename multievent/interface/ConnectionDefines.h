/**
* ��������ص�һЩ����
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
* �Ƿ�����naggle�㷨
*
* 1�����Σ�0������
*/
#ifndef ME_SOCKET_ENABLE_NAGGLE
	const int ME_SOCKET_NAGGLE = 1;
#else
	const int ME_SOCKET_NAGGLE = 0;
#endif	// ME_SOCKET_NO_NAGGLE

/* ��IPv4�� */
#ifndef ME_SUPPORT_IPV6
	/* IPv4 IPͷ����Ϊ20 */
	const DWORD ME_IP_HEADER_LENGTH = 20;

	/* IPv4 UDPͷ����Ϊ8 */
	const DWORD ME_UDP_HEADER_LENGTH = 8;
#endif	// ME_SUPPORT_IPV6

const DWORD ME_CONNECTION_SEND_BUF_SIZE = 64 * 1024;	// 64K
const DWORD ME_CONNECTION_RECV_BUF_SIZE = 192 * 1024;	// 192K��recv buff������Ϊ�����ܱ�����ƴ��

const DWORD ME_CONNECTION_READ_MAX_SIZE = 192 * 1024;	// 192K��һ��read��������ȡ��������

const DWORD ME_CONNECTION_UDP_SEND_MAX_SIZE = ME_CONNECTION_SEND_BUF_SIZE - ME_IP_HEADER_LENGTH - ME_UDP_HEADER_LENGTH;

const DWORD ME_WFMO_MAX_WAIT_NUM = 64;					// 64��ʹ��WaitForMultipleObjects����ܹ��ȴ��Ķ�������
const DWORD ME_SELECT_MAX_WAIT_NUM = FD_SETSIZE;		// 2048��ʹ��select����ܹ��ȴ��Ķ�������

/**
* ����������ش�����������Ͷ���
* ���е������������Ͷ������������
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

