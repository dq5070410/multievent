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
	* ����Connector����ӵ���κξ�������������������ִ��
	* 
	* <hHandle>����Open()�����У����ⲿ�����
	* ����˵Connector��Ȩ�ı�<hHandle>�ľ��
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

	Close();	// ȷ�������ظ���

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

	/* ����Open�ǳɹ�����ʧ�ܣ��������µľ������ */
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
	* ��sink��ֵ������NotifySuccess()֮ǰ�������ڶ��߳�ʱ����
	*/
	if ( ME_SUCCEEDED(hResult) )
	{
		m_pSink = pSink;
	}

	/** 
	* ͬ���ϲ㣬�����������������֪ͨ����Ҫ������֪ͨ
	* ������ɣ�Open()������û������OnConnect()��callback�����ֲ����������
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
	/* ������ҪPostEvent��������ǰ������֮����ȥcallback */
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

