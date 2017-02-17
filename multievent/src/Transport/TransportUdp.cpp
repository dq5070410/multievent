#include "TransportUdp.h"
#include "ConnectionInterface.h"
#include "ConnectionDefines.h"
#include "SocketUdp.h"
#include "Reactor.h"
#include "MessageBlock.h"

ME_NAME_SPACE_BEGIN

CMETransportUdp::CMETransportUdp( ME_HANDLE hHandle )
	: CTransportBase( IConnectionManager::CONNECTION_TYPE_UDP, hHandle )
{

}

CMETransportUdp::~CMETransportUdp()
{

}

ME_Result CMETransportUdp::SendData( 
	CMEReactor* pReactor, 
	CMEMessageBlock* pmbBlock )
{
	ME_ASSERTE_RETURN( pReactor && pmbBlock && GetHandle(), ME_ERROR_NULL_POINTER );
	ME_ASSERTE_RETURN( ME_INVALID_HANDLE != GetHandle(), ME_ERROR_NETWORK_SOCKET_ERROR );
	m_pReactor = pReactor;

	/* 不能超过UDP发送大小限制 */
	//ME_ASSERTE_RETURN( ME_CONNECTION_UDP_SEND_MAX_SIZE >= pmbBlock->GetLength(), ME_ERROR_INVALID_ARG );

	iovec szData[ME_IOV_MAX];
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

	// 下边就是和tcp不同的地方了
#ifdef ME_DEBUG
	CMESocketUdp* pSocketUdp = NULL;
	pSocketUdp = dynamic_cast<CMESocketUdp*>( m_pSocket );

	ME_ASSERTE( pSocketUdp );
#endif	// ME_DEBUG

	int iHaveSend = ((CMESocketUdp*)m_pSocket)->SendVTo(
		szData,
		iVecCount,
		m_PeerAddress );

	/** 
	* 在目前的限制下(ip send buf永远大于需要发送的包)，UDP是不可能发送失败
	* 即便是地址根本就不合法都不会失败，不过，还是留下这一段吧，以备不测
	*/
	ME_ASSERTE( 0 < iHaveSend );

	/* 发送了多少 */
	pmbBlock->MoveReadPos(
			CMEMessageBlock::DIRECTION_TYPE_FORWORD,
			iHaveSend ); // 正向移动读指针 
	
	// 肯定且必须全部发送完毕
	ME_ASSERTE( 0 == pmbBlock->GetLength() );

	return ME_OK;
}

ME_Result CMETransportUdp::SetOption( 
	DWORD dwOptionType, 
	void* pOptionValue )
{
	switch ( dwOptionType )
	{
	case ITransport::OPTION_TYPE_PEER_ADDRESS:
		{
			m_PeerAddress = *(static_cast<CMEInetAddress*>(pOptionValue));

			return ME_OK;
		}

		/* 如果还有其他case就继续往下写 */
	}

	/* 剩下的返回基类的SetOption即可 */
	return CTransportBase::SetOption(
		dwOptionType,
		pOptionValue );
}

ME_Result CMETransportUdp::GetOption( 
	DWORD dwOptionType, 
	void*& pOptionValue )
{
	ME_Result hResult = ME_ERROR_FAILURE;
	CMESocketUdp* pSocketUdp = NULL;

	switch ( dwOptionType )
	{
	case ITransport::OPTION_TYPE_PEER_ADDRESS:
		{
			*(static_cast<CMEInetAddress*>(pOptionValue)) = m_PeerAddress;

			return ME_OK;
		}

	case ITransport::OPTION_TYPE_TRANSPORT_TYPE:
		{
			*(static_cast<IConnectionManager::CONNECTION_TYPE*>(pOptionValue)) = IConnectionManager::CONNECTION_TYPE_UDP;

			return ME_OK;
		}

		/* 如果还有其他case就继续往下写 */
	}

	return CTransportBase::GetOption(
		dwOptionType, 
		pOptionValue );
}

DWORD CMETransportUdp::CheckUnReadDataImpl()
{
	/* UDP完全不存在ip堆栈残留未读数据这个说法 */
	return 0;
}

int CMETransportUdp::ReceiveImpl( 
	char*& pszData, 
	DWORD dwDataSize )
{
#ifdef ME_DEBUG
	CMESocketUdp* pSocketUdp = NULL;
	pSocketUdp = dynamic_cast<CMESocketUdp*>( m_pSocket );

	ME_ASSERTE( pSocketUdp );
#endif	// ME_DEBUG

	return ((CMESocketUdp*)m_pSocket)->RecvFrom(
		pszData,
		dwDataSize,
		m_PeerAddress );
}


ME_NAME_SPACE_END

