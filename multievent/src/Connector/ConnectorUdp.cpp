#include "ConnectorUdp.h"
#include "Thread.h"
#include "ThreadManager.h"
#include "EventAcceptorConnectorT.h"

ME_NAME_SPACE_BEGIN

CMEConnectorUdp::CMEConnectorUdp()
	: m_pSink( NULL )
{

}

CMEConnectorUdp::~CMEConnectorUdp()
{
	Close();
}

void CMEConnectorUdp::SetHandle( ME_HANDLE hHandle )
{
	/** 
	* 由于Connector并不拥有任何句柄，所有这个方法不予执行
	* 
	* <hHandle>是在Open()方法中，由外部传入的
	* 所以说Connector无权改变<hHandle>的句柄
	*/
}

ME_HANDLE CMEConnectorUdp::GetHandle() const
{
	return m_SocketUdp.GetHandle();
}

ME_Result CMEConnectorUdp::Open( 
	IMEConnectorImplSink* pSink, 
	const CMEInetAddress& peerAddress, 
	CMEReactor* pReactor, 
	ME_HANDLE& hHandle, 
	CMEInetAddress* pLocalAddress /* = NULL */ )
{
	ME_ASSERTE_RETURN( pSink && pReactor, ME_ERROR_NULL_POINTER );

	Close();	// 确保不会重复打开

	ME_Result hResult = ME_ERROR_FAILURE;
	if ( pLocalAddress )
	{
		hResult = m_SocketUdp.Open(
			*pLocalAddress,
			TRUE );
	}
	else
	{
		hResult = m_SocketUdp.Open( TRUE );
	}

	/* 不管Open是成功还是失败，都会有新的句柄产生 */
	hHandle = m_SocketUdp.GetHandle();

	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	hResult = m_SocketUdp.Enable( CMEIPBase::NON_BLOCK );
	if ( ME_FAILED(hResult) )
	{
		CMESocketErrorAutoSave errSave;
		ME_ERROR_TRACE_THIS( 
			"failed to set NON_BLOCK to socket"
			<< ", errno = " << errSave.GetLastError()
			<< ", socket handle = " << m_SocketUdp.GetHandle() );

		return hResult;
	}

	/**
	*
	* 把sink赋值上提于NotifySuccess()之前，避免在多线程时产生
	*/
	if ( ME_SUCCEEDED(hResult) )
	{
		m_pSink = pSink;
	}

	/** 
	* 同上上层，但不能在这个函数里通知，需要跳出来通知
	* 以免造成，Open()函数还没结束，OnConnect()就callback了这种不合理的现象
	*/
	NotifySuccess();

	return hResult;
}

void CMEConnectorUdp::Close()
{
	m_pSink = NULL;
	m_SocketUdp.SetHandle( ME_INVALID_HANDLE );
}

void CMEConnectorUdp::NotifySuccess()
{
	/* 这里需要PostEvent来跳出当前函数，之后，再去callback */
	OnConnectEvent* pEvent = new OnConnectEvent( 
		this,
		ME_OK );

	pEvent->Run( CMEThreadManager::Instance()->GetNetworkThread() );
}

void CMEConnectorUdp::OnConnect_c( ME_Result hResult )
{
	ME_ASSERTE_RETURN_VOID( m_pSink );

	m_pSink->OnConnect(
		hResult,
		GetHandle() );
}

ME_NAME_SPACE_END

