#include "AcceptorUdp.h"
#include "SocketUdp.h"
#include "InetAddress.h"
#include "OSSocket.h"
#include "Reactor.h"

ME_NAME_SPACE_BEGIN

CMEAcceptorUdp::CMEAcceptorUdp()
	: m_pSink( NULL )
	, m_iIndex( -1 )
	, m_pSocketUdp( new CMESocketUdp() )
{

}

CMEAcceptorUdp::~CMEAcceptorUdp()
{
	ME_DELETE( m_pSocketUdp );
}

void CMEAcceptorUdp::SetHandle( ME_HANDLE hHandle )
{
	ME_ASSERTE_RETURN_VOID( hHandle && (ME_INVALID_HANDLE) != hHandle );

	m_pSocketUdp->SetHandle( hHandle );
}

ME_HANDLE CMEAcceptorUdp::GetHandle() const
{
	return m_pSocketUdp->GetHandle();
}

ME_Result CMEAcceptorUdp::Open( 
	IMEAcceptorImplSink* pSink, 
	CMEReactor* pReactor, 
	const CMEInetAddress& hiaLocalAddress )
{
	ME_ASSERTE_RETURN( pSink && pReactor, ME_ERROR_NULL_POINTER );

	ME_Result hResult = m_pSocketUdp->Open( FALSE );
	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	/* 绑定一个本地的地址 */
	hResult = OS::SocketBind(
		m_pSocketUdp->GetHandle(),
		hiaLocalAddress.GetRawAddress() );
	
	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	hResult = pReactor->Register(
		this,
		this,
		CMEReactor::EVENT_TYPE_READ,	// 只有可能有这个事件，其他如close/exception都不可能出现
		m_iIndex );

	if ( ME_FAILED(hResult) )
	{
		Close( pReactor );

		return hResult;
	}

	m_pSink = (IMEAcceptorUdpImplSink*)pSink;
	ME_ASSERTE( m_pSink );

	return hResult;
}

void CMEAcceptorUdp::Close( CMEReactor* pReactor )
{	
	if ( pReactor )
	{
		pReactor->UnRegister(
			this,
			CMEReactor::EVENT_TYPE_READ,
			m_iIndex );
	}

	m_iIndex = -1;
	m_pSink = NULL;

	m_pSocketUdp->Close();
}

void CMEAcceptorUdp::OnInput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
#ifdef ME_DEBUG
	//ME_ASSERTE( hHandle == GetHandle() );
#endif	// ME_DEBUG

	ME_ASSERTE_RETURN_VOID( m_pSink );

	m_pSink->OnInput(
		ME_OK,
		hHandle );
}

void CMEAcceptorUdp::OnOutput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	ME_ASSERTE( FALSE );
}

void CMEAcceptorUdp::OnClose( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	ME_ASSERTE( FALSE );
}


ME_NAME_SPACE_END



