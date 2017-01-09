#include "AsyncIoConnect.h"
#include "InetAddress.h"
#include "OSSocket.h"
#include "Reactor.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

ME_Result CMEAsyncIoConnect::Post( 
	IMEAsyncIoSink* pSink, 
	ME_HANDLE hConnectHandle, 
	const CMEInetAddress* pPeerAddress, 
	BOOL bIPv4 /* = TRUE */, 
	BOOL bAutoBind /* = TRUE */ )
{
	if ( TRUE == bAutoBind )
	{
		static CMEInetAddress localAddr;
		CMEInetAddress::GetLocalAddress( 
			localAddr,
			bIPv4 );

		ME_Result hResult = OS::SocketBind(
			hConnectHandle,
			localAddr.GetRawAddress() );

		if ( ME_FAILED(hResult) )
		{
			OS::SocketClose( hConnectHandle );

			return hResult;
		}
	}

	Result* pIoResult = new Result(
		pSink,
		hConnectHandle );

	DWORD dwBytes = 0;
	BOOL bResult = g_lpfnConnectEx(
		(ME_SOCKET)hConnectHandle,
		reinterpret_cast<const sockaddr*>(pPeerAddress->GetRawAddress()),
		(ME_SOCK_LEN)sizeof(ME_SOCK_ADDR),
		NULL,
		0,
		&dwBytes,
		pIoResult );

	if ( (FALSE == bResult) && (WSA_IO_PENDING != ::WSAGetLastError()) )
	{
		/* 这里就不处理hConnectHandle了，留给使用者自己决定 */
		ME_DELETE( pIoResult );

		return ME_ERROR_FAILURE;
	}

	return ME_OK;
}

void CMEAsyncIoConnect::Cancel( ME_HANDLE hHandle )
{
	::CancelIo( hHandle );
}

CMEAsyncIoConnect::CMEAsyncIoConnectResult::CMEAsyncIoConnectResult( 
	IMEAsyncIoSink* pSink, 
	ME_HANDLE hConnectHandle )
	: CMEAsyncIoResult(
		CMEReactor::EVENT_TYPE_CONNECT,
		pSink,
		hConnectHandle )
{

}

void CMEAsyncIoConnect::CMEAsyncIoConnectResult::OnCompleteImpl( 
	BOOL bResult,
	int iActiveEvent,
	ME_HANDLE hInterface )
{
	ME_ASSERTE_RETURN_VOID( m_pAioSink );

	/* 如果操作是成功的 -- 同时也意味着连接成功 */
	if ( TRUE == bResult )
	{
		m_pAioSink->OnOutput(
			this,
			iActiveEvent );
	}
	else
	{
		m_pAioSink->OnClose(
			this,
			iActiveEvent );
	}
}

ME_NAME_SPACE_END

#endif	// ME_WIN


