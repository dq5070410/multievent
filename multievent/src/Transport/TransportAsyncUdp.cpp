#include "TransportAsyncUdp.h"
#include "ConnectionManager.h"
#include "MessageBlock.h"
#include "Reactor.h"
#include "Thread.h"
#include "ThreadManager.h"
#include "SocketUdp.h"

ME_NAME_SPACE_BEGIN

CMETransportAsyncUdp::CMETransportAsyncUdp( ME_HANDLE hSocketHandle )
	: CMETransportUdp( hSocketHandle )

#ifdef ME_LINUX
	, CMEAsyncIoResult(
		CMEReactor::EVENT_TYPE_READ | CMEReactor::EVENT_TYPE_WRITE,
		NULL,
		hSocketHandle )

	, m_bInUpcall( FALSE )

#endif	// ME_WIN

	, m_pRefSink( NULL )
{
#ifdef ME_WIN
	m_pmbBlock = new CMEMessageBlock( ME_CONNECTION_SEND_BUF_SIZE );
	m_pmbBlock->SetWritePos( ME_CONNECTION_SEND_BUF_SIZE );
	m_pmbBlock->AddReference();

	m_lSentFailed = 0;
#endif	// ME_WIN
}

CMETransportAsyncUdp::~CMETransportAsyncUdp()
{
#ifdef ME_WIN
	if ( m_pmbBlock )
	{
		m_pmbBlock->RemoveReference();
	}

#endif	// ME_WIN
}

DWORD CMETransportAsyncUdp::AddReference()
{
	if ( m_pRefSink )
	{
		m_pRefSink->AddReference();
	}

	return CMEReferenceControlT<MutexType>::AddReference();
}

DWORD CMETransportAsyncUdp::RemoveReference()
{
	if ( m_pRefSink )
	{
		m_pRefSink->RemoveReference();
	}

	return CMEReferenceControlT<MutexType>::RemoveReference();
}

#ifdef ME_WIN

ME_Result CMETransportAsyncUdp::Open( 
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
		(CMETransportUdp*)this,
		CMEReactor::EVENT_TYPE_READ,
		m_iIndex );

	m_pSink = pSink;

	/* 保存一份<pSink>作为，使<pSink>的生命周期与TransportAsyncUdp相同 */
	m_pRefSink = pSink;
	m_pRefSink->AddReference();

	/* 无论如何，先投递一个读事件，用于检测连接断开 */
	return m_asyncRecvFrom.Post(
		this,
		GetHandle(),
		m_pmbBlock,
		&m_PeerAddress );
}

ME_Result CMETransportAsyncUdp::SendData( 
	CMEReactor* pReactor, 
	CMEMessageBlock* pmbBlock )
{
	pmbBlock->AddReference();

	ME_Result hResult = ME_ERROR_WOULD_BLOCK;
	if ( m_lSentFailed == 0 )
	{
		hResult = m_asyncSendTo.Post(
			this,
			GetHandle(),
			pmbBlock,
			&m_PeerAddress );

		if ( ME_FAILED(hResult) )
		{
			++m_lSentFailed;
		}
	}

	pmbBlock->RemoveReference();

	return hResult;
}

ME_Result CMETransportAsyncUdp::Close( 
	CMEReactor* pReactor, 
	ME_Result hReason /* = ME_OK */ )
{
	m_asyncSendTo.Cancel( GetHandle() );
	m_asyncRecvFrom.Cancel( GetHandle() );

	return CMETransportUdp::Close(
		pReactor,
		hReason );
}

void CMETransportAsyncUdp::OnInput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	CMEAsyncIoRecvFrom::Result* pIoResult = (CMEAsyncIoRecvFrom::Result*)hHandle;
	//m_PeerAddress = pIoResult->m_hiaFromAddress;

	m_pSink->OnReceive(
		this,
		pIoResult->m_pmbBlock.ParaIn() );

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
	m_asyncRecvFrom.Post(
		this,
		GetHandle(),
		m_pmbBlock,
		&m_PeerAddress );
}

void CMETransportAsyncUdp::OnOutput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	--m_lSentFailed;
	
	CMETransportUdp::OnOutput(
		GetHandle(),
		iEvent );
}

void CMETransportAsyncUdp::OnClose( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	m_asyncSendTo.Cancel( GetHandle() );
	m_asyncRecvFrom.Cancel( GetHandle() );

	CMETransportUdp::OnClose(
		GetHandle(),
		iEvent );
}

// 由于担心内存碎片过多的问题，把这个函数注释掉，不使用
//CMEMessageBlock* CMETransportAsyncUdp::CreateAsyncIoBuf()
//{
//	char* pszBuf = new char[ME_CONNECTION_SEND_BUF_SIZE];
//	CMEMessageBlock* pmbBlock = new CMEMessageBlock( 
//		ME_CONNECTION_SEND_BUF_SIZE, 
//		pszBuf, 
//		ME_CONNECTION_SEND_BUF_SIZE, 
//		CMEMessageBlock::MESSAGEBLOCK_ATTR_DELETE_USERDATA );
//
//	return pmbBlock;
//}

#elif defined( ME_LINUX )

void CMETransportAsyncUdp::OnOutput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	ME_ASSERTE_RETURN_VOID( m_pSink );
	m_pSink->OnCanSendAgain(
		this,
		ME_OK );
}

void CMETransportAsyncUdp::OnCompleteImpl( 
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

void CMETransportAsyncUdp::OnDestroy()
{
	RemoveReference();
}

#endif	// ME_WIN






ME_NAME_SPACE_END




