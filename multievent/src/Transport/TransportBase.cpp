#include "TransportBase.h"
#include "Reactor.h"
#include "ConnectionInterface.h"
#include "ConnectionDefines.h"
#include "SocketTcp.h"
#include "SocketUdp.h"
#include "MessageBlock.h"

ME_NAME_SPACE_BEGIN

CMETransportBase::CMETransportBase( 
	DWORD dwType, 
	ME_HANDLE hSocketHandle )
	: m_pSink( NULL )
	, m_pReactor( NULL )
	, m_iIndex( -1 )
{
	if ( ME_BIT_ENABLED(dwType, IMEConnectionManager::CONNECTION_TYPE_TCP) )
	{
		m_pSocket = new CMESocketTcp( hSocketHandle );
	}
	else if ( ME_BIT_ENABLED(dwType, IMEConnectionManager::CONNECTION_TYPE_UDP) )
	{
		m_pSocket = new CMESocketUdp( hSocketHandle );
	}
}

CMETransportBase::~CMETransportBase()
{
	Close( m_pReactor );
	ME_DELETE( m_pSocket );
}

ME_Result CMETransportBase::Open( 
	CMEReactor* pReactor, 
	IMETransportImplSink* pSink )
{
	ME_ASSERTE_RETURN( pReactor && pSink, ME_ERROR_NULL_POINTER );
	m_pReactor = pReactor;

	SetBuffSize();

	/**
	* ��Ϊ�첽socket
	*/
	ME_Result hResult = m_pSocket->Enable( CMEIPBase::NON_BLOCK );
	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	hResult = pReactor->Register(
		this,
		this,
		CMEReactor::EVENT_TYPE_READ,
		m_iIndex );

	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	m_pSink = pSink;

	return ME_OK;
}

ME_Result CMETransportBase::Close( 
	CMEReactor* pReactor, 
	ME_Result hReason /* = ME_OK */ )
{
	ME_ASSERTE_RETURN( pReactor, ME_ERROR_NULL_POINTER );
	m_pReactor = pReactor;

	m_pSink = NULL;
	ME_Result hResult = pReactor->UnRegister(
		this,
		CMEReactor::EVENT_TYPE_ALL,
		m_iIndex );

	ME_ASSERTE( ME_SUCCEEDED(hResult) );

	m_iIndex = -1;

	hResult = m_pSocket->Close();
	ME_ASSERTE( ME_SUCCEEDED(hResult) );

	return hResult;
}

ME_Result CMETransportBase::SetOption( 
	DWORD dwOptionType, 
	void* pOptionValue )
{
	ME_Result hResult = ME_ERROR_FAILURE;

	switch ( dwOptionType )
	{
	case IMETransport::OPTION_TYPE_SEND_BUF_LENGTH:
		{
			hResult = m_pSocket->SetOption(
				SOL_SOCKET, 
				SO_SNDBUF, 
				pOptionValue, 
				sizeof(DWORD) );

			return SOCKET_ERROR == hResult ? ME_ERROR_NETWORK_SOCKET_ERROR : ME_OK;
		}

	case IMETransport::OPTION_TYPE_RECEIVE_BUF_LENGTH:
		{
			hResult = m_pSocket->SetOption(
				SOL_SOCKET, 
				SO_RCVBUF, 
				pOptionValue, 
				sizeof(DWORD) );

			return SOCKET_ERROR == hResult ? ME_ERROR_NETWORK_SOCKET_ERROR : ME_OK;
		}

	default :
		{
			ME_ERROR_TRACE_THIS( "unknown type = " << dwOptionType );

			break;
		}
	}

	return ME_ERROR_NOT_FOUND;
}

ME_Result CMETransportBase::GetOption( 
	DWORD dwOptionType, 
	void*& pOptionValue )
{
	ME_Result hResult = ME_ERROR_FAILURE;

	switch ( dwOptionType )
	{
	case IMETransport::OPTION_TYPE_SOCKET_STILL_ALIVE:
		{
			*(static_cast<BOOL*>(pOptionValue)) = ME_INVALID_HANDLE == m_pSocket->GetHandle() ? FALSE : TRUE;

			return ME_OK;
		}

	case IMETransport::OPTION_TYPE_LOCAL_ADDRESS:
		{
			hResult = m_pSocket->GetLoacalAddress( *(static_cast<CMEInetAddress*>(pOptionValue)) );

			return SOCKET_ERROR == hResult ? ME_ERROR_NETWORK_SOCKET_ERROR : ME_OK;
		}

	case IMETransport::OPTION_TYPE_PEER_ADDRESS:
		{
			hResult = m_pSocket->GetRemoteAddress( *(static_cast<CMEInetAddress*>(pOptionValue)) );

			return SOCKET_ERROR == hResult ? ME_ERROR_NETWORK_SOCKET_ERROR : ME_OK;
		}

	case IMETransport::OPTION_TYPE_SEND_BUF_LENGTH:
		{
			int iLength = sizeof(DWORD);
			hResult = m_pSocket->GetOption(
				SOL_SOCKET, 
				SO_SNDBUF, 
				pOptionValue, 
				&iLength );

			return SOCKET_ERROR == hResult ? ME_ERROR_NETWORK_SOCKET_ERROR : ME_OK;
		}

	case IMETransport::OPTION_TYPE_RECEIVE_BUF_LENGTH:
		{
			int iLength = sizeof(DWORD);
			hResult = m_pSocket->GetOption(
				SOL_SOCKET, 
				SO_RCVBUF, 
				pOptionValue, 
				&iLength );

			return SOCKET_ERROR == hResult ? ME_ERROR_NETWORK_SOCKET_ERROR : ME_OK;
		}

	case IMETransport::OPTION_TYPE_GET_SOCKET:
		{
			pOptionValue = m_pSocket;

			return ME_OK;
		}

	default :
		{
			ME_ERROR_TRACE_THIS( "unknown type = " << dwOptionType );

			break;
		}
	}

	return ME_ERROR_NOT_FOUND;
}

void CMETransportBase::SetHandle( ME_HANDLE hHandle )
{
	m_pSocket->SetHandle( hHandle );
}

ME_HANDLE CMETransportBase::GetHandle() const
{
	return m_pSocket->GetHandle();
}

void CMETransportBase::OnInput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
#ifdef ME_DEBUG
	ME_ASSERTE( hHandle == m_pSocket->GetHandle() );

#endif	// ME_DEBUG

	char szBuf[ME_CONNECTION_READ_MAX_SIZE];	// �ǲ���Ҫ���static����Ҫ����
	char* pszBuf = szBuf;

	int iHaveReceive = ReceiveImpl(
		pszBuf,
		ME_CONNECTION_READ_MAX_SIZE );

	/* < 0 ��ʾ�쳣�Ͽ���== 0 ��ʾ�����Ͽ�(Ҳ�ǣ��Է�������close) */
	if ( 0 >= iHaveReceive )
	{
		if ( EWOULDBLOCK == errno )
		{
			return;
		}

		TransportSinkType* pTmpSink = m_pSink;

		ME_ASSERTE_RETURN_VOID( m_pReactor );
		Close( 
			m_pReactor, 
			ME_ERROR_NETWORK_SOCKET_RESET );

		ME_ASSERTE_RETURN_VOID( pTmpSink );
		pTmpSink->OnDisconnect(
			this,
			0 == iHaveReceive ? ME_ERROR_NETWORK_SOCKET_RESET : ME_ERROR_NETWORK_SOCKET_ERROR );

		return;
	}

	/* ������������� */
	/**
	* ������������ݱ�<szBuff>�ĳ���Ҫ��, ��ô��ȡ<ME_CONNECTION_READ_MAX_SIZE>����
	* �������, �ͷ�����
	*/
	CMEMessageBlock mbBlock(
		iHaveReceive > ME_CONNECTION_READ_MAX_SIZE ? ME_CONNECTION_READ_MAX_SIZE : iHaveReceive,
		szBuf,
		iHaveReceive > ME_CONNECTION_READ_MAX_SIZE ? ME_CONNECTION_READ_MAX_SIZE : iHaveReceive );

	ME_ASSERTE_RETURN_VOID( m_pSink );
	m_pSink->OnReceive(
		this,
		&mbBlock );
}

void CMETransportBase::OnOutput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	/* ���callbackֻ����TCP������»���֣�UDP�ǲ�����ֵ� */

	ME_ASSERTE_RETURN_VOID( m_pReactor );
	m_pReactor->UnRegister(
		this,
		CMEReactor::EVENT_TYPE_WRITE,
		m_iIndex );

	ME_ASSERTE_RETURN_VOID( m_pSink );
	m_pSink->OnCanSendAgain(
		this,
		ME_OK );
}

void CMETransportBase::OnClose( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	/**
	* �����selectģ�ͣ���������������
	* ����WSAAsyncSelect��WSAEventSelect֮��ĲŻ����
	* 
	* ����Ŀǰ�����������reactorֻ����selectȥ����
	* �������ﻹ��������Ӧ���룬�Ա�������ܻ���ֵ�
	* �µ�reactor����WSAAsyncSelect�ȵ�
	*/

	CheckUnReadDataImpl();

	TransportSinkType* pTmpSink = m_pSink;

	ME_ASSERTE_RETURN_VOID( m_pReactor );
	Close( 
		m_pReactor, 
		ME_ERROR_NETWORK_SOCKET_CLOSE );

	ME_ASSERTE_RETURN_VOID( pTmpSink );
	pTmpSink->OnDisconnect(
		this,
		ME_ERROR_NETWORK_SOCKET_CLOSE );
}

void CMETransportBase::SetBuffSize()
{
	DWORD dwBufSize = ME_CONNECTION_SEND_BUF_SIZE;
	ME_Result hResult = SetOption(
		IMETransport::OPTION_TYPE_SEND_BUF_LENGTH,
		(void*)&dwBufSize );

	ME_ASSERTE_RETURN_VOID( ME_SUCCEEDED(hResult) );

	dwBufSize = ME_CONNECTION_RECV_BUF_SIZE;
	SetOption(
		IMETransport::OPTION_TYPE_RECEIVE_BUF_LENGTH,
		(void*)&dwBufSize );

	ME_ASSERTE_RETURN_VOID( ME_SUCCEEDED(hResult) );
}

ME_NAME_SPACE_END

