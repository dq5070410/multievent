#include "SocketTcp.h"
#include "OSSocket.h"

ME_NAME_SPACE_BEGIN

CMESocketTcp::CMESocketTcp()
{

}

CMESocketTcp::CMESocketTcp( ME_HANDLE hHandle )
	: CMESocketBase( hHandle )
{

}

CMESocketTcp::~CMESocketTcp()
{

}

ME_Result CMESocketTcp::DisableRead()
{
	ME_ASSERTE_RETURN( (m_hHandle && (ME_INVALID_HANDLE != m_hHandle)), SOCKET_ERROR );

	ME_Result hResult = OS::SocketShutdown(
		m_hHandle,
		ME_SD_RECEIVE );

	ShowSocketError( hResult );

	return hResult;
}

ME_Result CMESocketTcp::DisableWrite()
{
	ME_ASSERTE_RETURN( (m_hHandle && (ME_INVALID_HANDLE != m_hHandle)), SOCKET_ERROR );

	ME_Result hResult = OS::SocketShutdown(
		m_hHandle,
		ME_SD_SEND );

	ShowSocketError( hResult );

	return hResult;
}

ME_Result CMESocketTcp::OpenImpl()
{
	ME_Result hResult = OS::SocketOpen(
		PF_INET,
		SOCK_STREAM,
		IPPROTO_TCP,
		m_hHandle );

	ShowSocketError( hResult );

	return hResult;
}

ME_Result CMESocketTcp::CloseImpl()
{
	/* 确保所有残留在tcp buffer中的数据都发送出去 */
	if ( m_hHandle &&
		 (ME_INVALID_HANDLE != m_hHandle) )
	{
		return DisableWrite();
	}

	return SOCKET_ERROR;
}

ME_NAME_SPACE_END









