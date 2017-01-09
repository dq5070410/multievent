#include "AcceptorTcp.h"
#include "Reactor.h"
#include "SocketTcp.h"
#include "InetAddress.h"

ME_NAME_SPACE_BEGIN

CMEAcceptorTcp::CMEAcceptorTcp()
	: m_pSink( NULL )
	, m_iIndex( -1 )
	, m_pSocketTcp( new CMESocketTcp() )
{

}

CMEAcceptorTcp::~CMEAcceptorTcp()
{
	ME_DELETE( m_pSocketTcp );
}

void CMEAcceptorTcp::SetHandle( ME_HANDLE hHandle )
{
	ME_ASSERTE_RETURN_VOID( hHandle && (ME_INVALID_HANDLE) != hHandle );

	m_pSocketTcp->SetHandle( hHandle );
}

ME_HANDLE CMEAcceptorTcp::GetHandle() const
{
	return m_pSocketTcp->GetHandle();
}

ME_Result CMEAcceptorTcp::Open( 
	IMEAcceptorImplSink* pSink, 
	CMEReactor* pReactor, 
	const CMEInetAddress& hiaLocalAddress )
{
	ME_ASSERTE_RETURN( pSink && pReactor, ME_ERROR_NULL_POINTER );

	ME_Result hResult = m_pSocketTcp->Open( FALSE );
	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	/* 先侦听，后注册事件，防止事件丢失 */
	hResult = m_pSocketTcp->Listen( hiaLocalAddress );
	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	hResult = pReactor->Register(
		this,
		(AcceptorType*)this,
		CMEReactor::EVENT_TYPE_ACCEPT,	// 只有可能有这个事件，其他如close/exception都不可能出现
		m_iIndex );

	if ( ME_FAILED(hResult) )
	{
		Close( pReactor );

		return hResult;
	}

	m_pSink = pSink;

	return hResult;
}

void CMEAcceptorTcp::Close( CMEReactor* pReactor )
{
	if ( pReactor )
	{
		pReactor->UnRegister(
		this,
		CMEReactor::EVENT_TYPE_ACCEPT,
		m_iIndex );
	}

	m_iIndex = -1;
	m_pSink = NULL;

	m_pSocketTcp->Close();
}

void CMEAcceptorTcp::OnInput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
#ifdef ME_DEBUG
	ME_ASSERTE( hHandle == GetHandle() );
#endif	// ME_DEBUG

	ME_ASSERTE_RETURN_VOID( m_pSink );

	CMEInetAddress hiaPeerAddr;
	ME_HANDLE hInComingHandle = ME_INVALID_HANDLE;
	ME_Result hResult = m_pSocketTcp->Accept(
		hInComingHandle,
		hiaPeerAddr );

	m_pSink->OnConnect(
		hResult,
		hInComingHandle,
		&hiaPeerAddr );
}

void CMEAcceptorTcp::OnOutput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	ME_ASSERTE( FALSE );
}

void CMEAcceptorTcp::OnClose( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	ME_ASSERTE( FALSE );
}

ME_NAME_SPACE_END


















