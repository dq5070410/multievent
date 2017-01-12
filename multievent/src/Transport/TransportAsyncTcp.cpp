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
	* ���ﲻ����ConnectAsync��AcceptAsync��Open��������ֱ�ӵ�call����
	* ��Open����Ϊtransport����Ǵ�Connector��Acceptorת�ƹ����ģ�
	* ��Acceptor�����Ļ��ã���Ϊ���µľ��������Register��ʱ�򲻻�ʧ�ܣ�
	* ��Connector�����ľͲ����ˣ���Ϊ��ͬһ�����������֮ǰ�Ѿ�ʹ��
	* ::CreateIoCompletionPort�󶨹��������һ�εİ� - call�����Open
	* �ͻ��а󶨵Ķ���������Registerһ����ʧ�ܣ�һ��ʧ�ܾ͵���<m_pSink>
	* <m_pReactor>�ȴӸ���̳й����ĳ�Ա������ֵʧ�ܣ������ս������
	* ������callback��ʱ������<m_pSink>Ϊ�ն��¼��޷�֪ͨ�ϲ�
	*
	* ��ˣ��������ֻ������ʵ��
	*/

	ME_ASSERTE_RETURN( pReactor && pSink, ME_ERROR_NULL_POINTER );
	m_pReactor = pReactor;

	SetBuffSize();

	/**
	* 2014.1.23 added by Τ��
	* ��Ϊ�첽socket
	*/
	ME_Result hResult = m_pSocket->Enable( CMEIPBase::NON_BLOCK );
	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	/* ���жϷ���ֵ����Ϊ�п������ظ��󶨶�ʧ�� */
	pReactor->Register(
		this,
		(CMETransportTcp*)this,
		CMEReactor::EVENT_TYPE_READ,
		m_iIndex );

	m_pSink = pSink;

	/* ����һ��<pSink>��Ϊ��ʹ<pSink>������������TransportAsyncTcp��ͬ */
	m_pRefSink = pSink;

	/* 2014.11.4 edited by Τ�� */
	/* ���ﲻ���������ü�������Ϊ�ᵼ��sink�޷�ɾ���������ڴ�й¶ */
	//m_pRefSink->AddReference();

	/* ������Σ���Ͷ��һ�����¼������ڼ�����ӶϿ� */
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
	* ����һ��Postʧ��(EWOULDBLOCK)������£������ٶ�Ͷ��
	* ME_ASYNC_MAX_POST������һ���̶��ϸ����շ�������(������
	* �����������ڴ�ʹ�������� -- ����Խ������Խ��)
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
	* ���õ������ϱߵĻص������з���һЩ����رա�����֮��
	* ���ܻᵼ�±�����ɾ���������±ߵĵ��û�crash������
	*
	* wrapper�ǻ�㶨��
	*/
	m_pmbBlock->SetReadPos( 0 );
	m_pmbBlock->SetWritePos( m_pmbBlock->GetCapacity() );
	/**
	* ��һ���첽��RecvIo�Ѿ����ģ�����Ҫ��Ͷ��һ��
	* ��Ϊ��һ���������������ݻ����ӶϿ�֪ͨ��׼��
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

// ���ڵ����ڴ���Ƭ��������⣬���������ע�͵�����ʹ��
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
        iHaveSend ); // �����ƶ���ָ��
							   
    if ( 0 < pmbBlock->GetLength() )    // ����ʣ�����ݣ�Ҳ���ǣ�ֻ������һ����
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







