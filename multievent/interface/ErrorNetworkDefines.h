/**
* 自定义的基本网络错误类型的定义
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
	/* socket发生错误 */
	ME_ERROR_NETWORK_SOCKET_ERROR					= (int)(ME_ERROR_NETWORK + 1),

	/* 对方断开一个已有连接 */
	ME_ERROR_NETWORK_SOCKET_RESET					= (int)(ME_ERROR_NETWORK + 2),

	/* 一个已有连接被关闭 */
	ME_ERROR_NETWORK_SOCKET_CLOSE					= (int)(ME_ERROR_NETWORK + 3),

	/* 只有部分数据完成操作 */
	ME_ERROR_NETWORK_PARTIAL_DATA					= (int)(ME_ERROR_NETWORK + 4),

	/* 绑定地址或端口时出错 */
	ME_ERROR_NETWORK_SOCKET_BIND_ERROR				= (int)(ME_ERROR_NETWORK + 5),

	/* 连接对端时出错 */
	ME_ERROR_NETWORK_CONNECT_ERROR					= (int)(ME_ERROR_NETWORK + 6),

	/* 连接超时 */
	ME_ERROR_NETWORK_CONNECT_TIMEOUT				= (int)(ME_ERROR_NETWORK + 7),

	/* DNS解析失败 */
	ME_ERROR_NETWORK_DNS_FAILURE					= (int)(ME_ERROR_NETWORK + 8),

	/* 代理服务器无法访问 */
	ME_ERROR_NETWORK_PROXY_SERVER_UNAVAILABLE		= (int)(ME_ERROR_NETWORK + 9),

	/* 不知名的错误 */
	ME_ERROR_NETWORK_UNKNOWN_ERROR					= (int)(ME_ERROR_NETWORK + 10),

	/* 没有这个服务 */
	ME_ERROR_NETWORK_NO_SERVICE			     		= (int)(ME_ERROR_NETWORK + 11),
};

#endif	// __ERROR_NETWORK_DEFINES_H_





