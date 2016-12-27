/**
* �Զ���Ļ�������������͵Ķ���
* 
*
* History
*	12/02/2016		Created
*/

#ifndef __ERROR_NETWORK_DEFINES_H_
#define __ERROR_NETWORK_DEFINES_H_

#include "ErrorDefines.h"

enum
{
	/* socket�������� */
	ME_ERROR_NETWORK_SOCKET_ERROR					= (int)(ME_ERROR_NETWORK + 1),

	/* �Է��Ͽ�һ���������� */
	ME_ERROR_NETWORK_SOCKET_RESET					= (int)(ME_ERROR_NETWORK + 2),

	/* һ���������ӱ��ر� */
	ME_ERROR_NETWORK_SOCKET_CLOSE					= (int)(ME_ERROR_NETWORK + 3),

	/* ֻ�в���������ɲ��� */
	ME_ERROR_NETWORK_PARTIAL_DATA					= (int)(ME_ERROR_NETWORK + 4),

	/* �󶨵�ַ��˿�ʱ���� */
	ME_ERROR_NETWORK_SOCKET_BIND_ERROR				= (int)(ME_ERROR_NETWORK + 5),

	/* ���ӶԶ�ʱ���� */
	ME_ERROR_NETWORK_CONNECT_ERROR					= (int)(ME_ERROR_NETWORK + 6),

	/* ���ӳ�ʱ */
	ME_ERROR_NETWORK_CONNECT_TIMEOUT				= (int)(ME_ERROR_NETWORK + 7),

	/* DNS����ʧ�� */
	ME_ERROR_NETWORK_DNS_FAILURE					= (int)(ME_ERROR_NETWORK + 8),

	/* ����������޷����� */
	ME_ERROR_NETWORK_PROXY_SERVER_UNAVAILABLE		= (int)(ME_ERROR_NETWORK + 9),

	/* ��֪���Ĵ��� */
	ME_ERROR_NETWORK_UNKNOWN_ERROR					= (int)(ME_ERROR_NETWORK + 10),

	/* û��������� */
	ME_ERROR_NETWORK_NO_SERVICE			     		= (int)(ME_ERROR_NETWORK + 11),
};

#endif	// __ERROR_NETWORK_DEFINES_H_





