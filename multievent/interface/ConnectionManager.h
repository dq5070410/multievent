/**
* 连接管理的真正实现
* 
*
* History
*	12/14/2016		Created
*/

#ifndef __CONNECTION_MANAGER_H_
#define __CONNECTION_MANAGER_H_

#include "Base.h"
#include "ConnectionInterface.h"
#include "ConnectionDefines.h"

ME_NAME_SPACE_BEGIN

class ME_OS_EXPORT CMEConnectionManager
	: public IConnectionManager
	, public CMEMutexTypeTraits<CMEConnDummy>
{
public:
	static void SocketStartup();

	static void SocketCleanup();

public:
	CMEConnectionManager();

	virtual ~CMEConnectionManager();

public:
	/**
	* 创建一个客户端
	* 
	* <dwType>, 什么协议的客户端, 类型必须从ME_CONNECTION_TYPE中或的方式取值
	* <pConnector>, out, 传入的连接器智能指针对象
	*
	* 返回值:
	*		ME_OK为成功; 其他为失败
	*/
	virtual ME_Result CreateClient(
		DWORD dwType,
		CConnectorAutoPtr& pConnector );

	/**
	* 创建一个服务器端
	* 
	* <dwType>, 什么协议的服务器端, 类型必须从ME_CONNECTION_TYPE中或的方式取值
	* <pAcceptor>, out, 传入的连接器智能指针对象
	*
	* 返回值:
	*		ME_OK为成功; 其他为失败
	*/
	virtual ME_Result CreateServer(
		DWORD dwType,
		CAcceptorAutoPtr& pAcceptor );

protected:

	/**
	* 创建一个客户端
	* 
	* <dwType>, 什么协议的客户端, 类型必须从ME_CONNECTION_TYPE中或的方式取值
	* <pConnector>, out, 传入的连接器智能指针对象
	*
	* 返回值:
	*		ME_OK为成功; 其他为失败
	*/
	ME_Result CreateClientImpl(
		DWORD dwType,
		CConnectorAutoPtr& pConnector );

	/**
	* 创建一个服务器端
	* 
	* <dwType>, 什么协议的服务器端, 类型必须从ME_CONNECTION_TYPE中或的方式取值
	* <pAcceptor>, out, 传入的连接器智能指针对象
	*
	* 返回值:
	*		ME_OK为成功; 其他为失败
	*/
	virtual ME_Result CreateServerImpl(
		DWORD dwType,
		CAcceptorAutoPtr& pAcceptor );

private:

#ifdef ME_WIN32
	/**
	* 通过WSAIoctl函数从Mswsock.lib库中load AcceptEx函数
	*
	* 如果在没有事先获得函数指针的情况下直接调用函数(也就是说，编译时静态连接mswsock.lib，在程序中直接调用函数)，
	* 那么性能将很受影响。因为AcceptEx()被置于Winsock2架构之外，每次调用时它都被迫通过WSAIoctl()取得函数指针。
	* 要避免这种性能损失，需要使用这些API的应用程序应该通过调用WSAIoctl()直接从底层的提供者那里取得函数的指针
	*/
	static void LoadAcceptEx();

	/**
	* 通过WSAIoctl函数从Mswsock.lib库中load ConnectEx函数
	*
	* 如果在没有事先获得函数指针的情况下直接调用函数(也就是说，编译时静态连接mswsock.lib，在程序中直接调用函数)，
	* 那么性能将很受影响。因为ConnectEx()被置于Winsock2架构之外，每次调用时它都被迫通过WSAIoctl()取得函数指针。
	* 要避免这种性能损失，需要使用这些API的应用程序应该通过调用WSAIoctl()直接从底层的提供者那里取得函数的指针
	*/
	static void LoadConnectEx();

	/**
	* 如果用到AcceptEx接受对端连接时，就必须要使用GetAcceptExSockaddrs才能正确获取对端的地址
	*
	* 特别需要注意的是它与AcceptEx的匹配问题，分两种情况
	* 1. 如果是直接使用::AcceptEx()函数，那么只能直接使用GetAcceptExSockaddrs()函数获取对端地址
	* 2. 如果采取获取AcceptEx函数指针的方式，就必须用WSAIoctl()函数load出GetAcceptExSockaddrs函数指针
	*	 并使用这个指针获取对端地址才能保证获取到正确的对端地址
	*/
	static void LoadGetAcceptExSockaddrs();

#endif	// ME_WIN32
};

ME_NAME_SPACE_END


#endif	// __CONNECTION_MANAGER_H_
