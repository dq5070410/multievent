#include "AcceptorAsyncTcp.h"
#include "WakenerAsyncIOCP.h"
#include "InetAddress.h"
#include "Reactor.h"

#include "Thread.h"
#include "ThreadManager.h"
#include "OSSocket.h"

ME_NAME_SPACE_BEGIN

CMEAcceptorAsyncTcp::CMEAcceptorAsyncTcp()
#ifdef ME_LINUX
	: CMEAsyncIoResult(
		CMEReactor::EVENT_TYPE_ACCEPT,
		NULL )

	, m_pRefSink( NULL )
#else	// ME_LINUX

	: m_pRefSink( NULL )
#endif	// ME_LINUX
{
}

CMEAcceptorAsyncTcp::~CMEAcceptorAsyncTcp()
{
}

DWORD CMEAcceptorAsyncTcp::AddReference()
{
	if ( m_pRefSink )
	{
		m_pRefSink->AddReference();
	}

	return CMEReferenceControlT<MutexType>::AddReference();
}

DWORD CMEAcceptorAsyncTcp::RemoveReference()
{
	if ( m_pRefSink )
	{
		m_pRefSink->RemoveReference();
	}

	return CMEReferenceControlT<MutexType>::RemoveReference();
}

#ifdef ME_WIN

ME_Result CMEAcceptorAsyncTcp::Open( 
	IMEAcceptorImplSink* pSink, 
	CMEReactor* pReactor, 
	const CMEInetAddress& hiaLocalAddress )
{
	ME_Result hResult = CMEAcceptorTcp::Open(
		pSink,
		pReactor,
		hiaLocalAddress );

	if ( ME_FAILED(hResult) )
	{
		return hResult;
	}

	return m_asyncAccept.Post(
		this,
		GetHandle() );
}

void CMEAcceptorAsyncTcp::Close( CMEReactor* pReactor )
{
	m_asyncAccept.Cancel( GetHandle() );

	CMEAcceptorTcp::Close( pReactor );
}

void CMEAcceptorAsyncTcp::OnInput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	ME_ASSERTE_RETURN_VOID( m_pSink );

	/**
	* ��һ���첽��AcceptIo�Ѿ����ģ�����Ҫ��Ͷ��һ��
	* ��Ϊ��һ������������������׼��
	*/
	m_asyncAccept.Post(
		this,
		GetHandle() );

	CMEInetAddress hiaLocalAddress;
	CMEInetAddress hiaRemoteAddress;
	CMEAsyncIoAccept::Result* pIoResult = (CMEAsyncIoAccept::Result*)hHandle;

    // ��OnComplete�Ľ����Post�Ժ��жϼ����أ��Ա�֤Reactor��һֱ�ڳ��Բ���
    // Acceptor��IO�¼�
    ME_ASSERTE_RETURN_VOID( TRUE == pIoResult->m_bResult );

	/**
	* ��Ϊ��ʹ��::AcceptEx()����ʱ�������<lpOutputBuffer>�Լ�<lpOutputBufferLength>
	* ����Ϊ0�������Ƿ���::GetAcceptExSockaddrs()������񶼲�Ӱ��<pIoResult->hHandle>
	* ��ʹ�ã����Ǽ���MSDN��ȷ��ʾ��
	*
	* "The GetAcceptExSockaddrs function is used exclusively with the AcceptEx function
	* to parse the first data that the socket receives into local and remote addresses. 
	* You are required to use this function because the AcceptEx function writes address
	* information in an internal (TDI) format."
	*
	* Ϊ��ȷ����ȫû�����⣬���Ի�����update socket info֮ǰ��һ�°�
	*/
	GetPeerAddress( 
		hiaRemoteAddress,
		hiaLocalAddress,
		pIoResult );

	/**
	* ����<pIoResult->hHandle>��Ԥ�ȷ���ģ�������ϵͳ�������ɵ�(���ַ�ʽ��ʹ��::select()
	* ��ΪIO��׽�����������Բ�ͬ����������е�һ����ͬ��)������������ͱ������<hListen>��
	* Ҳ�����������������������<pIoResult->hHandle>����Ȼ���п��ܷ���WSAENOTCONN��������
	* �����ʹ��::shutdown()֮��ĺ�����<pIoResult->hHandle>���в���ʱ�������ͻ�����ˣ�
	* ::shutdown()ʧ����ζ��û��RST����ԶԶ˴���Ͽ������������һ������
	*
	* �򵥵���˵������windows��һ��bug��Ϊ�˱�������ֻ���ֶ�ʹ��::setsockopt()����socket����
	* ����
	*/
	ME_HANDLE hListen = GetHandle();

	OS::SocketSetSocketOption(
		pIoResult->hHandle,
		SOL_SOCKET,
		SO_UPDATE_ACCEPT_CONTEXT,
		&hListen,
		sizeof(hListen) );

	m_pSink->OnConnect(
		ME_OK,
		pIoResult->hHandle,
		&hiaRemoteAddress );
}

void CMEAcceptorAsyncTcp::OnOutput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	ME_ASSERTE( FALSE );
}

void CMEAcceptorAsyncTcp::OnClose( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	ME_ASSERTE( FALSE );
}

ME_Result CMEAcceptorAsyncTcp::GetPeerAddress( 
	CMEInetAddress& hiaPeerAddr, 
	CMEInetAddress& hiaLocalAddr, 
	CMEAsyncIoAccept::Result* pIoResult )
{
	ME_ASSERTE_RETURN( pIoResult, ME_ERROR_INVALID_ARG );

	ME_SOCK_ADDR* pLocalAddr = NULL;
	ME_SOCK_ADDR* pRemoteAddr = NULL;
	int iLocalAddrLen = 0;
	int iRemoteAddrLen = 0;

	g_lpfnGetAcceptExSockaddrs(
		&pIoResult->s_cBuf,
		0,
		pIoResult->m_dwLocalAddrLen,
		pIoResult->m_dwRemoteAddrLen,
		(sockaddr**)&pLocalAddr, 
		&iLocalAddrLen,
		(sockaddr**)&pRemoteAddr,
		&iRemoteAddrLen );

	hiaLocalAddr.Set( *pLocalAddr );
	hiaPeerAddr.Set( *pRemoteAddr );

	return ME_OK;
}

#elif defined( ME_LINUX )

void CMEAcceptorAsyncTcp::OnCompleteImpl( 
	BOOL bResult,
	int iActiveEvent,
	ME_HANDLE hInterface )
{
	AddReference();

	CMEReactor* pReactor = ((CMEThread*)CMEThreadManagerSingleton::Instance()->GetNetworkThread())->GetReactor();
	pReactor->Register(
		this,
		this,
		CMEReactor::EVENT_TYPE_ACCEPT,
		m_iIndex );

	CMEAcceptorTcp::OnInput(
		GetHandle(),
		CMEReactor::EVENT_TYPE_ACCEPT );
}

void CMEAcceptorAsyncTcp::OnDestroy()
{
	RemoveReference();
}

#endif	// ME_WIN

ME_NAME_SPACE_END
