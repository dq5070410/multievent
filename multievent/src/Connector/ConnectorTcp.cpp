#include "ConnectorTcp.h"
#include "InetAddress.h"
#include "Reactor.h"
#include "SocketTcp.h"
#include "SocketErrorAutoSave.h"
#include "OSSocket.h"
#include "Thread.h"
#include "ThreadManager.h"
#include "EventAcceptorConnectorT.h"

#include <iostream>
using namespace std;

ME_NAME_SPACE_BEGIN

CMEConnectorTcp::CMEConnectorTcp()
	: m_pSink( NULL )
	, m_pReactor( NULL )
	, m_iIndex( -1 )
{

}

CMEConnectorTcp::~CMEConnectorTcp()
{
	Close();
}

void CMEConnectorTcp::SetHandle( ME_HANDLE hHandle )
{
	/** 
	* ����Connector����ӵ���κξ�������������������ִ��
	* 
	* <hHandle>����Open()�����У����ⲿ�����
	* ����˵Connector��Ȩ�ı�<hHandle>�ľ��
	*/
}

ME_HANDLE CMEConnectorTcp::GetHandle() const
{
	return m_SocketTcp.GetHandle();
}

ME_Result CMEConnectorTcp::Open( 
	IMEConnectorImplSink* pSink, 
	const CMEInetAddress& peerAddress, 
	CMEReactor* pReactor,
	ME_HANDLE& hHandle,
	CMEInetAddress* pLocalAddress /* = NULL */ )
{
	ME_ASSERTE_RETURN( pSink && pReactor, ME_ERROR_NULL_POINTER );

	//Close();	// ȷ�������ظ���

	ME_Result hResult = ME_ERROR_FAILURE;
	if ( pLocalAddress )
	{
		hResult = m_SocketTcp.Open(
			*pLocalAddress,
			TRUE );
	}
	else
	{
		hResult = m_SocketTcp.Open( TRUE );
	}

	/* ����Open�ǳɹ�����ʧ�ܣ��������µľ������ */
	hHandle = m_SocketTcp.GetHandle();

	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	hResult = m_SocketTcp.Enable( CMEIPBase::NON_BLOCK );
	if ( ME_FAILED(hResult) )
	{
		CMESocketErrorAutoSave errSave;
		ME_ERROR_TRACE_THIS( 
			"failed to set NON_BLOCK to socket"
			<< ", errno = " << errSave.GetLastError()
			//<< ", socket handle = " << tmpSocket.GetHandle() );
			<< ", socket handle = " << m_SocketTcp.GetHandle() );

		return hResult;
	}

	int iTmpIndex = -1;
	/*hResult = pReactor->Register(
		this,
		this,
		CMEReactor::EVENT_TYPE_CONNECT | CMEReactor::EVENT_TYPE_CLOSE,
		iTmpIndex );

	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );*/

	hResult = Connect(
		&m_SocketTcp,
		peerAddress,
		pReactor,
		iTmpIndex );

	/* �������ϻ�֪���ӽ�� */
	if ( EWOULDBLOCK == hResult )
	{
		hResult = ME_OK;	// �����ϲ㱾�β����ɹ�
	}
	/* ���亱��������£��������첽��socketҲ�п���һconnect�ͳɹ� */
	else if ( ME_SUCCEEDED(hResult) )
	{
		/** 
		* ͬ���ϲ㣬�����������������֪ͨ����Ҫ������֪ͨ
		* ������ɣ�Open()������û������OnConnect()��callback�����ֲ����������
		*/
		NotifySuccess();
	}

	if ( ME_SUCCEEDED(hResult) )
	{
		m_pSink = pSink;
		m_pReactor = pReactor;

		hResult = pReactor->Register(
        	this,
	        this,
    	    CMEReactor::EVENT_TYPE_CONNECT | CMEReactor::EVENT_TYPE_CLOSE,
        	iTmpIndex );

		ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

		m_iIndex = iTmpIndex;
	}

	/* Connect()ʧ�ܻ�EWOULDBLOCK */
	return hResult;
}

void CMEConnectorTcp::Close()
{
	if ( m_pReactor )
	{
		m_pReactor->UnRegister( 
			this,
			CMEReactor::EVENT_TYPE_ALL,
			m_iIndex );

		m_pReactor = NULL;
	}

	m_pSink = NULL;
	m_SocketTcp.SetHandle( ME_INVALID_HANDLE );
	m_iIndex = -1;
}

ME_Result CMEConnectorTcp::Connect( 
	CMESocketTcp* pSocket, 
	const CMEInetAddress& peerAddr,
	CMEReactor* pReactor,
	int iIndex )
{
	ME_ASSERTE_RETURN( pSocket && pReactor, ME_ERROR_NULL_POINTER );

	ME_Result hResult = OS::SocketConnect(
		pSocket->GetHandle(),
		peerAddr.GetRawAddress() );

	if ( SOCKET_ERROR == hResult )
	{
		CMESocketErrorAutoSave errSave;
		if ( EWOULDBLOCK != errSave.GetLastError() )
		{
			pReactor->UnRegister(
				this,
				CMEReactor::EVENT_TYPE_ALL,
				iIndex );

			ME_ERROR_TRACE_THIS( 
				"failed to connect"
				<< ", address you try to reach = " << peerAddr.GetIP()
				<< ", errno = " << errSave.GetLastError()
				<< ", socket handle = " << pSocket->GetHandle() );

			return hResult;
		}

		return EWOULDBLOCK;	// = EWOULDBLOCK��ζ�Ų������ϻ�֪�������ô���ȴ�reactor��callback�ͺ���
	}

	/**
	* ���亱��������£��������첽��socketҲ�п���һconnect�ͳɹ�
	* ��..Ҳ�ǳɹ�����Ҫ֪ͨ�ϲ�OnConnect
	*/
	return ME_OK;
}

void CMEConnectorTcp::NotifySuccess()
{
	/* ������ҪPostEvent��������ǰ������֮����ȥcallback */
	OnConnectEvent* pEvent = new OnConnectEvent( 
		this,
		ME_OK );

	pEvent->Run( CMEThreadManager::Instance()->GetNetworkThread() );
}

void CMEConnectorTcp::OnInput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	/* �����ܷ��� */
	ME_ASSERTE( FALSE );
}

void CMEConnectorTcp::OnOutput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
#ifdef ME_DEBUG
	ME_ASSERTE( hHandle && (hHandle == m_SocketTcp.GetHandle()) );
	ME_ASSERTE( ME_BIT_ENABLED(iEvent, CMEReactor::EVENT_TYPE_CONNECT) );
#endif	// ME_DEBUG

	ME_ASSERTE_RETURN_VOID( m_pSink );
	ConnectorSinkType* pTmpSink = m_pSink;

	Close();

	pTmpSink->OnConnect(
		ME_OK,
		hHandle );
}

void CMEConnectorTcp::OnClose( 
	ME_HANDLE hHandle, 
	int iEvent )
{
#ifdef ME_DEBUG
	ME_ASSERTE( hHandle && (hHandle == m_SocketTcp.GetHandle()) );
	ME_ASSERTE( ME_BIT_ENABLED(iEvent, CMEReactor::EVENT_TYPE_CLOSE) );
#endif	// ME_DEBUG

	ME_ASSERTE_RETURN_VOID( m_pSink );
	ConnectorSinkType* pTmpSink = m_pSink;

	Close();

	pTmpSink->OnConnect(
		ME_ERROR_NETWORK_CONNECT_ERROR,
		hHandle );
}

void CMEConnectorTcp::OnConnect_c( ME_Result hResult )
{
	/* �����������ӳɹ������������ */
	OnOutput( 
		GetHandle(),
		CMEReactor::EVENT_TYPE_CONNECT );
}

ME_NAME_SPACE_END

