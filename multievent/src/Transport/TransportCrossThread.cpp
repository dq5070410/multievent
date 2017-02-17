#include "TransportCrossThread.h"
#include "MessageBlock.h"
#include "Thread.h"
#include "ThreadManager.h"
#include "EventTransportT.h"

ME_NAME_SPACE_BEGIN

CMETransportCrossThread::CMETransportCrossThread( ITransport* pSrcTransport )
	: m_pSink( NULL )
	, m_pTransport( pSrcTransport )

	, m_bSendFailed( FALSE )
	, m_bNeedToCallCanSendAgain( FALSE )
	, m_bClosed( FALSE )
{

}

CMETransportCrossThread::~CMETransportCrossThread()
{
	m_DataBuf.clear();
}

ME_Result CMETransportCrossThread::Open( ITransportSink* pSink )
{
	ME_ASSERTE_RETURN( (m_bClosed == FALSE), ME_ERROR_NOT_AVAILABLE );

	if ( NULL == m_pSink )
	{
		{
			CMELockGuardT<CMELockThreadMutex> Guard( &m_sinkLock );
			m_pSink = pSink;
		}

		/* 第一次open, Post一个event到网络线程执行真正的open */
		OpenNetworkEvent* pEvent = NULL;
		pEvent = new OpenNetworkEvent( this );
		ME_ASSERTE_RETURN( pEvent, ME_ERROR_OUT_OF_MEMORY );

		return pEvent->Run( CMEThreadManager::Instance()->GetNetworkThread() );
	}

	/* 如果已经open过了, 那么就换一个sink */
	{
		CMELockGuardT<CMELockThreadMutex> Guard( &m_sinkLock );
		m_pSink = pSink;
	}

	return ME_OK;
}

ME_Result CMETransportCrossThread::Close( ME_Result hReason /* = ME_OK */ )
{
	ME_ASSERTE_RETURN( (m_bClosed == FALSE), ME_ERROR_NOT_AVAILABLE );

	{
		CMELockGuardT<CMELockThreadMutex> Guard( &m_sinkLock );
		m_pSink = NULL;
	}

	m_bClosed = TRUE;

	CloseNetworkEvent* pEvent = NULL;
	pEvent = new CloseNetworkEvent( this );
	ME_ASSERTE_RETURN( pEvent, ME_ERROR_OUT_OF_MEMORY );

	return pEvent->Run( CMEThreadManager::Instance()->GetNetworkThread() );
}

ME_Result CMETransportCrossThread::SendData( CMEMessageBlock* pmbBlock )
{
	ME_ASSERTE_RETURN( m_pTransport && pmbBlock, ME_ERROR_NULL_POINTER );
	ME_ASSERTE_RETURN( (m_bClosed == FALSE), ME_ERROR_NOT_AVAILABLE );

	/* 如果在网络线程已经发送失败的情况下，上层再一次尝试发送 */
	if ( TRUE == m_bSendFailed )
	{
		/* 这个时候就需要标识一下，当下一个OnCanSendAgain()来的时候需要通知他 */
		m_bNeedToCallCanSendAgain = TRUE;

		return ME_ERROR_NOT_AVAILABLE;	// 返回错误，让上层等待OnCanSendAgain()
	}

    /** 
     *  @par  持有克隆出来的MB时，对其引用计数+1，在SendData_n发送完时-1，保持MB
     *  @par  引用计数的正确，不会被过早删除
     */
    CMEMessageBlock* pmbBlockCopy = pmbBlock->Clone();
    pmbBlockCopy->AddReference();

	SendDataNetworkEvent* pEvent = NULL;
	pEvent = new SendDataNetworkEvent(
		this,
		pmbBlockCopy );

	ME_ASSERTE_RETURN( pEvent, ME_ERROR_OUT_OF_MEMORY );

	ME_Result hResult = pEvent->Run( CMEThreadManager::Instance()->GetNetworkThread() );

	if ( ME_SUCCEEDED(hResult) )
	{
		pmbBlock->MoveReadPos(
			CMEMessageBlock::DIRECTION_TYPE_FORWORD,
			pmbBlock->GetLength() );
	}

	return hResult;
}

ME_Result CMETransportCrossThread::SetOption( 
	DWORD dwOptionType, 
	void* pOptionValue )
{
	ME_ASSERTE_RETURN( m_pTransport, ME_ERROR_NULL_POINTER );
	ME_ASSERTE_RETURN( (m_bClosed == FALSE), ME_ERROR_NOT_AVAILABLE );

	return m_pTransport->SetOption(
		dwOptionType,
		pOptionValue );
}

ME_Result CMETransportCrossThread::GetOption( 
	DWORD dwOptionType, 
	void*& pOptionValue )
{
	ME_ASSERTE_RETURN( m_pTransport, ME_ERROR_NULL_POINTER );
	ME_ASSERTE_RETURN( (m_bClosed == FALSE), ME_ERROR_NOT_AVAILABLE );

	return m_pTransport->GetOption(
		dwOptionType,
		pOptionValue );
}

ITransportSink* CMETransportCrossThread::GetSink() const
{
	ME_ASSERTE_RETURN( (m_bClosed == FALSE), NULL );

	CMELockGuardT<CMELockThreadMutex> Guard( &const_cast<CMELockThreadMutex&>(m_sinkLock) );
	return m_pSink;
}

DWORD CMETransportCrossThread::AddReference()
{
	return CMEReferenceControlT<MutexType>::AddReference();
}

DWORD CMETransportCrossThread::RemoveReference()
{
	return CMEReferenceControlT<MutexType>::RemoveReference();
}

void CMETransportCrossThread::OnReferenceRelease()
{
	Destroy_u();
}

void CMETransportCrossThread::Destroy_u()
{
	{
		CMELockGuardT<CMELockThreadMutex> Guard( &m_sinkLock );
		m_pSink = NULL;
	}

	CMEMTSafeDestroy::Destroy_u();
}

void CMETransportCrossThread::Destroy_n()
{
	Close_n();

	CMEMTSafeDestroy::Destroy_n();
}

ME_Result CMETransportCrossThread::Open_n()
{
	ME_ASSERTE_RETURN( m_pTransport, ME_ERROR_NULL_POINTER );

	ME_Result hResult = m_pTransport->Open( this );
	if ( ME_FAILED(hResult) )
	{
		m_pTransport->Close( ME_ERROR_NOT_AVAILABLE );

		OnDisconnectUserEvent* pEvent = NULL;
		pEvent = new OnDisconnectUserEvent(
			this,
			ME_ERROR_NOT_AVAILABLE );

		ME_ASSERTE_RETURN( pEvent, ME_ERROR_OUT_OF_MEMORY );

		pEvent->Run( CMEThreadManager::Instance()->GetUserThread() );

		return ME_ERROR_NOT_AVAILABLE;
	}

	return hResult;
}

void CMETransportCrossThread::Close_n()
{
	ME_ASSERTE_RETURN_VOID( m_pTransport );

	m_pTransport->Close();
}

ME_Result CMETransportCrossThread::SendData_n( PayloadType* pmbBlock )
{
	ME_Result hResult = ME_OK;

	/* <pmbBlock>不为空, 说明是从SendData()这个函数中call的，且在之前没有发生过发送失败 */
	if ( pmbBlock && m_DataBuf.empty() )
	{
		/* 如果缓冲区没有数据, 说明前面发送一切正常, 接下来直接发送即可 */
		hResult = m_pTransport->SendData( pmbBlock );
		if ( ME_SUCCEEDED(hResult) )
		{
			/* 如果发送成功就释放, 因为在SendData()中, 这个mb是DeepCopy()得来的 */
			pmbBlock->RemoveReference();
			pmbBlock = NULL;

			return hResult;
		}
	}

	/* 
	* 如果是从SendData()函数call的，且之前有过发送失败 
	*
	* 能到这里说明前面的动作失败，或者，在此之前就有过发送失败
	*/
	if ( pmbBlock )
	{
		
#ifdef ME_WIN
		/**
		* 当返回值为WSA_IO_PENDING时，说明正在使用TransportAsync系列的
		* 异步Transport，由于win32 重叠IO的特性，即便在WSASend()发送失败
		* 而且错误码为WSA_IO_PENDING时，这个包依然会最终发送成功(当然，发送
		* 中途连接断开那是另外一个故事了)，因为系统会保留它并且自动的
		* 持续发送直到全部发送完成，然后送出通知，所以这里不需要缓存
		*
		* 如果缓存并且重发会造成这个包会被发送两次
		*/
		if ( ME_ERROR_IO_PENDING != hResult )
#endif	// ME_WIN
		{
			m_DataBuf.push_back( pmbBlock );
		}

		/* 无论哪一种都要先进队，等待下一次发送 */
		//m_DataBuf.push_back( pmbBlock );

		/* 如果前面的动作失败，而不是之前就有过发送失败 */
		if ( ME_FAILED(hResult) )
		{
			/* 置为TRUE以便于在上层call SendData()的时候可以立即知道发送失败了 */
			m_bSendFailed = TRUE;

			return ME_ERROR_NETWORK_PARTIAL_DATA;
		}
	}

	/** 
	* 以下是在OnCanSendAgain() call的
	*/
	ListItemType pTmpItem;
	while( FALSE == m_DataBuf.empty() )
	{
		pTmpItem = m_DataBuf.front();
		hResult = m_pTransport->SendData( pTmpItem.ParaInOut() /* 用ParaInOut()是因为MB会在SendData时发生改变 */ );
		if ( ME_SUCCEEDED(hResult) )
		{
			m_DataBuf.pop_front();	// pop_front()会触发ListItemType的析构操作，最终会调用RemoveReference()
		}
		else
		{
			m_bSendFailed = TRUE;	// 如果又发送失败，那么就等下一个OnCanSendAgain()

			return hResult;
		}
	}

	m_bSendFailed = FALSE;	// 如果缓存都成功发送了就把标志置一下

	return hResult;
}

void CMETransportCrossThread::OnReceive( 
	ITransport* pTransport, 
	CMEMessageBlock* pmbBlock )
{
	OnReceiveUserEvent* pEvent = NULL;
	pEvent = new OnReceiveUserEvent(
		this,
		CMEAutoPtrT<PayloadType>(pmbBlock->Clone()) );

	ME_ASSERTE_RETURN_VOID( pEvent );

	pEvent->Run( CMEThreadManager::Instance()->GetUserThread() );
}

void CMETransportCrossThread::OnDisconnect( 
	ITransport* pTransport, 
	ME_Result hResult /* = ME_OK */ )
{
	OnDisconnectUserEvent* pEvent = NULL;
	pEvent = new OnDisconnectUserEvent(
		this,
		hResult );

	ME_ASSERTE_RETURN_VOID( pEvent );

	pEvent->Run( CMEThreadManager::Instance()->GetUserThread() );
}

void CMETransportCrossThread::OnCanSendAgain( 
	ITransport* pTransport, 
	ME_Result hReason /* = ME_OK */ )
{
	ME_ASSERTE_RETURN_VOID( ME_SUCCEEDED(hReason) );

	/* 在通知上层之前，首先把缓存里的MB发送完毕 */
	hReason = SendData_n( NULL );
	if ( ME_FAILED(hReason) )
	{
		ME_ASSERTE( FALSE == m_DataBuf.empty() );

		return;
	}

	OnCanSendAgainUserEvent* pEvent = NULL;
	pEvent = new OnCanSendAgainUserEvent( 
		this,
		hReason );

	ME_ASSERTE_RETURN_VOID( pEvent );

	pEvent->Run( CMEThreadManager::Instance()->GetUserThread() );
}

void CMETransportCrossThread::OnReceive_u( PayloadTypeAutoPtr& pmbBlock )
{
	CMELockGuardT<CMELockThreadMutex> Guard( &m_sinkLock );

	/* == NULL要么是从来没有被Open()过，要么就是已经调用了Close() */
	if ( NULL == m_pSink )
	{
		/* 无论是哪种情况，都需要减少引用计数 */
		pmbBlock = NULL;

		ME_WARNING_TRACE_THIS( "transport didn't open yet or has been closed" );

		return;
	}

	m_pSink->OnReceive(
		this,
		pmbBlock.ParaIn() );
}

void CMETransportCrossThread::OnDisconnect_u( ME_Result hReason /* = ME_OK */ )
{
	CMELockGuardT<CMELockThreadMutex> Guard( &m_sinkLock );

	/* <m_pSink>为空就不需要通知 */
	if ( NULL == m_pSink )
	{
		return;
	}

	m_pSink->OnDisconnect(
		this,
		hReason );
}

void CMETransportCrossThread::OnCanSendAgain_u( ME_Result hResult /* = ME_OK */ )
{
	CMELockGuardT<CMELockThreadMutex> Guard( &m_sinkLock );

	/**
	* 如果<m_bNeedToCallCanSendAgain>为真时，说明在网络线程在发送失败之后，上层仍然尝试发送数据
	* 这个时候，从SendData()的返回值就可以得知发送失败，一旦发生这种情况，就需要
	* 通知上层什么时候能够再发送，所以也就有了底下这一段
	*/
	if ( TRUE == m_bNeedToCallCanSendAgain )
	{
		m_bNeedToCallCanSendAgain = FALSE;

		ME_ASSERTE_RETURN_VOID( m_pSink );
		m_pSink->OnCanSendAgain( 
			this,
			hResult );
	}
}



ME_NAME_SPACE_END


