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

	// 下边这段应该是把MB的数据转换成iovec
	int iVecCount = pmbBlock->FillIOVec(
		szData,
		ME_IOV_MAX );

	if ( 0 >= iVecCount )
	{
		ME_ERROR_TRACE_THIS( "can't create iovec" );
		ME_ASSERTE( FALSE );

		/**
		* 这里不能返回错误，或者上层会停止所有发送动作，并等待一个OnCanSendAgain
		* 而在这种情况下，OnCanSendAgain是不会来的
		*
		* 程序能够跑到这个位置，只能说明一个问题：内存用完了
		*/
		return ME_OK;
	}

	int iHaveSend = m_pSocket->SendV(
		szData,
		iVecCount );

	ME_Result hResult = ME_ERROR_FAILURE;
	if( 0 > iHaveSend )
	{
		/* 连接并没有断开，仅仅是因为tcp/ip栈的send buf已经满了 */
		if ( EWOULDBLOCK == errno )
		{
			hResult = pReactor->Register(
				this,
				this,
				CMEReactor::EVENT_TYPE_READ | CMEReactor::EVENT_TYPE_WRITE,
				m_iIndex );

			//cout << "SENDDATA, ++++ register write ++++, result = " << hResult << endl;

			ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

			/* 告诉上层该操作无法立即完成，需要上层存储当前的数据包，等待OnCanSendAgain再发送 */
			return ME_ERROR_WOULD_BLOCK;
		}
		/* 连接已经断开 */
		else
		{
			ME_WARNING_TRACE_THIS( "connection broke" );

			/* 因为Close()函数会把Sink置为NULL，所以这里预先保存 */
			TransportSinkType* pSink = m_pSink;

			Close(
				pReactor,
				ME_ERROR_NETWORK_SOCKET_CLOSE );

			/**
			* 这里不能直接调用pSink->OnDisconnect()，因为这里还是SendData的堆栈里
			* 如果这么做了，就会出现SendData还没有完成，而OnDisconnect被回调了的
			* 不合逻辑的行为，所以，需要Post一个event，让SendData顺利完成之后在
			* 调用OnDisconnect
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

	/* 以下是发送成功 */

	/* 发送了多少 */
	pmbBlock->MoveReadPos(
			CMEMessageBlock::DIRECTION_TYPE_FORWORD,
			iHaveSend ); // 正向移动读指针

	
	if ( 0 < pmbBlock->GetLength() )	// 还有剩余数据，也就是，只发送了一部分
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

		/* 如果还有其他case就继续往下写 */
	}

	/* 如果都不是，那么就让Base来处理 */
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

		/* 如果还有其他case就继续往下写 */
	}

	/* 如果都不是，那么就让Base来处理 */
	return CMETransportBase::GetOption(
		dwOptionType, 
		pOptionValue );
}

DWORD CMETransportTcp::CheckUnReadDataImpl()
{
	/* 如果socket状态正常 */
	if ( ME_INVALID_HANDLE != m_pSocket->GetHandle() )
	{
		int iUnRead = 0;

		/* 检查tcp buffer还有没有残留的未读数据 */
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

	char szBuff[ME_CONNECTION_READ_MAX_SIZE];	// 是否用static类型的char[], 有待测试
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

		/* 如果tcp buffer没有遗留数据 */
		if ( -1 == iHaveReceive )
		{
			break;
		}
		else
		{
			/**
			* 如果读出的数据比<szBuff>的长度要大, 那么读取<ME_CONNECTION_READ_MAX_SIZE>长度
			* 如果不是, 就反过来
			*/
			CMEMessageBlock mbBlock(
				iHaveReceive > ME_CONNECTION_READ_MAX_SIZE ? ME_CONNECTION_READ_MAX_SIZE : iHaveReceive,
				szBuff,
				iHaveReceive > ME_CONNECTION_READ_MAX_SIZE ? ME_CONNECTION_READ_MAX_SIZE : iHaveReceive );

			/* 通知上层 */
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

