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

	/* ���ܳ���UDP���ʹ�С���� */
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
		* ���ﲻ�ܷ��ش��󣬻����ϲ��ֹͣ���з��Ͷ��������ȴ�һ��OnCanSendAgain
		* ������������£�OnCanSendAgain�ǲ�������
		*
		* �����ܹ��ܵ����λ�ã�ֻ��˵��һ�����⣺�ڴ�������
		*/
		return ME_OK;
	}

	// �±߾��Ǻ�tcp��ͬ�ĵط���
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
	* ��Ŀǰ��������(ip send buf��Զ������Ҫ���͵İ�)��UDP�ǲ����ܷ���ʧ��
	* �����ǵ�ַ�����Ͳ��Ϸ�������ʧ�ܣ�����������������һ�ΰɣ��Ա�����
	*/
	ME_ASSERTE( 0 < iHaveSend );

	/* �����˶��� */
	pmbBlock->MoveReadPos(
			CMEMessageBlock::DIRECTION_TYPE_FORWORD,
			iHaveSend ); // �����ƶ���ָ�� 
	
	// �϶��ұ���ȫ���������
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

		/* �����������case�ͼ�������д */
	}

	/* ʣ�µķ��ػ����SetOption���� */
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

		/* �����������case�ͼ�������д */
	}

	return CTransportBase::GetOption(
		dwOptionType, 
		pOptionValue );
}

DWORD CMETransportUdp::CheckUnReadDataImpl()
{
	/* UDP��ȫ������ip��ջ����δ���������˵�� */
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

