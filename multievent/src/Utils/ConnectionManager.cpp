#include "ConnectionManager.h"
#include "LockThreadMutex.h"
#include "SingletonT.h"
#include "AcceptorWrapper.h"
#include "ConnectorWrapper.h"
#include "AcceptorCrossThread.h"
#include "ConnectorCrossThread.h"
#include "ThreadManager.h"
#include "Thread.h"
#include "OSThread.h"
#include "SocketTcp.h"
#include "ConnectorOperatorSSL.h"
#include "AcceptorOperatorSSL.h"
#include "Reactor.h"
#include "AsyncIoDefines.h"

ME_NAME_SPACE_BEGIN

const char ME_LOCAL_IP_ADDRESS[] = "127.0.0.1";

#ifdef ME_WIN
	LPFN_ACCEPTEX g_lpfnAcceptEx = NULL;
	LPFN_CONNECTEX g_lpfnConnectEx = NULL;
	LPFN_GETACCEPTEXSOCKADDRS g_lpfnGetAcceptExSockaddrs = NULL;

#endif	// ME_WIN

IConnectionManager* IConnectionManager::Instance()
{
	return CMESingletonT<CMEConnectionManager>::Instance();
}

void CMEConnectionManager::SocketStartup()
{
#ifdef ME_WIN
	WORD wVersionRequested;
	WSADATA wsaData;
	int iError;

	wVersionRequested = MAKEWORD( 
		2, 
		2 );

	iError = ::WSAStartup( 
		wVersionRequested, 
		&wsaData );

	ME_ASSERTE_RETURN_VOID( ME_SUCCEEDED(iError) );

	LoadAcceptEx();
	LoadConnectEx();
	LoadGetAcceptExSockaddrs();

#endif	// ME_WIN
}

void CMEConnectionManager::SocketCleanup()
{
#ifdef ME_WIN
	::WSACleanup();

#endif	// ME_WIN
}

CMEConnectionManager::CMEConnectionManager()
{

}

CMEConnectionManager::~CMEConnectionManager()
{

}

ME_Result CMEConnectionManager::CreateClient( 
	DWORD dwType, 
	CConnectorAutoPtr& pConnector )
{
	/* 必须是TCP或UDP协议 */
	ME_ASSERTE_RETURN(
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_TCP) ||
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_UDP),
		ME_ERROR_INVALID_ARG );

	/* 不能同时是TCP又是UDP */
	ME_ASSERTE_RETURN(
		!(ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_TCP) &&
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_UDP)),
		ME_ERROR_INVALID_ARG );

	/* 不能是 “没有开启TCP时开启Length” */
	ME_ASSERTE_RETURN(
		!(ME_BIT_DISABLED(dwType, IConnectionManager::CONNECTION_TYPE_TCP) &&
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_LENGTH)),
		ME_ERROR_INVALID_ARG );

    /* 因为SSL也具有处理分包和粘包的能力，所以两者有一个启用了，就能用心跳 */
    /* 不能是 “没有开启Length时也没有开启SSL时开启KeepAlive” */
	ME_ASSERTE_RETURN(
		!(ME_BIT_DISABLED(dwType, IConnectionManager::CONNECTION_TYPE_LENGTH) &&
        ME_BIT_DISABLED(dwType, IConnectionManager::CONNECTION_TYPE_SSL) &&
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_KEEP_ALIVE)),
		ME_ERROR_INVALID_ARG );

    /* 不能是 “没有开启TCP时开启SSL” */
    ME_ASSERTE_RETURN(
        !(ME_BIT_DISABLED(dwType, IConnectionManager::CONNECTION_TYPE_TCP) &&
        ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_SSL)),
        ME_ERROR_INVALID_ARG );

    /* 因为SSL也具有处理分包和粘包的能力，所以用了SSL，就不需要用Length了 */
    if ( ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_SSL) &&
        ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_LENGTH) )
    {
        ME_ERROR_TRACE( "Set SSL and Length at same time" );

        ME_CLR_BITS( dwType, IConnectionManager::CONNECTION_TYPE_LENGTH );
    }

	/** 
	* 这里要取得当前线程的ID，看看与网络线程ID是不是一样
	* 如果一样，直接调用CreateClientImpl
	*
	* 不一样，就new xxxxCrossThread系列的东西，然后Open它
	*/
	CMEThread* pThread = (CMEThread*)CMEThreadManager::Instance()->GetNetworkThread();

	if ( (OS::GetThreadID() != pThread->GetThreadOSID()) &&
		 (ME_BIT_DISABLED(pThread->GetReactor()->GetType(), CMEReactor::REACTOR_TYPE_ASYNC)) )
	{
		pConnector = new CMEConnectorCrossThread( dwType );
	}
	else
	{
		return CreateClientImpl(
			dwType, 
			pConnector );
	}

	return ME_OK;
}

ME_Result CMEConnectionManager::CreateServer( 
	DWORD dwType, 
	CAcceptorAutoPtr& pAcceptor )
{
	/* 必须是TCP或UDP协议 */
	ME_ASSERTE_RETURN(
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_TCP) ||
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_UDP),
		ME_ERROR_INVALID_ARG );

	/* 不能同时是TCP又是UDP */
	ME_ASSERTE_RETURN(
		!(ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_TCP) &&
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_UDP)),
		ME_ERROR_INVALID_ARG );

	/* 不能是 “没有开启TCP时开启Length” */
	ME_ASSERTE_RETURN(
		!(ME_BIT_DISABLED(dwType, IConnectionManager::CONNECTION_TYPE_TCP) &&
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_LENGTH)),
		ME_ERROR_INVALID_ARG );

    /* 因为SSL也具有处理分包和粘包的能力，所以两者有一个启用了，就能用心跳 */
	/* 不能是 “没有开启Length时也没有开启SSL时开启KeepAlive” */
	ME_ASSERTE_RETURN(
		!(ME_BIT_DISABLED(dwType, IConnectionManager::CONNECTION_TYPE_LENGTH) &&
        ME_BIT_DISABLED(dwType, IConnectionManager::CONNECTION_TYPE_SSL) &&
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_KEEP_ALIVE)),
		ME_ERROR_INVALID_ARG );

    /* 不能是 “没有开启TCP时开启SSL” */
    ME_ASSERTE_RETURN(
        !(ME_BIT_DISABLED(dwType, IConnectionManager::CONNECTION_TYPE_TCP) &&
        ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_SSL)),
        ME_ERROR_INVALID_ARG );

    /* 因为SSL也具有处理分包和粘包的能力，所以用了SSL，就不需要用Length了 */
    if ( ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_SSL) &&
        ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_LENGTH) )
    {
        ME_ERROR_TRACE( "Set SSL and Length at same time" );

        ME_CLR_BITS( dwType, IConnectionManager::CONNECTION_TYPE_LENGTH );
    }

	/** 
	* 这里要取得当前线程的ID，看看与网络线程ID是不是一样
	* 如果一样，直接调用CreateServerImpl
	*
	* 不一样，就new xxxxCrossThread系列的东西，然后Open它
	*/
	CMEThread* pThread = (CMEThread*)CMEThreadManager::Instance()->GetNetworkThread();
	
	if ( (OS::GetThreadID() != pThread->GetThreadOSID()) &&
		 (ME_BIT_DISABLED(pThread->GetReactor()->GetType(), CMEReactor::REACTOR_TYPE_ASYNC)) )
	{
		pAcceptor = new CMEAcceptorCrossThread( dwType );
	}
	else
	{
		return CreateServerImpl(
			dwType, 
			pAcceptor );
	}
	
	return ME_OK;
}

ME_Result CMEConnectionManager::CreateClientImpl( 
	DWORD dwType, 
	CConnectorAutoPtr& pConnector )
{
	pConnector = new CMEConnectorWrapper;
	ME_Result hResult = ((CMEConnectorWrapper*)pConnector.ParaIn())->Initialize( dwType );
	if ( ME_FAILED(hResult) )
	{
		pConnector = NULL;
	}

    /**< 如果需要启用SSL，那么ConnectorOperatorSSL必须最先注册给Connector */
    if ( ME_BIT_ENABLED(dwType, CONNECTION_TYPE_SSL) )
    {
        IMEConnectionOperator* pOperator = new CMEConnectorOperatorSSL();
        ((CMEConnectorWrapper*)pConnector.ParaIn())->RegisterOperator( pOperator );
    }

	return hResult;
}

ME_Result CMEConnectionManager::CreateServerImpl( 
	DWORD dwType, 
	CAcceptorAutoPtr& pAcceptor )
{
	pAcceptor = new CMEAcceptorWrapper;
	ME_Result hResult = ((CMEAcceptorWrapper*)pAcceptor.ParaIn())->Initialize( dwType );
	if ( ME_FAILED(hResult) )
	{
		pAcceptor = NULL;
	}

    /**< 如果需要启用SSL，那么AcceptorOperatorSSL必须最先注册给Acceptor */
    if ( ME_BIT_ENABLED(dwType, CONNECTION_TYPE_SSL) )
    {
        IMEConnectionOperator* pOperator = new CMEAcceptorOperatorSSL();
        ((CMEAcceptorWrapper*)pAcceptor.ParaIn())->RegisterOperator( pOperator );
    }

	return hResult;
}

#ifdef ME_WIN

void CMEConnectionManager::LoadAcceptEx()
{
	if ( NULL == g_lpfnAcceptEx )
	{
		CMEInetAddress hiaAddr(
			ME_LOCAL_IP_ADDRESS,
			0 );

		/* 临时构建一个句柄出来，用于满足WSAIoctl()函数的要求 */
		CMESocketTcp hsSocket;
		ME_Result hResult = hsSocket.Open(
			hiaAddr,
			TRUE );

		ME_ASSERTE( ME_SUCCEEDED(hResult) );

		GUID GuidAcceptEx = WSAID_ACCEPTEX;
		DWORD dwBytes = 0;

		int iResult = ::WSAIoctl(
			(SOCKET)hsSocket.GetHandle(), 
			SIO_GET_EXTENSION_FUNCTION_POINTER, 
			&GuidAcceptEx, 
			sizeof(GuidAcceptEx),
			&g_lpfnAcceptEx, 
			sizeof(g_lpfnAcceptEx), 
			&dwBytes, 
			NULL, 
			NULL );

		ME_ASSERTE( ME_SUCCEEDED(iResult) );

		/* 函数指针<g_lpfnAcceptEx>已经顺利取出，这个临时的socket也没用了，关闭，释放它 */
		hsSocket.Close();
	}
}

void CMEConnectionManager::LoadConnectEx()
{
	if ( NULL == g_lpfnConnectEx )
	{
		CMEInetAddress hiaAddr(
			ME_LOCAL_IP_ADDRESS,
			0 );

		/* 临时构建一个句柄出来，用于满足WSAIoctl()函数的要求 */
		CMESocketTcp hsSocket;
		ME_Result hResult = hsSocket.Open(
			hiaAddr,
			TRUE );

		ME_ASSERTE( ME_SUCCEEDED(hResult) );

		GUID GuidConnectEx = WSAID_CONNECTEX;
		DWORD dwBytes = 0;

		int iResult = ::WSAIoctl(
			(SOCKET)hsSocket.GetHandle(), 
			SIO_GET_EXTENSION_FUNCTION_POINTER, 
			&GuidConnectEx, 
			sizeof(GuidConnectEx),
			&g_lpfnConnectEx, 
			sizeof(g_lpfnConnectEx), 
			&dwBytes, 
			NULL, 
			NULL );

		ME_ASSERTE( ME_SUCCEEDED(iResult) );

		/* 函数指针<g_lpfnConnectEx>已经顺利取出，这个临时的socket也没用了，关闭，释放它 */
		hsSocket.Close();
	}
}

void CMEConnectionManager::LoadGetAcceptExSockaddrs()
{
	if ( NULL == g_lpfnGetAcceptExSockaddrs )
	{
		CMEInetAddress hiaAddr(
			ME_LOCAL_IP_ADDRESS,
			0 );

		/* 临时构建一个句柄出来，用于满足WSAIoctl()函数的要求 */
		CMESocketTcp hsSocket;
		ME_Result hResult = hsSocket.Open(
			hiaAddr,
			TRUE );

		ME_ASSERTE( ME_SUCCEEDED(hResult) );

		GUID GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
		DWORD dwBytes = 0;

		int iResult = ::WSAIoctl(
			(SOCKET)hsSocket.GetHandle(), 
			SIO_GET_EXTENSION_FUNCTION_POINTER, 
			&GuidGetAcceptExSockaddrs, 
			sizeof(GuidGetAcceptExSockaddrs),
			&g_lpfnGetAcceptExSockaddrs, 
			sizeof(g_lpfnGetAcceptExSockaddrs), 
			&dwBytes, 
			NULL, 
			NULL );

		ME_ASSERTE( ME_SUCCEEDED(iResult) );

		/* 函数指针<g_lpfnGetAcceptExSockaddrs>已经顺利取出，这个临时的socket也没用了，关闭，释放它 */
		hsSocket.Close();
	}
}

#endif	// ME_WIN

ME_NAME_SPACE_END









