#include "SocketBase.h"
#include "OSSocket.h"
#include "InetAddress.h"

ME_NAME_SPACE_BEGIN

CMESocketBase::CMESocketBase()
{

}

CMESocketBase::CMESocketBase( ME_HANDLE hHandle )
	: CMEIPBase( hHandle )
{

}

CMESocketBase::~CMESocketBase()
{

}

ME_Result CMESocketBase::Open( BOOL bReuse /* = FALSE */ )
{
	ME_Result hResult = OpenImpl();
	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	hResult = SetOption(
		SOL_SOCKET,
		SO_REUSEADDR,
		&bReuse,
		sizeof(bReuse) );

	if ( ME_FAILED(hResult) )
	{
		CMESocketErrorAutoSave errAutoSave;

		Close();

		ME_ERROR_TRACE_THIS( "failed to open, error = " << errAutoSave.GetLastError() );

		return SOCKET_ERROR;
	}

	return hResult;
}

ME_Result CMESocketBase::Open( 
	const CMEInetAddress& hiaLocalAddr,
	BOOL bReuse /* = FALSE */ )
{
	ME_Result hResult = Open( bReuse );
	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	return OS::SocketBind( 
		m_hHandle,
		hiaLocalAddr.GetRawAddress() );
}

ME_Result CMESocketBase::Listen( const CMEInetAddress& hiaLocalAddr )
{
	ME_Result hResult = OS::SocketBind(
		m_hHandle,
		hiaLocalAddr.GetRawAddress() );

	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	return OS::SocketListen( m_hHandle );
}

ME_Result CMESocketBase::Accept( 
	ME_HANDLE& hHandle, 
	CMEInetAddress& hiaPeerAddr )
{
	hHandle = OS::SocketAccept(
		m_hHandle,
		*(const_cast<ME_SOCK_ADDR*>(hiaPeerAddr.GetRawAddress())) );

	return ME_INVALID_HANDLE == hHandle ? SOCKET_ERROR : ME_OK;
}

ME_Result CMESocketBase::Close()
{
	CloseImpl();

	if ( (NULL == m_hHandle) ||
		 (ME_INVALID_HANDLE == m_hHandle) )
	{
		return SOCKET_ERROR;
	}

	ME_Result hResult = OS::SocketClose( m_hHandle );

	m_hHandle = ME_INVALID_HANDLE;

	return hResult;
}

int CMESocketBase::Send( 
	const char* pszData, 
	DWORD dwLength, 
	int iFlag /* = 0 */ )
{
	ME_ASSERTE_RETURN( pszData, SOCKET_ERROR );
	ME_ASSERTE_RETURN( (m_hHandle && (ME_INVALID_HANDLE != m_hHandle)), SOCKET_ERROR );

	int iResult = OS::SocketSend(
		(ME_HANDLE)m_hHandle,
		pszData,
		dwLength,
		iFlag );

	ShowSocketError( iResult );

	return iResult;
}

int CMESocketBase::SendV( 
	const iovec pszIov[], 
	DWORD dwCount )
{
	ME_ASSERTE_RETURN( pszIov, SOCKET_ERROR );
	ME_ASSERTE_RETURN( (m_hHandle && (ME_INVALID_HANDLE != m_hHandle)), SOCKET_ERROR );

	int iResult = OS::SocketSendV(
		m_hHandle,
		pszIov,
		dwCount );

	ShowSocketError( iResult );

	return iResult;
}

int CMESocketBase::Recv( 
	char*& pszBuf, 
	DWORD dwLength, 
	int iFlag /* = 0 */ )
{
	ME_ASSERTE_RETURN( (m_hHandle && (ME_INVALID_HANDLE != m_hHandle)), SOCKET_ERROR );

	int iResult = OS::SocketRecv(
		m_hHandle,
		pszBuf,
		dwLength,
		iFlag );

	ShowSocketError( iResult );

	return iResult;
}

int CMESocketBase::RecvV( 
	iovec szIov[], 
	DWORD dwCount )
{
	ME_ASSERTE_RETURN( (m_hHandle && (ME_INVALID_HANDLE != m_hHandle)), SOCKET_ERROR );

	int iResult = OS::SocketRecvV(
		m_hHandle,
		szIov,
		dwCount );

	ShowSocketError( iResult );

	return iResult;
}

ME_Result CMESocketBase::SetOption( 
	int iLevel, 
	int iOption, 
	const void* pOptval, 
	int iOptlen )
{
	ME_ASSERTE_RETURN( (m_hHandle && (ME_INVALID_HANDLE != m_hHandle)), SOCKET_ERROR );

	int iResult = OS::SocketSetSocketOption(
		(ME_HANDLE)m_hHandle,
		iLevel,
		iOption,
		pOptval,
		iOptlen );

	ShowSocketError( iResult );

	return iResult;
}

ME_Result CMESocketBase::GetOption( 
	int iLevel, 
	int iOption, 
	void* pOptval, 
	int* pOptlen )
{
	ME_ASSERTE_RETURN( (m_hHandle && (ME_INVALID_HANDLE != m_hHandle)), SOCKET_ERROR );

	int iResult = OS::SocketGetSocketOption(
		(ME_HANDLE)m_hHandle,
		iLevel,
		iOption,
		pOptval,
		pOptlen );

	ShowSocketError( iResult );

	return iResult;
}

ME_Result CMESocketBase::GetLoacalAddress( CMEInetAddress& hiaLocalAddr )
{
	ME_ASSERTE_RETURN( (m_hHandle && (ME_INVALID_HANDLE != m_hHandle)), SOCKET_ERROR );

	int iResult = OS::SocketGetLocalAddress(
		(ME_HANDLE)m_hHandle,
		*(const_cast<ME_SOCK_ADDR*>(hiaLocalAddr.GetRawAddress())) );

	ShowSocketError( iResult );

	return iResult;
}

ME_Result CMESocketBase::GetRemoteAddress( CMEInetAddress& hiaRemoteAddress )
{
	ME_ASSERTE_RETURN( (m_hHandle && (ME_INVALID_HANDLE != m_hHandle)), SOCKET_ERROR );

	int iResult = OS::SocketGetRemoteAddress(
		(ME_HANDLE)m_hHandle,
		*(const_cast<ME_SOCK_ADDR*>(hiaRemoteAddress.GetRawAddress())) );

	ShowSocketError( iResult );

	return iResult;
}

void CMESocketBase::ShowSocketError( int iResult )
{
	if ( SOCKET_ERROR == iResult )
	{
		CMESocketErrorAutoSave errAutoSave;
		ME_ERROR_TRACE_THIS( "failed" << ", errno = " << errAutoSave.GetLastError() );
	}
}


ME_NAME_SPACE_END



