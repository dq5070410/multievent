#include "SocketUdp.h"
#include "OSSocket.h"
#include "InetAddress.h"

ME_NAME_SPACE_BEGIN

CMESocketUdp::CMESocketUdp()
{

}

CMESocketUdp::CMESocketUdp( ME_HANDLE hHandle )
	: CMESocketBase( hHandle )
{

}

CMESocketUdp::~CMESocketUdp()
{

}

int CMESocketUdp::SendTo( 
	const char* pszData, 
	DWORD dwLength, 
	const CMEInetAddress& hiaPeerAddress, 
	int iFlag /* = 0 */ )
{
	ME_ASSERTE_RETURN( pszData, SOCKET_ERROR );
	ME_ASSERTE_RETURN( (m_hHandle && (ME_INVALID_HANDLE != m_hHandle)), SOCKET_ERROR );

	int iResult = OS::SocketSendTo(
		(ME_HANDLE)m_hHandle,
		pszData,
		dwLength,
		hiaPeerAddress.GetRawAddress(),
		iFlag );

	ShowSocketError( iResult );

	return iResult;
}

int CMESocketUdp::SendVTo( 
	const iovec pszIov[], 
	DWORD dwCount, 
	const CMEInetAddress& hiaPeerAddress )
{
	ME_ASSERTE_RETURN( pszIov, SOCKET_ERROR );
	ME_ASSERTE_RETURN( (m_hHandle && (ME_INVALID_HANDLE != m_hHandle)), SOCKET_ERROR );

	int iResult = OS::SocketSendVTo(
		(ME_HANDLE)m_hHandle,
		pszIov,
		dwCount,
		hiaPeerAddress.GetRawAddress() );

	ShowSocketError( iResult );

	return iResult;
}

int CMESocketUdp::RecvFrom( 
	char*& pszData, 
	DWORD dwLength, 
	CMEInetAddress& hiaPeerAddress, 
	int iFlag /* = 0 */ )
{
	ME_ASSERTE_RETURN( pszData, SOCKET_ERROR );
	ME_ASSERTE_RETURN( (m_hHandle && (ME_INVALID_HANDLE != m_hHandle)), SOCKET_ERROR );

	int iResult = OS::SocketRecvFrom(
		(ME_HANDLE)m_hHandle,
		pszData,
		dwLength,
		*(const_cast<ME_SOCK_ADDR*>(hiaPeerAddress.GetRawAddress())),
		iFlag );

	ShowSocketError( iResult );

	return iResult;
}

ME_Result CMESocketUdp::OpenImpl()
{
	ME_Result hResult = OS::SocketOpen(
		PF_INET,
		SOCK_DGRAM,
		IPPROTO_UDP,
		m_hHandle );

	ShowSocketError( hResult );

	return hResult;
}

ME_Result CMESocketUdp::CloseImpl()
{
	/**
	* UDP与TCP不同，因为没有ACK的机制也不存在什么“buffer中残留的数据” 
	* 所以这里直接返回就好，剩下的事情由基类的Close来做好了
	*/
	return ME_OK;
}

ME_NAME_SPACE_END


















