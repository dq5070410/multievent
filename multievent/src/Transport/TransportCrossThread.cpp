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

		/* ��һ��open, Postһ��event�������߳�ִ��������open */
		OpenNetworkEvent* pEvent = NULL;
		pEvent = new OpenNetworkEvent( this );
		ME_ASSERTE_RETURN( pEvent, ME_ERROR_OUT_OF_MEMORY );

		return pEvent->Run( CMEThreadManager::Instance()->GetNetworkThread() );
	}

	/* ����Ѿ�open����, ��ô�ͻ�һ��sink */
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

	/* ����������߳��Ѿ�����ʧ�ܵ�����£��ϲ���һ�γ��Է��� */
	if ( TRUE == m_bSendFailed )
	{
		/* ���ʱ�����Ҫ��ʶһ�£�����һ��OnCanSendAgain()����ʱ����Ҫ֪ͨ�� */
		m_bNeedToCallCanSendAgain = TRUE;

		return ME_ERROR_NOT_AVAILABLE;	// ���ش������ϲ�ȴ�OnCanSendAgain()
	}

    /** 
     *  @par  ���п�¡������MBʱ���������ü���+1����SendData_n������ʱ-1������MB
     *  @par  ���ü�������ȷ�����ᱻ����ɾ��
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

	/* <pmbBlock>��Ϊ��, ˵���Ǵ�SendData()���������call�ģ�����֮ǰû�з���������ʧ�� */
	if ( pmbBlock && m_DataBuf.empty() )
	{
		/* ���������û������, ˵��ǰ�淢��һ������, ������ֱ�ӷ��ͼ��� */
		hResult = m_pTransport->SendData( pmbBlock );
		if ( ME_SUCCEEDED(hResult) )
		{
			/* ������ͳɹ����ͷ�, ��Ϊ��SendData()��, ���mb��DeepCopy()������ */
			pmbBlock->RemoveReference();
			pmbBlock = NULL;

			return hResult;
		}
	}

	/* 
	* ����Ǵ�SendData()����call�ģ���֮ǰ�й�����ʧ�� 
	*
	* �ܵ�����˵��ǰ��Ķ���ʧ�ܣ����ߣ��ڴ�֮ǰ���й�����ʧ��
	*/
	if ( pmbBlock )
	{
		
#ifdef ME_WIN
		/**
		* ������ֵΪWSA_IO_PENDINGʱ��˵������ʹ��TransportAsyncϵ�е�
		* �첽Transport������win32 �ص�IO�����ԣ�������WSASend()����ʧ��
		* ���Ҵ�����ΪWSA_IO_PENDINGʱ���������Ȼ�����շ��ͳɹ�(��Ȼ������
		* ��;���ӶϿ���������һ��������)����Ϊϵͳ�ᱣ���������Զ���
		* ��������ֱ��ȫ��������ɣ�Ȼ���ͳ�֪ͨ���������ﲻ��Ҫ����
		*
		* ������沢���ط������������ᱻ��������
		*/
		if ( ME_ERROR_IO_PENDING != hResult )
#endif	// ME_WIN
		{
			m_DataBuf.push_back( pmbBlock );
		}

		/* ������һ�ֶ�Ҫ�Ƚ��ӣ��ȴ���һ�η��� */
		//m_DataBuf.push_back( pmbBlock );

		/* ���ǰ��Ķ���ʧ�ܣ�������֮ǰ���й�����ʧ�� */
		if ( ME_FAILED(hResult) )
		{
			/* ��ΪTRUE�Ա������ϲ�call SendData()��ʱ���������֪������ʧ���� */
			m_bSendFailed = TRUE;

			return ME_ERROR_NETWORK_PARTIAL_DATA;
		}
	}

	/** 
	* ��������OnCanSendAgain() call��
	*/
	ListItemType pTmpItem;
	while( FALSE == m_DataBuf.empty() )
	{
		pTmpItem = m_DataBuf.front();
		hResult = m_pTransport->SendData( pTmpItem.ParaInOut() /* ��ParaInOut()����ΪMB����SendDataʱ�����ı� */ );
		if ( ME_SUCCEEDED(hResult) )
		{
			m_DataBuf.pop_front();	// pop_front()�ᴥ��ListItemType���������������ջ����RemoveReference()
		}
		else
		{
			m_bSendFailed = TRUE;	// ����ַ���ʧ�ܣ���ô�͵���һ��OnCanSendAgain()

			return hResult;
		}
	}

	m_bSendFailed = FALSE;	// ������涼�ɹ������˾Ͱѱ�־��һ��

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

	/* ��֪ͨ�ϲ�֮ǰ�����Ȱѻ������MB������� */
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

	/* == NULLҪô�Ǵ���û�б�Open()����Ҫô�����Ѿ�������Close() */
	if ( NULL == m_pSink )
	{
		/* �������������������Ҫ�������ü��� */
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

	/* <m_pSink>Ϊ�վͲ���Ҫ֪ͨ */
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
	* ���<m_bNeedToCallCanSendAgain>Ϊ��ʱ��˵���������߳��ڷ���ʧ��֮���ϲ���Ȼ���Է�������
	* ���ʱ�򣬴�SendData()�ķ���ֵ�Ϳ��Ե�֪����ʧ�ܣ�һ�������������������Ҫ
	* ֪ͨ�ϲ�ʲôʱ���ܹ��ٷ��ͣ�����Ҳ�����˵�����һ��
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


