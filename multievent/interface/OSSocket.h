/**
* SOCKET相关操作的API
* 
*
* History
*	12/11/2016		Created
*/

#ifndef __OS_SOCKET_H_
#define __OS_SOCKET_H_

#include "Base.h"

NAME_SPACE_OS_BEGIN

/**
* 打开一个socket并获得句柄
* 
* <iFamily>, 协议族
* <iType>, 类型
* <iProtocol>, 协议
* <bReuseAddr>, 地址是否允许重用
* <Handle>, 返回的socket句柄
*
* 返回值:
*		SOCKET_ERROR是失败; ME_OK是成功
*/
ME_Result ME_OS_EXPORT SocketOpen(
	int iFamily,
	int iType,
	int iProtocol,
	ME_HANDLE& Handle );

/**
* 关闭一个socket
* 
* <Handle>, 需要关闭的socket句柄
*
* 返回值:
*		SOCKET_ERROR是失败; ME_OK是成功
*/
ME_Result ME_OS_EXPORT SocketClose( ME_HANDLE Handle );

/**
* 发送数据
* 
* <Handle>, 在哪个socket上发送数据
* <pszData>, 要发送的数据
* <dwLength>, 要发送数据的长度
* <iFlag>, 标记
*
* 返回值:
*		返回本次调用发送的字节数数量
*/
int ME_OS_EXPORT SocketSend(
	ME_HANDLE Handle,
	const char* pszData,
	DWORD dwLength,
	int iFlag = 0 );

/**
* 发送数据
* 
* <Handle>, 在哪个socket上发送数据
* <pszIov>, 要发送的数据数组
* <dwCount>, 要发送的数据数组的个数
*
* 返回值:
*		返回本次调用发送的字节数数量
*/
int ME_OS_EXPORT SocketSendV(
	ME_HANDLE Handle,
	const iovec pszIov[],
	DWORD dwCount );

/**
* 收取数据
* 
* <Handle>, 在哪个socket上收取数据
* <pszBuf>, 收取数据的缓冲区
* <dwLength>, 收取数据的缓冲区大小
* <iFlag>, 标记
*
* 返回值:
*		返回本次收取的字节数数量
*/
int ME_OS_EXPORT SocketRecv(
	ME_HANDLE Handle,
	char*& pszBuf,
	DWORD dwLength,
	int iFlag = 0 );

/**
* 发送数据
* 
* <Handle>, 在哪个socket上收取数据
* <pszIov>, 收取数据缓冲区数组
* <dwCount>, 收取数据缓冲区数组的个数
*
* 返回值:
*		返回本次收取的字节数数量
*/
int ME_OS_EXPORT SocketRecvV(
	ME_HANDLE Handle,
	iovec szIov[],
	DWORD dwCount );

/**
* 发送数据, 仅用于UDP
* 
* <Handle>, 在哪个socket上发送数据
* <pszData>, 要发送的数据
* <dwLength>, 要发送数据的长度
* <pPeerAddress>, 目标地址
* <iFlag>, 标记
*
* 返回值:
*		返回本次调用发送的字节数数量
*/
int ME_OS_EXPORT SocketSendTo(
	ME_HANDLE Handle,
	const char* pszData,
	DWORD dwLength,
	const ME_SOCK_ADDR* pPeerAddress,
	int iFlag = 0 );

/**
* 发送数据, 仅用于UDP
* 
* <Handle>, 在哪个socket上发送数据
* <pszIov>, 要发送的数据数组
* <dwCount>, 要发送的数据数组的个数
* <pPeerAddress>, 目标地址
*
* 返回值:
*		返回本次调用发送的字节数数量
*/
int ME_OS_EXPORT SocketSendVTo(
	ME_HANDLE Handle,
	const iovec pszIov[],
	DWORD dwCount,
	const ME_SOCK_ADDR* pPeerAddress );

/**
* 收取数据, 仅用于UDP
* 
* <Handle>, 在哪个socket上收取数据
* <pszBuf>, 收取数据的缓冲区
* <dwLength>, 收取数据的缓冲区大小
* <hsaPeerAddress>, 目标地址
* <iFlag>, 标记
*
* 返回值:
*		返回本次收取的字节数数量
*/
int ME_OS_EXPORT SocketRecvFrom(
	ME_HANDLE Handle,
	char*& pszBuf,
	DWORD dwLength,
	ME_SOCK_ADDR& hsaPeerAddress,
	int iFlag = 0 );

/**
* 为一个socket句柄绑定一个本地地址
* 
* <Handle>, socket句柄
* <pLocalAddrYouWantToBind>, 需要绑定的地址
*
* 返回值:
*		SOCKET_ERROR是失败; ME_OK是成功
*/
ME_Result ME_OS_EXPORT SocketBind(
	ME_HANDLE Handle,
	const ME_SOCK_ADDR* pLocalAddrYouWantToBind );

/**
* 屏蔽某个socket的某些功能
* 
* <Handle>, socket句柄
* <iFlag>, 需要屏蔽的功能
*
* 返回值:
*		SOCKET_ERROR是失败; ME_OK是成功
*/
ME_Result ME_OS_EXPORT SocketShutdown(
	ME_HANDLE Handle,
	int iFlag );

/**
* 连接
* 
* <Handle>, socket句柄
* <pPeerAddress>, 要连接到哪里
*
* 返回值:
*		SOCKET_ERROR是失败; ME_OK是成功
*/
ME_Result ME_OS_EXPORT SocketConnect(
	ME_HANDLE Handle,
	const ME_SOCK_ADDR* pPeerAddress );


/**
* 监听
* 
* <Handle>, socket句柄
*
* 返回值:
*		SOCKET_ERROR是失败; ME_OK是成功
*/
ME_Result ME_OS_EXPORT SocketListen( ME_HANDLE Handle );

/**
* 监听
* 
* <Handle>, socket句柄
* <hsaPeerAddress>, 要在什么地址、端口上监听
*
* 返回值:
*		SOCKET_ERROR是失败; ME_OK是成功
*/
ME_HANDLE ME_OS_EXPORT SocketAccept(
	ME_HANDLE Handle,
	ME_SOCK_ADDR& hsaPeerAddress );

/**
* 把一个4字节的DWORD值转换成符合IPV4规范的地址
* 
* <dwIP>, IP地址的DWORD值
*
* 返回值:
*		字符串长度为0是失败; 非ME_OK是成功
*/
CMEString ME_OS_EXPORT IpDigitalToString( DWORD dwIP );

/**
* 把一个符合IPV4规范的地址转换成4字节的DWORD值
* 
* <pszIP>, IP地址
*
* 返回值:
*		0是失败; 非ME_OK是成功
*/
DWORD ME_OS_EXPORT IpStringToDigital( const char* pszIP );

/**
* 设置socket的I/O模式
* 
* <Handle>, 需要设置的socket句柄
* <iFlag>, 标志
* <pArg>, 对应于<iFlag>的值
*
* 返回值:
*		SOCKET_ERROR是失败; ME_OK是成功
*/
ME_Result ME_OS_EXPORT IOCtrl(
	ME_SOCKET Handle,
	int iFlag,
	void* pArg );

/**
* 获取一个已有连接中所绑定的本地地址
* 
* <Handle>, 需要设置的socket句柄
* <hsaLocalAddress>, out, 被绑定的地址
*
* 返回值:
*		SOCKET_ERROR是失败; ME_OK是成功
*/
ME_Result ME_OS_EXPORT SocketGetLocalAddress(
	ME_HANDLE Handle,
	ME_SOCK_ADDR& hsaLocalAddress );

/**
* 获取一个已有连接中所绑定的对端地址
* 
* <Handle>, 需要设置的socket句柄
* <hsaRemoteAddress>, out, 对端地址
*
* 返回值:
*		SOCKET_ERROR是失败; ME_OK是成功
*/
ME_Result ME_OS_EXPORT SocketGetRemoteAddress(
	ME_HANDLE Handle,
	ME_SOCK_ADDR& hsaRemoteAddress );

/**
* 设置socket的属性
* 
* <Handle>, 需要设置的socket句柄
* <iLevel>, 要设置的这个属性被定于哪一个层次
* <iOption>, 要设置的属性
* <pOptval>, 设置<iOption>属性的值
* <iOptlen>, 属性值<pOptval>的长度是多少
*
* 返回值:
*		SOCKET_ERROR是失败; ME_OK是成功
*/
ME_Result ME_OS_EXPORT SocketSetSocketOption(
	ME_HANDLE Handle,
	int iLevel, 
	int iOption, 
	const void* pOptval, 
	int iOptlen );

/**
* 获取socket的属性
* 
* <Handle>, 需要设置的socket句柄
* <iLevel>, 要获取的这个属性被定于哪一个层次
* <iOption>, 要获取的属性
* <pOptval>, 设置<iOption>属性的值
* <iOptlen>, 属性值<pOptval>的长度是多少
*
* 返回值:
*		SOCKET_ERROR是失败; ME_OK是成功
*/
ME_Result ME_OS_EXPORT SocketGetSocketOption(
	ME_HANDLE Handle,
	int iLevel, 
	int iOption, 
	void* pOptval, 
	int* pOptlen );

NAME_SPACE_OS_END


#endif	// __OS_SOCKET_H_



