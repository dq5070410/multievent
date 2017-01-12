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
		(CMETransportUdp*)this,
		CMEReactor::EVENT_TYPE_READ,
		m_iIndex );

	m_pSink = pSink;

	/* ����һ��<pSink>��Ϊ��ʹ<pSink>������������TransportAsyncUdp��ͬ */
	m_pRefSink = pSink;
	m_pRefSink->AddReference();

	/* ������Σ���Ͷ��һ�����¼������ڼ�����ӶϿ� */
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

// ���ڵ����ڴ���Ƭ��������⣬���������ע�͵�����ʹ��
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




