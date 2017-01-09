#include "ConnectorAsyncTcp.h"
#include "Reactor.h"

#include "ReactorAsyncEpoll.h"

ME_NAME_SPACE_BEGIN

CMEConnectorAsyncTcp::CMEConnectorAsyncTcp()
#ifdef ME_LINUX
	: CMEAsyncIoResult(
		CMEReactor::EVENT_TYPE_CONNECT,
		NULL )

	, m_pRefSink( NULL )
#else	// ME_LINUX

	: m_pRefSink( NULL )
#endif	// ME_LINUX
{

}

CMEConnectorAsyncTcp::~CMEConnectorAsyncTcp()
{

}

DWORD CMEConnectorAsyncTcp::AddReference()
{
	if ( m_pRefSink )
	{
		m_pRefSink->AddReference();
	}

	return CMEReferenceControlT<MutexType>::AddReference();
}

DWORD CMEConnectorAsyncTcp::RemoveReference()
{
	if ( m_pRefSink )
	{
		m_pRefSink->RemoveReference();
	}

	return CMEReferenceControlT<MutexType>::RemoveReference();
} 

#ifdef ME_WIN

ME_Result CMEConnectorAsyncTcp::Open( 
	IMEConnectorImplSink* pSink, 
	const CMEInetAddress& peerAddress, 
	CMEReactor* pReactor, 
	ME_HANDLE& hHandle, 
	CMEInetAddress* pLocalAddress /* = NULL */ )
{
	ME_ASSERTE_RETURN( pSink && pReactor, ME_ERROR_NULL_POINTER );

	//Close();	// 确保不会重复打开

	ME_Result hResult = ME_ERROR_FAILURE;
	BOOL bAutoBind = FALSE;
	if ( pLocalAddress )
	{
		hResult = m_SocketTcp.Open(
			*pLocalAddress,
			TRUE );
	}
	else
	{
		bAutoBind = TRUE;

		hResult = m_SocketTcp.Open( TRUE );
	}

	/* 不管Open是成功还是失败，都会有新的句柄产生 */
	hHandle = m_SocketTcp.GetHandle();

	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	/**
	* 保留回调对象
	* 
	* 在多线程环境下，一旦<m_asyncConnect>一旦Post就马上有一个
	* 线程来处理，这有可能导致，<m_pSink>还没有被赋值，那边已经
	* 在处理连接成功的消息，最终引起OnConnect()没有回调
	*/
	m_pSink = pSink;

	/* 保存一份<pSink>作为，使<pSink>的生命周期与TransportAsyncTcp相同 */
	m_pRefSink = pSink;

	/* 这里不能增加引用计数，因为会导致sink无法删除而导致内存泄露 */
	//m_pRefSink->AddReference();

	int iTmpIndex = -1;
	hResult = pReactor->Register(
		this,
		(CMEConnectorTcp*)this,
		CMEReactor::EVENT_TYPE_CONNECT | CMEReactor::EVENT_TYPE_CLOSE,
		iTmpIndex );

	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	hResult = m_asyncConnect.Post(
		this,
		GetHandle(),
		&peerAddress,
		/**
		* 更为准确的写法应该是根据<peerAddress>的类型 - IPv4还是IPv6来确定
		* 但由于CMEInetAddress还没有提供这个方法，所以暂时固定为IPv4
		*/
		TRUE,
		bAutoBind );

	if ( ME_SUCCEEDED(hResult) )
	{
		//m_pSink = pSink;
		m_pReactor = pReactor;
	}

	return hResult;
}

void CMEConnectorAsyncTcp::Close()
{
	m_asyncConnect.Cancel( GetHandle() );

	CMEConnectorTcp::Close();
}

void CMEConnectorAsyncTcp::OnInput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	ME_ASSERTE( FALSE );
}

void CMEConnectorAsyncTcp::OnOutput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	CMEConnectorTcp::OnOutput(
		GetHandle(),
		iEvent );
}

void CMEConnectorAsyncTcp::OnClose( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	CMEConnectorTcp::OnClose(
		GetHandle(),
		iEvent );
}

#elif defined( ME_LINUX )

ME_Result CMEConnectorAsyncTcp::Open(
    IMEConnectorImplSink* pSink,
    const CMEInetAddress& peerAddress,
    CMEReactor* pReactor,
    ME_HANDLE& hHandle,
    CMEInetAddress* pLocalAddress /* = NULL */ )
{
	ME_ASSERTE_RETURN( pSink, ME_ERROR_NULL_POINTER );

	ME_Result hResult = ME_ERROR_FAILURE;
    if ( pLocalAddress )
    {
        hResult = m_SocketTcp.Open(
            *pLocalAddress,
            TRUE );
    }
    else
    {
        hResult = m_SocketTcp.Open( TRUE );
    }

	hHandle = m_SocketTcp.GetHandle();
    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	hResult = m_SocketTcp.Enable( CMEIPBase::NON_BLOCK );
	if ( ME_FAILED(hResult) )
    {
        CMESocketErrorAutoSave errSave;
        ME_ERROR_TRACE_THIS(
            "failed to set NON_BLOCK to socket"
            << ", errno = " << errSave.GetLastError()
            //<< ", socket handle = " << tmpSocket.GetHandle() );
            << ", socket handle = " << m_SocketTcp.GetHandle() );

        return hResult;
    }

	int iTmpIndex = -1;
	hResult = Connect(
        &m_SocketTcp,
        peerAddress,
        pReactor,
        iTmpIndex );

	if ( EWOULDBLOCK == hResult )
    {
        hResult = ME_OK;   // 告诉上层本次操作成功
    }
	/* 极其罕见的情况下，就算是异步的socket也有可能一connect就成功 */
    else if ( ME_SUCCEEDED(hResult) )
    {
        /**
        * 同上上层，但不能在这个函数里通知，需要跳出来通知
        * 以免造成，Open()函数还没结束，OnConnect()就callback了这种不合理的现象
        */
        NotifySuccess();
    }

	if ( ME_SUCCEEDED(hResult) )
    {
        m_pSink = pSink;

        hResult = pReactor->Register(
            this,
            this,
            CMEReactor::EVENT_TYPE_CONNECT | CMEReactor::EVENT_TYPE_CLOSE,
            iTmpIndex );

        ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

        m_iIndex = iTmpIndex;
    }

	return hResult;
}

void CMEConnectorAsyncTcp::Close()
{
	CMESingletonT<CMEReactorAsyncEpoll>::Instance()->UnRegister(
		this,
		CMEReactor::EVENT_TYPE_ALL,
		Internal );

	Internal = -1;
	m_pSink = NULL;
	m_SocketTcp.SetHandle( ME_INVALID_HANDLE );
	m_iIndex = -1;
}

void CMEConnectorAsyncTcp::OnOutput(
	ME_HANDLE hHandle,
	int iEvent )
{
	ME_ASSERTE_RETURN_VOID( m_pSink );
	IMEConnectorImplSink* pTmpSink = m_pSink;

	Close();

	pTmpSink->OnConnect(
		ME_OK,
		hHandle );
}

void CMEConnectorAsyncTcp::OnCompleteImpl( 
	BOOL bResult,
	int iActiveEvent,
	ME_HANDLE hInterface )
{
	AddReference();

	if ( ME_BIT_DISABLED(iActiveEvent, EPOLLERR) )
	{
		OnOutput(
			GetHandle(),
			CMEReactor::EVENT_TYPE_CONNECT );
	}
	else
	{
		CMEConnectorTcp::OnClose(
			GetHandle(),
			CMEReactor::EVENT_TYPE_ERR );
	}
}

void CMEConnectorAsyncTcp::OnDestroy()
{
	RemoveReference();
}

#endif	// ME_WIN


ME_NAME_SPACE_END



