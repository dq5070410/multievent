#include "TransportTcp.h"
#include "ConnectionInterface.h"
#include "ConnectionDefines.h"
#include "SocketTcp.h"
#include "Reactor.h"
#include "OSSocket.h"
#include "MessageBlock.h"
#include "EventTransportT.h"
#include "ThreadManager.h"
#include "Thread.h"

#include <iostream>
using namespace std;

ME_NAME_SPACE_BEGIN

CMETransportTcp::CMETransportTcp( ME_HANDLE hHandle )
	: CMETransportBase( IMEConnectionManager::CONNECTION_TYPE_TCP, hHandle )
{

}

CMETransportTcp::~CMETransportTcp()
{
}

ME_Result CMETransportTcp::SendData( 
	CMEReactor* pReactor, 
	CMEMessageBlock* pmbBlock )
{
	ME_ASSERTE_RETURN( pReactor && pmbBlock && GetHandle(), ME_ERROR_NULL_POINTER );
	ME_ASSERTE_RETURN( ME_INVALID_HANDLE != GetHandle(), ME_ERROR_NETWORK_SOCKET_ERROR );
	m_pReactor = pReactor;

	iovec szData[ME_IOV_MAX];

	// �±����Ӧ���ǰ�MB������ת����iovec
	int iVecCount = pmbBlock->FillIOVec(
		szData,
		ME_IOV_MAX );

	if ( 0 >= iVecCount )
	{
		ME_ERROR_TRACE_THIS( "can't create iovec" );
		ME_ASSERTE( FALSE );

		/**
		* ���ﲻ�ܷ��ش��󣬻����ϲ��ֹͣ���з��Ͷ��������ȴ�һ��OnCanSendAgain
		* ������������£�OnCanSendAgain�ǲ�������
		*
		* �����ܹ��ܵ����λ�ã�ֻ��˵��һ�����⣺�ڴ�������
		*/
		return ME_OK;
	}

	int iHaveSend = m_pSocket->SendV(
		szData,
		iVecCount );

	ME_Result hResult = ME_ERROR_FAILURE;
	if( 0 > iHaveSend )
	{
		/* ���Ӳ�û�жϿ�����������Ϊtcp/ipջ��send buf�Ѿ����� */
		if ( EWOULDBLOCK == errno )
		{
			hResult = pReactor->Register(
				this,
				this,
				CMEReactor::EVENT_TYPE_READ | CMEReactor::EVENT_TYPE_WRITE,
				m_iIndex );

			//cout << "SENDDATA, ++++ register write ++++, result = " << hResult << endl;

			ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

			/* �����ϲ�ò����޷�������ɣ���Ҫ�ϲ�洢��ǰ�����ݰ����ȴ�OnCanSendAgain�ٷ��� */
			return ME_ERROR_WOULD_BLOCK;
		}
		/* �����Ѿ��Ͽ� */
		else
		{
			ME_WARNING_TRACE_THIS( "connection broke" );

			/* ��ΪClose()�������Sink��ΪNULL����������Ԥ�ȱ��� */
			TransportSinkType* pSink = m_pSink;

			Close(
				pReactor,
				ME_ERROR_NETWORK_SOCKET_CLOSE );

			/**
			* ���ﲻ��ֱ�ӵ���pSink->OnDisconnect()����Ϊ���ﻹ��SendData�Ķ�ջ��
			* �����ô���ˣ��ͻ����SendData��û����ɣ���OnDisconnect���ص��˵�
			* �����߼�����Ϊ�����ԣ���ҪPostһ��event����SendData˳�����֮����
			* ����OnDisconnect
			*/
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

	/* �����Ƿ��ͳɹ� */

	/* �����˶��� */
	pmbBlock->MoveReadPos(
			CMEMessageBlock::DIRECTION_TYPE_FORWORD,
			iHaveSend ); // �����ƶ���ָ��

	
	if ( 0 < pmbBlock->GetLength() )	// ����ʣ�����ݣ�Ҳ���ǣ�ֻ������һ����
	{
		hResult = pReactor->Register(
			this,
			this,
			CMEReactor::EVENT_TYPE_READ | CMEReactor::EVENT_TYPE_WRITE,
			m_iIndex );

		//cout << "SENDDATA, ==== register write ====, result = " << hResult << endl;

		ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

		return ME_ERROR_NETWORK_PARTIAL_DATA;
	}

	return ME_OK;
}

ME_Result CMETransportTcp::SetOption( 
	DWORD dwOptionType, 
	void* pOptionValue )
{
	ME_Result hResult = ME_ERROR_FAILURE;
	CMESocketTcp* pSocketTcp = NULL;

	switch ( dwOptionType )
	{
	case IMETransport::OPTION_TYPE_TCP_KEEP_AVLIE:
		{
			pSocketTcp = dynamic_cast<CMESocketTcp*>( m_pSocket );
			ME_ASSERTE_RETURN( pSocketTcp, ME_ERROR_NETWORK_SOCKET_ERROR );

			hResult = pSocketTcp->SetOption(
				SOL_SOCKET, 
				SO_KEEPALIVE, 
				pOptionValue, 
				sizeof(DWORD) );

			return SOCKET_ERROR == hResult ? ME_ERROR_NETWORK_SOCKET_ERROR : ME_OK;
		}

		/* �����������case�ͼ�������д */
	}

	/* ��������ǣ���ô����Base������ */
	return CMETransportBase::SetOption(
		dwOptionType,
		pOptionValue );
}

ME_Result CMETransportTcp::GetOption( 
	DWORD dwOptionType, 
	void*& pOptionValue )
{
	ME_Result hResult = ME_ERROR_FAILURE;
	CMESocketTcp* pSocketTcp = NULL;

	switch ( dwOptionType )
	{
	case IMETransport::OPTION_TYPE_TRANSPORT_TYPE:
		{
			*(static_cast<IMEConnectionManager::CONNECTION_TYPE*>(pOptionValue)) = IMEConnectionManager::CONNECTION_TYPE_TCP;

			return ME_OK;
		}

	case IMETransport::OPTION_TYPE_TCP_KEEP_AVLIE:
		{
			int iLength = sizeof(DWORD);
			hResult = m_pSocket->GetOption(
				SOL_SOCKET, 
				SO_KEEPALIVE, 
				pOptionValue, 
				&iLength );

			return SOCKET_ERROR == hResult ? ME_ERROR_NETWORK_SOCKET_ERROR : ME_OK;
		}

		/* �����������case�ͼ�������д */
	}

	/* ��������ǣ���ô����Base������ */
	return CMETransportBase::GetOption(
		dwOptionType, 
		pOptionValue );
}

DWORD CMETransportTcp::CheckUnReadDataImpl()
{
	/* ���socket״̬���� */
	if ( ME_INVALID_HANDLE != m_pSocket->GetHandle() )
	{
		int iUnRead = 0;

		/* ���tcp buffer����û�в�����δ������ */
		if ( (SOCKET_ERROR == OS::IOCtrl((ME_SOCKET)m_pSocket->GetHandle(), FIONREAD, &iUnRead)) ||
			(0 >= iUnRead) )
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}

	char szBuff[ME_CONNECTION_READ_MAX_SIZE];	// �Ƿ���static���͵�char[], �д�����
	char* pszBuff = szBuff;

	int iHaveReceive = 0;
	for ( ; ; )
	{
		::memset( 
			szBuff,
			0,
			ME_CONNECTION_READ_MAX_SIZE );

		iHaveReceive = m_pSocket->Recv(
			pszBuff,
			ME_CONNECTION_READ_MAX_SIZE );

		/* ���tcp bufferû���������� */
		if ( -1 == iHaveReceive )
		{
			break;
		}
		else
		{
			/**
			* ������������ݱ�<szBuff>�ĳ���Ҫ��, ��ô��ȡ<ME_CONNECTION_READ_MAX_SIZE>����
			* �������, �ͷ�����
			*/
			CMEMessageBlock mbBlock(
				iHaveReceive > ME_CONNECTION_READ_MAX_SIZE ? ME_CONNECTION_READ_MAX_SIZE : iHaveReceive,
				szBuff,
				iHaveReceive > ME_CONNECTION_READ_MAX_SIZE ? ME_CONNECTION_READ_MAX_SIZE : iHaveReceive );

			/* ֪ͨ�ϲ� */
			ME_ASSERTE_RETURN( m_pSink, 0 );
			m_pSink->OnReceive( 
				this,
				&mbBlock );
		}
	}

	return 0;
}

int CMETransportTcp::ReceiveImpl( 
	char*& pszData, 
	DWORD dwDataSize )

{
	return m_pSocket->Recv(
		pszData,
		dwDataSize );
}
void CMETransportTcp::OnDisconnect_c( 
	TransportSinkType* pSink,
	ME_Result hReason )
{
	ME_ASSERTE_RETURN_VOID( pSink );
	pSink->OnDisconnect(
		this,
		hReason );
}


ME_NAME_SPACE_END

