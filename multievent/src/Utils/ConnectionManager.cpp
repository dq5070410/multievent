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
	/* ������TCP��UDPЭ�� */
	ME_ASSERTE_RETURN(
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_TCP) ||
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_UDP),
		ME_ERROR_INVALID_ARG );

	/* ����ͬʱ��TCP����UDP */
	ME_ASSERTE_RETURN(
		!(ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_TCP) &&
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_UDP)),
		ME_ERROR_INVALID_ARG );

	/* ������ ��û�п���TCPʱ����Length�� */
	ME_ASSERTE_RETURN(
		!(ME_BIT_DISABLED(dwType, IConnectionManager::CONNECTION_TYPE_TCP) &&
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_LENGTH)),
		ME_ERROR_INVALID_ARG );

    /* ��ΪSSLҲ���д���ְ���ճ��������������������һ�������ˣ����������� */
    /* ������ ��û�п���LengthʱҲû�п���SSLʱ����KeepAlive�� */
	ME_ASSERTE_RETURN(
		!(ME_BIT_DISABLED(dwType, IConnectionManager::CONNECTION_TYPE_LENGTH) &&
        ME_BIT_DISABLED(dwType, IConnectionManager::CONNECTION_TYPE_SSL) &&
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_KEEP_ALIVE)),
		ME_ERROR_INVALID_ARG );

    /* ������ ��û�п���TCPʱ����SSL�� */
    ME_ASSERTE_RETURN(
        !(ME_BIT_DISABLED(dwType, IConnectionManager::CONNECTION_TYPE_TCP) &&
        ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_SSL)),
        ME_ERROR_INVALID_ARG );

    /* ��ΪSSLҲ���д���ְ���ճ������������������SSL���Ͳ���Ҫ��Length�� */
    if ( ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_SSL) &&
        ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_LENGTH) )
    {
        ME_ERROR_TRACE( "Set SSL and Length at same time" );

        ME_CLR_BITS( dwType, IConnectionManager::CONNECTION_TYPE_LENGTH );
    }

	/** 
	* ����Ҫȡ�õ�ǰ�̵߳�ID�������������߳�ID�ǲ���һ��
	* ���һ����ֱ�ӵ���CreateClientImpl
	*
	* ��һ������new xxxxCrossThreadϵ�еĶ�����Ȼ��Open��
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
	/* ������TCP��UDPЭ�� */
	ME_ASSERTE_RETURN(
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_TCP) ||
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_UDP),
		ME_ERROR_INVALID_ARG );

	/* ����ͬʱ��TCP����UDP */
	ME_ASSERTE_RETURN(
		!(ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_TCP) &&
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_UDP)),
		ME_ERROR_INVALID_ARG );

	/* ������ ��û�п���TCPʱ����Length�� */
	ME_ASSERTE_RETURN(
		!(ME_BIT_DISABLED(dwType, IConnectionManager::CONNECTION_TYPE_TCP) &&
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_LENGTH)),
		ME_ERROR_INVALID_ARG );

    /* ��ΪSSLҲ���д���ְ���ճ��������������������һ�������ˣ����������� */
	/* ������ ��û�п���LengthʱҲû�п���SSLʱ����KeepAlive�� */
	ME_ASSERTE_RETURN(
		!(ME_BIT_DISABLED(dwType, IConnectionManager::CONNECTION_TYPE_LENGTH) &&
        ME_BIT_DISABLED(dwType, IConnectionManager::CONNECTION_TYPE_SSL) &&
		ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_KEEP_ALIVE)),
		ME_ERROR_INVALID_ARG );

    /* ������ ��û�п���TCPʱ����SSL�� */
    ME_ASSERTE_RETURN(
        !(ME_BIT_DISABLED(dwType, IConnectionManager::CONNECTION_TYPE_TCP) &&
        ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_SSL)),
        ME_ERROR_INVALID_ARG );

    /* ��ΪSSLҲ���д���ְ���ճ������������������SSL���Ͳ���Ҫ��Length�� */
    if ( ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_SSL) &&
        ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_LENGTH) )
    {
        ME_ERROR_TRACE( "Set SSL and Length at same time" );

        ME_CLR_BITS( dwType, IConnectionManager::CONNECTION_TYPE_LENGTH );
    }

	/** 
	* ����Ҫȡ�õ�ǰ�̵߳�ID�������������߳�ID�ǲ���һ��
	* ���һ����ֱ�ӵ���CreateServerImpl
	*
	* ��һ������new xxxxCrossThreadϵ�еĶ�����Ȼ��Open��
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

    /**< �����Ҫ����SSL����ôConnectorOperatorSSL��������ע���Connector */
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

    /**< �����Ҫ����SSL����ôAcceptorOperatorSSL��������ע���Acceptor */
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

		/* ��ʱ����һ�������������������WSAIoctl()������Ҫ�� */
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

		/* ����ָ��<g_lpfnAcceptEx>�Ѿ�˳��ȡ���������ʱ��socketҲû���ˣ��رգ��ͷ��� */
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

		/* ��ʱ����һ�������������������WSAIoctl()������Ҫ�� */
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

		/* ����ָ��<g_lpfnConnectEx>�Ѿ�˳��ȡ���������ʱ��socketҲû���ˣ��رգ��ͷ��� */
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

		/* ��ʱ����һ�������������������WSAIoctl()������Ҫ�� */
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

		/* ����ָ��<g_lpfnGetAcceptExSockaddrs>�Ѿ�˳��ȡ���������ʱ��socketҲû���ˣ��رգ��ͷ��� */
		hsSocket.Close();
	}
}

#endif	// ME_WIN

ME_NAME_SPACE_END









