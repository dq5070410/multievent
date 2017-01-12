#include "TransportAsyncTcp.h"
#include "ConnectionManager.h"
#include "MessageBlock.h"
#include "Reactor.h"
#include "Thread.h"
#include "ThreadManager.h"
#include "SocketTcp.h"
#include "SocketUdp.h"
#include "EventTransportT.h"

const DWORD ME_ASYNC_MAX_POST = 8;

ME_NAME_SPACE_BEGIN

CMETransportAsyncTcp::CMETransportAsyncTcp( ME_HANDLE hSocketHandle )
	: CMETransportTcp( hSocketHandle )

#ifdef ME_LINUX
	, CMEAsyncIoResult( 
		CMEReactor::EVENT_TYPE_READ | CMEReactor::EVENT_TYPE_WRITE, 
		NULL, 
		hSocketHandle )

	, m_bInUpcall( FALSE )	

#endif	// ME_LINUX

	, m_pRefSink( NULL )
{
#ifdef ME_WIN
	m_pmbBlock = new CMEMessageBlock( ME_CONNECTION_SEND_BUF_SIZE );
	m_pmbBlock->SetWritePos( ME_CONNECTION_SEND_BUF_SIZE );
	m_pmbBlock->AddReference();

	m_lSentFailed = 0;
#endif	// ME_WIN
}
	
CMETransportAsyncTcp::~CMETransportAsyncTcp()
{
#ifdef ME_WIN
	if ( m_pmbBlock )
	{
		m_pmbBlock->RemoveReference();
	}

#endif	// ME_WIN
}

DWORD CMETransportAsyncTcp::AddReference()
{
	if ( m_pRefSink )
	{
		m_pRefSink->AddReference();
	}

	return CMEReferenceControlT<MutexType>::AddReference();
}

DWORD CMETransportAsyncTcp::RemoveReference()
{
	if ( m_pRefSink )
	{
		m_pRefSink->RemoveReference();
	}

	return CMEReferenceControlT<MutexType>::RemoveReference();
}

#ifdef ME_WIN

ME_Result CMETransportAsyncTcp::Open( 
	CMEReactor* pReactor, 
	IMETransportImplSink* pSink )
{
	/**
	* 这里不能像ConnectAsync或AcceptAsync的Open函数那样直接的call父类
	* 的Open，因为transport句柄是从Connector或Acceptor转移过来的，
	* 从Acceptor过来的还好，因为是新的句柄，进行Register的时候不会失败，
	* 而Connector过来的就不行了，因为是同一个句柄，而且之前已经使用
	* ::CreateIoCompletionPort绑定过，如果再一次的绑定 - call父类的Open
	* 就会有绑定的动作，所以Register一定会失败，一旦失败就导致<m_pSink>
	* <m_pReactor>等从父类继承过来的成员变量赋值失败，其最终结果就是
	* 导致在callback的时候由于<m_pSink>为空而事件无法通知上层
	*
	* 因此，这个函数只能重新实现
	*/

	ME_ASSERTE_RETURN( pReactor && pSink, ME_ERROR_NULL_POINTER );
	m_pReactor = pReactor;

	SetBuffSize();

	/**
	* 2014.1.23 added by 韦珂
	* 设为异步socket
	*/
	ME_Result hResult = m_pSocket->Enable( CMEIPBase::NON_BLOCK );
	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	/* 不判断返回值，因为有可能是重复绑定而失败 */
	pReactor->Register(
		this,
		(CMETransportTcp*)this,
		CMEReactor::EVENT_TYPE_READ,
		m_iIndex );

	m_pSink = pSink;

	/* 保存一份<pSink>作为，使<pSink>的生命周期与TransportAsyncTcp相同 */
	m_pRefSink = pSink;

	/* 2014.11.4 edited by 韦珂 */
	/* 这里不能增加引用计数，因为会导致sink无法删除而导致内存泄露 */
	//m_pRefSink->AddReference();

	/* 无论如何，先投递一个读事件，用于检测连接断开 */
	return m_asyncRecv.Post(
		this,
		GetHandle(),
		m_pmbBlock );
}

ME_Result CMETransportAsyncTcp::SendData( 
	CMEReactor* pReactor, 
	CMEMessageBlock* pmbBlock )
{
	pmbBlock->AddReference();

	ME_Result hResult = ME_ERROR_WOULD_BLOCK;

	/**
	* 在上一次Post失败(EWOULDBLOCK)的情况下，允许再多投递
	* ME_ASYNC_MAX_POST个，在一定程度上改善收发的性能(不过会
	* 引起少量的内存使用量上升 -- 连接越多上升越多)
	*/
	if ( m_lSentFailed <= ME_ASYNC_MAX_POST )
	{
		hResult = m_asyncSend.Post(
			this,
			GetHandle(),
			pmbBlock );

		if ( ME_FAILED(hResult) )
		{
			++m_lSentFailed;
		}
	}
	
	pmbBlock->RemoveReference();

	return hResult;
}

ME_Result CMETransportAsyncTcp::Close( 
	CMEReactor* pReactor, 
	ME_Result hReason /* = ME_OK */ )
{
	m_asyncSend.Cancel( GetHandle() );
	m_asyncRecv.Cancel( GetHandle() );

	return CMETransportBase::Close(
		pReactor,
		hReason );
}

void CMETransportAsyncTcp::OnInput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	CMEAsyncIoRecv::Result* pIoResult = (CMEAsyncIoRecv::Result*)hHandle;

	if ( m_pSink )
	{
		m_pSink->OnReceive(
			this,
			pIoResult->m_pmbBlock.ParaIn() );
	}
	
	/**
	* 不用担心在上边的回调函数中发生一些诸如关闭、销毁之类
	* 可能会导致本对象被删除而导致下边的调用会crash的问题
	*
	* wrapper们会搞定的
	*/
	m_pmbBlock->SetReadPos( 0 );
	m_pmbBlock->SetWritePos( m_pmbBlock->GetCapacity() );
	/**
	* 上一个异步的RecvIo已经消耗，现在要再投递一个
	* 以为下一个即将到来的数据或连接断开通知做准备
	*/
	m_asyncRecv.Post(
		this,
		GetHandle(),
		m_pmbBlock );
}

void CMETransportAsyncTcp::OnOutput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	--m_lSentFailed;

	CMETransportBase::OnOutput(
		hHandle,
		iEvent );
}

void CMETransportAsyncTcp::OnClose( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	m_asyncSend.Cancel( GetHandle() );
	m_asyncRecv.Cancel( GetHandle() );

	CMETransportTcp::OnClose(
		GetHandle(),
		iEvent );
}

// 由于担心内存碎片过多的问题，把这个函数注释掉，不使用
//CMEMessageBlock* CMETransportAsyncTcp::CreateAsyncIoBuf()
//{
//	BYTE* pszBuf = new BYTE[ME_CONNECTION_SEND_BUF_SIZE];
//	CMEMessageBlock* pmbBlock = new CMEMessageBlock( 
//		ME_CONNECTION_SEND_BUF_SIZE, 
//		(void*)pszBuf, 
//		ME_CONNECTION_SEND_BUF_SIZE, 
//		CMEMessageBlock::MESSAGEBLOCK_ATTR_DELETE_USERDATA );
//	
//	return pmbBlock;
//}

#elif defined( ME_LINUX )

ME_Result CMETransportAsyncTcp::SendData(
    CMEReactor* pReactor,
    CMEMessageBlock* pmbBlock )
{
	ME_ASSERTE_RETURN( pReactor && pmbBlock && GetHandle(), ME_ERROR_NULL_POINTER );
    ME_ASSERTE_RETURN( ME_INVALID_HANDLE != GetHandle(), ME_ERROR_NETWORK_SOCKET_ERROR );
    m_pReactor = pReactor;

    iovec szData[ME_IOV_MAX];
	int iVecCount = pmbBlock->FillIOVec(
		szData,
		ME_IOV_MAX );

	int iHaveSend = m_pSocket->SendV(
        szData,
	    iVecCount );

	ME_Result hResult = ME_ERROR_FAILURE;
	if( 0 > iHaveSend ) 
	{
		if ( EWOULDBLOCK == errno )
		{
			InternalHigh = 1;

			if ( m_bInUpcall == FALSE ) 
			{
				hResult = pReactor->Register(
					this,
					this,
					CMEReactor::EVENT_TYPE_READ | CMEReactor::EVENT_TYPE_WRITE,
					m_iIndex );

        		ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult ); 
			}

			return ME_ERROR_WOULD_BLOCK;
		}
		else
		{
			ME_WARNING_TRACE_THIS( "connection broke" );
			TransportSinkType* pSink = m_pSink;
			Close(
				pReactor,
				ME_ERROR_NETWORK_SOCKET_CLOSE );
	
			OnDisconnectUserEvent* pEvent = new OnDisconnectUserEvent(
				this,
				pSink,
				ME_ERROR_NETWORK_SOCKET_CLOSE );

			ME_ASSERTE_RETURN( pEvent, ME_ERROR_OUT_OF_MEMORY );
			CMEThread* pThread = (CMEThread*)CMEThreadManager::Instance()->GetCurrentThread();
			pEvent->Run( pThread );
       
			return ME_ERROR_NETWORK_SOCKET_CLOSE;
		}
	}

	pmbBlock->MoveReadPos(
        CMEMessageBlock::DIRECTION_TYPE_FORWORD,
        iHaveSend ); // 正向移动读指针
							   
    if ( 0 < pmbBlock->GetLength() )    // 还有剩余数据，也就是，只发送了一部分
    {
		InternalHigh = 1;

		if ( m_bInUpcall == FALSE ) 
		{
        	hResult = pReactor->Register(
	        	this,
	        	this,
	        	CMEReactor::EVENT_TYPE_READ | CMEReactor::EVENT_TYPE_WRITE,
	        	m_iIndex );

        	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult ); 
		}

        return ME_ERROR_NETWORK_PARTIAL_DATA;
	}

	InternalHigh = 0;

	return ME_OK;
}

void CMETransportAsyncTcp::OnOutput(
    ME_HANDLE hHandle,
	int iEvent )
{
	ME_ASSERTE_RETURN_VOID( m_pSink );
	m_pSink->OnCanSendAgain(
		this,
		ME_OK );
}

void CMETransportAsyncTcp::OnCompleteImpl( 
	BOOL bResult,
	int iActiveEvent,
	ME_HANDLE hInterface )
{
	AddReference();

	m_bInUpcall = TRUE;

	ME_ASSERTE_RETURN_VOID( m_pSink );

	int iEvent = CMEReactor::EVENT_TYPE_READ;
	if ( ME_BIT_ENABLED(iActiveEvent, EPOLLIN) )
	{
		OnInput(
			GetHandle(),
			CMEReactor::EVENT_TYPE_READ );
	}
	
	if ( ME_BIT_ENABLED(iActiveEvent, EPOLLOUT) )
	{
		InternalHigh = 0;

		OnOutput(
			GetHandle(),
			CMEReactor::EVENT_TYPE_WRITE );
	}

	if ( (TRUE == bResult) )
	{
		if ( 0 != InternalHigh )
		{
			iEvent |= CMEReactor::EVENT_TYPE_WRITE;
		}

		CMEReactor* pReactor = ((CMEThread*)CMEThreadManagerSingleton::Instance()->GetNetworkThread())->GetReactor();
		pReactor->Register(
			this,
			this,
			iEvent,
			m_iIndex );
	}

	m_bInUpcall = FALSE;
}

void CMETransportAsyncTcp::OnDestroy()
{
	RemoveReference();
}


#endif	// ME_WIN

ME_NAME_SPACE_END







