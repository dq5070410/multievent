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
	* 上一个异步的AcceptIo已经消耗，现在要再投递一个
	* 以为下一个即将到来的连接做准备
	*/
	m_asyncAccept.Post(
		this,
		GetHandle() );

	CMEInetAddress hiaLocalAddress;
	CMEInetAddress hiaRemoteAddress;
	CMEAsyncIoAccept::Result* pIoResult = (CMEAsyncIoAccept::Result*)hHandle;

    // 将OnComplete的结果在Post以后判断及返回，以保证Reactor能一直在尝试捕获
    // Acceptor的IO事件
    ME_ASSERTE_RETURN_VOID( TRUE == pIoResult->m_bResult );

	/**
	* 因为在使用::AcceptEx()函数时，其参数<lpOutputBuffer>以及<lpOutputBufferLength>
	* 都置为0，所以是否用::GetAcceptExSockaddrs()函数与否都不影响<pIoResult->hHandle>
	* 的使用，但是鉴于MSDN明确表示：
	*
	* "The GetAcceptExSockaddrs function is used exclusively with the AcceptEx function
	* to parse the first data that the socket receives into local and remote addresses. 
	* You are required to use this function because the AcceptEx function writes address
	* information in an internal (TDI) format."
	*
	* 为了确保完全没有问题，所以还是在update socket info之前用一下吧
	*/
	GetPeerAddress( 
		hiaRemoteAddress,
		hiaLocalAddress,
		pIoResult );

	/**
	* 由于<pIoResult->hHandle>是预先分配的，不是由系统主动生成的(这种方式与使用::select()
	* 作为IO捕捉方法有着明显不同，这就是其中的一个不同点)，所以在这里就必须根据<hListen>，
	* 也就是侦听句柄的属性来更新<pIoResult->hHandle>，不然就有可能发生WSAENOTCONN错误，尤其
	* 是如果使用::shutdown()之类的函数对<pIoResult->hHandle>进行操作时这个错误就会出来了，
	* ::shutdown()失败意味着没有RST，这对对端处理断开的情况可能是一个困扰
	*
	* 简单的来说，这是windows的一个bug，为了避免它，只能手动使用::setsockopt()进行socket属性
	* 更新
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
