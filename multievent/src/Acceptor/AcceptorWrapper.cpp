#include "AcceptorWrapper.h"
#include "InetAddress.h"
#include "AcceptorFactory.h"
#include "TransportFactory.h"
#include "OSSocket.h"
#include "ThreadManager.h"
#include "Thread.h"
#include "TransportUdpManager.h"
#include "EventAcceptorConnectorT.h"
#include "Reactor.h"
#include "AsyncIoRecvFrom.h"
#include "MessageBlock.h"

ME_NAME_SPACE_BEGIN

typedef CMETransportUdpManagerSingleton UdpManager;

CMEAcceptorWrapper::CMEAcceptorWrapper()
	: m_dwType( IMEConnectionManager::CONNECTION_TYPE_NONE )
	, m_pAcceptor( NULL )
	, m_iReactorType( CMEReactor::REACTOR_TYPE_NULL )
	, m_pSink( NULL )
{
	
}

CMEAcceptorWrapper::~CMEAcceptorWrapper()
{
	/**
	* 上层需要在析构之前显示的调用Close()，
	* 这里就不再重复调用了
	*/
	//Close( ME_OK );
}

ME_Result CMEAcceptorWrapper::Initialize( DWORD dwType )
{
	AddReference();

	IMEAcceptorImpl* pAcceptor = NULL;
	ME_Result hResult = CMEAcceptorFactorySingleton::Instance()->Create(
		dwType,
		pAcceptor );

	if ( ME_FAILED(hResult) )
	{
		ME_DELETE( pAcceptor );

		
		RemoveReference();

		return hResult;
	}

	m_dwType = dwType;
	m_pAcceptor = pAcceptor;

	RemoveReference();

	return ME_OK;
}

ME_Result CMEAcceptorWrapper::Open( 
	IMEAcceptorSink* pSink, 
	const CMEInetAddress& localAddress )
{
	// 这里应该要拿到当前线程的Reactor，只有这样才能用m_pAcceptor->Open

	ME_ASSERTE_RETURN( m_pAcceptor && pSink, ME_ERROR_NULL_POINTER );
	ME_ASSERTE_RETURN( !m_pSink, ME_ERROR_ALREADY_INITIALIZED );

	AddReference();

	CMEThread* pThread = (CMEThread*)CMEThreadManager::Instance()->GetNetworkThread();
	ME_Result hResult = m_pAcceptor->Open(
		this,
		pThread->GetReactor(),
		localAddress );

	if ( ME_FAILED(hResult) )
	{
		m_pAcceptor->Close( pThread->GetReactor() );
	}
	else
	{
		m_iReactorType = pThread->GetReactor()->GetType();
		m_pSink = pSink;
	}

	RemoveReference();

	return hResult;
}

void CMEAcceptorWrapper::Close( ME_Result scReason /* = ME_OK */ )
{
	AddReference();

	CMEThread* pThread = (CMEThread*)CMEThreadManager::Instance()->GetNetworkThread();
	if ( m_pAcceptor )
	{
		m_pAcceptor->Close( pThread->GetReactor() );

		//ME_DELETE( m_pAcceptor );
		m_pAcceptor = NULL;
	}

	m_pSink = NULL;

	RemoveReference();
}

ME_Result CMEAcceptorWrapper::RegisterOperator( IMEConnectionOperator* pOperator )
{
    ME_ASSERTE_RETURN( (NULL != pOperator), ME_ERROR_INVALID_ARG );

    OperatorDequeType::iterator iter = m_dequeOperator.begin();
    for ( ; iter != m_dequeOperator.end(); ++iter )
    {
        if ( (*iter)->GetType() == pOperator->GetType() )
        {
            return ME_ERROR_FOUND;
        }
    }

    m_dequeOperator.push_back( IMEConnectionOperatorAutoPtr(pOperator) );

    return ME_OK;
}

ME_Result CMEAcceptorWrapper::UnregisterOperator( IMEConnectionOperator* pOperator )
{
    ME_ASSERTE_RETURN( (NULL != pOperator), ME_ERROR_INVALID_ARG );

    OperatorDequeType::iterator iter = m_dequeOperator.begin();
    for ( ; iter != m_dequeOperator.end(); ++iter )
    {
        if ( (*iter) == pOperator )
        {
            m_dequeOperator.erase( iter );

            break;
        }
    }

    return ME_OK;
}

ME_Result CMEAcceptorWrapper::SetOption(
    DWORD dwOptionType,
    void* pOptionValue )
{
    ME_Result hResult = ME_ERROR_FAILURE;

    switch ( dwOptionType )
    {
    case CONNECTION_OPTION_TYPE_SSL_TYPE:
    case CONNECTION_OPTION_TYPE_CERTIFICATE_FILE:
    case CONNECTION_OPTION_TYPE_PRIVATEKEY_FILE:
    case CONNECTION_OPTION_TYPE_PRIVATEKEY_PASSWORD:
        {
            OperatorDequeType::iterator iter = m_dequeOperator.begin();
            for ( ; iter != m_dequeOperator.end(); ++iter )
            {
                if ( (*iter)->GetType() == IMEConnectionManager::CONNECTION_TYPE_SSL )
                {
                    return (*iter)->SetOption(
                        dwOptionType,
                        pOptionValue);
                }
            }

    	    break;
        }

    default:
        {
            break;
        }
    }

    return hResult;
}

ME_Result CMEAcceptorWrapper::GetOption(
    WORD dwOptionType,
    void*& pOptionValue )
{
    return ME_ERROR_FAILURE;
}

DWORD CMEAcceptorWrapper::AddReference()
{
	return CMEReferenceControlT<MutexType>::AddReference();
}

DWORD CMEAcceptorWrapper::RemoveReference()
{
	return CMEReferenceControlT<MutexType>::RemoveReference();
}

void CMEAcceptorWrapper::OnConnect( 
	ME_Result hResult, 
	ME_HANDLE hHandle, 
	CMEInetAddress* pPeerAddress )
{
	ME_ASSERTE_RETURN_VOID( m_pSink );
	ME_ASSERTE_RETURN_VOID( hHandle && (ME_INVALID_HANDLE != hHandle) );

	AddReference();

	/**
	* 几乎不存在这样的可能，如果是这个函数回调上来，hResult必定是OK，
	* 不过，为了防止意外，还是加了下面这一段
	*/
	if ( ME_FAILED(hResult) )
	{
		OS::SocketClose( hHandle );

		ME_ERROR_TRACE_THIS( "Unexpected error = " << errno );

		RemoveReference();

		return;
	}

	CMETransportAutoPtr pTransportAutoPtr;
	hResult = CMETransportFactorySingleton::Instance()->Create(
		m_dwType,
		hHandle,
		pTransportAutoPtr.ParaOut() );

	ME_ASSERTE( ME_SUCCEEDED(hResult) && pTransportAutoPtr );

    hResult = PreOnConnect(
        hHandle,
        pTransportAutoPtr );

    if ( ME_FAILED(hResult) )
    {
        pTransportAutoPtr = NULL;

        RemoveReference();

        return;
    }

	m_pSink->OnConnect( pTransportAutoPtr );

	/**
	* 如果用tcpview之类的工具，来关闭一个正在监听的端口，会发生什么？
	* 如果用select模型，在listen成功之后，实际上，只有FD_READ和FD_WRITE
	* 类型的事件，如果用tcpview强行kill掉正在监听的端口，会是FD_READ发生
	* 么？如果发生，那么这个句柄<hHandle>是当初listen那个句柄么？
	*
	* 有点忘了，回头需要测试一下
	*/

	RemoveReference();
}

void CMEAcceptorWrapper::OnInput( 
	ME_Result hResult, 
	ME_HANDLE hHandle )
{
	AddReference();

	char szBuf[ME_CONNECTION_READ_MAX_SIZE];
	::memset(
		szBuf,
		0,
		ME_CONNECTION_READ_MAX_SIZE );

	
	CMEInetAddress peerAddress;
	int iRecvLen = 0;
	char* pszBuf = szBuf;

	/* 读取数据并获取对端地址 */
	ReadData(
		hHandle, 
		iRecvLen, 
		pszBuf, 
		peerAddress );

	/* 几乎不可能的 */
	if ( 0 > iRecvLen )
	{
		ME_WARNING_TRACE_THIS( "length of received data = " << iRecvLen );

		RemoveReference();

		return;
	}

	IMETransport* pTransport = UdpManager::Instance()->FindTransport( peerAddress );

	/* 通知TransportUdpManager，让它来处理OnConnect()以及OnReceive()等callback */
	NotifyUdpManager( 
		pTransport,
		iRecvLen,
		pszBuf,
		peerAddress );

	RemoveReference();
}

void CMEAcceptorWrapper::ReadData( 
	ME_HANDLE hHandle, 
	int& iRecvLen, 
	char*& pszData, 
	CMEInetAddress& hiaPeerAddr )
{
	if ( CMEReactor::REACTOR_TYPE_ASYCH_IOCP != m_iReactorType )
	{
		m_SocketUdp.SetHandle( hHandle );

		iRecvLen = m_SocketUdp.RecvFrom(
			pszData,
			ME_CONNECTION_READ_MAX_SIZE,
			hiaPeerAddr );
	}

#ifdef ME_WIN	
	else
	{
		CMEAsyncIoRecvFrom::Result* pIoResult = (CMEAsyncIoRecvFrom::Result*)hHandle;
		iRecvLen = pIoResult->m_pmbBlock->GetLength();
		hiaPeerAddr = *pIoResult->m_pFromAddress;

		/**
		*
		* 其实这段最正确且安全的写法应当是用m_pmbBlock->Read()方法把数据读取到szBuf，
		* 这样的行为就和上边一样了，但是，在AsyncIO的情况下，m_pmbBlock是预先分配且
		* 只包含一个DB，所以为了效率，就暂时这么写
		*/
		iovec ioData[1];
		pIoResult->m_pmbBlock->FillIOVec( ioData, 1 );
		pszData = ioData[0].iov_base;

		ME_ASSERTE( iRecvLen == ioData[0].iov_len );
	}
#endif	// ME_WIN
}

void CMEAcceptorWrapper::NotifyUdpManager( 
	IMETransport* pTransport, 
	int iRecvLen, 
	char* pszData, 
	const CMEInetAddress& hiaPeerAddr )
{
	/* 如果能够与对端地址绑定的transport对象能够找到 */
	if ( pTransport )
	{
		/* 说明之前就收到过来自<peerAddress>的包了，直接转<UdpManager>处理就行 */
		UdpManager::Instance()->OnUserDataReceive(
			pTransport,
			pszData,
			iRecvLen );
	}
	else
	{
		/* 没找到就先通知OnUserConnect，以便获取一个transport */
		UdpManager::Instance()->OnUserConnect(
			m_dwType,
			hiaPeerAddr,
			pTransport );

		/* 不要在这个函数里边去call OnConnect() */
		OnAcceptCurrentEvent* pEvent = new OnAcceptCurrentEvent(
			this,
			pTransport );

		pEvent->Run( CMEThreadManager::Instance()->GetCurrentThread() );

		/* 紧接着再处理数据，这样就看起来就和TCP的流程一样了 -- 先OnConnect，在OnReceive */
		UdpManager::Instance()->OnUserDataReceive(
			pTransport,
			pszData,
			iRecvLen );
	}
}

void CMEAcceptorWrapper::OnConnect_c( TransportType* pTransport )
{
	ME_ASSERTE_RETURN_VOID( pTransport );

	AddReference();

	if ( m_pSink )
	{
		CMETransportAutoPtr pTmpTransport;
		pTmpTransport.ParaOut() = pTransport;

		m_pSink->OnConnect( pTmpTransport );
	}

	RemoveReference();
}

ME_Result CMEAcceptorWrapper::PreOnConnect(
    ME_HANDLE hSocket,
    CMETransportAutoPtr& pTransportAutoPtr )
{
    ME_Result hResult = ME_OK;

    OperatorDequeType::reverse_iterator iter = m_dequeOperator.rbegin();
    for (; iter != m_dequeOperator.rend(); ++iter )
    {
        ME_ASSERTE_RETURN( ((*iter) != NULL), ME_ERROR_NULL_POINTER );

        hResult = (*iter)->PreOnConnect(
            hSocket,
            pTransportAutoPtr );

        if ( ME_FAILED(hResult) )
        {
            break;
        }
    }

    return hResult;
}

ME_NAME_SPACE_END











