#include "AsyncIoAccept.h"
#include "Reactor.h"
#include "InetAddress.h"
#include "OSSocket.h"
#include "SocketTcp.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

char CMEAsyncIoAccept::CMEAsyncIoAcceptResult::s_cBuf = 0;

ME_Result CMEAsyncIoAccept::Post( 
	IMEAsyncIoSink* pSink,
	ME_HANDLE hListenHandle )
{
	ME_ASSERTE_RETURN( pSink, ME_ERROR_INVALID_ARG );

	CMESocketTcp hsSocketAccept;

	/* 预先分配一个有效句柄 */
	ME_Result hResult = hsSocketAccept.Open( TRUE );
	if ( ME_FAILED(hResult) )
	{
		hsSocketAccept.Close();

		return hResult;
	}

	Result* pIoResult = new Result( 
		pSink,
		sizeof(ME_SOCK_ADDR) + ME_MS_ACCEPTEX_FUNC_REMAIN_LENGTH,	/* MSDN要求，必须+16 */
		sizeof(ME_SOCK_ADDR) + ME_MS_ACCEPTEX_FUNC_REMAIN_LENGTH	/* MSDN要求，必须+16 */ );

	pIoResult->hHandle = hsSocketAccept.GetHandle();
	DWORD dwBytes = 0;

	hResult = g_lpfnAcceptEx(
		(ME_SOCKET)hListenHandle,
		(ME_SOCKET)hsSocketAccept.GetHandle(),
		&pIoResult->s_cBuf,				// 不能写NULL
		0,								// 0表示不用接收数据，即，有连接进来就call，如果这个值不为0，那么系统会等到收到第一个数据包才会call
		pIoResult->m_dwLocalAddrLen,	// MSDN要求，必须+16
		pIoResult->m_dwRemoteAddrLen,
		&dwBytes,
		pIoResult );

	if ( (FALSE == hResult) && (WSA_IO_PENDING != ::WSAGetLastError()) )
	{
		hsSocketAccept.Close();
		ME_DELETE( pIoResult );

		return ME_ERROR_FAILURE;
	}

	return ME_OK;
}

void CMEAsyncIoAccept::Cancel( ME_HANDLE hHandle )
{
	::CancelIo( hHandle );
}

CMEAsyncIoAccept::CMEAsyncIoAcceptResult::CMEAsyncIoAcceptResult( 
	IMEAsyncIoSink* pSink, 
	DWORD dwLocalAddrLen, 
	DWORD dwRemoteAddrLen )
	: CMEAsyncIoResult(
		CMEReactor::EVENT_TYPE_ACCEPT,
		pSink )

	, m_dwLocalAddrLen( dwLocalAddrLen )
	, m_dwRemoteAddrLen( dwRemoteAddrLen )
{

}

void CMEAsyncIoAccept::CMEAsyncIoAcceptResult::OnCompleteImpl( 
	BOOL bResult,
	int iActiveEvent,
	ME_HANDLE hInterface )
{
    // 先把结果记下来，在OnInput的过程中再判断返回
    // 01/26/2015 by fangyu
    m_bResult = bResult;

	/**
	* 这里需要确保的是不存在 bResult == FALSE的情况
	* 不然的话，需要关闭CMEAsyncIoAcceptResult::hHandle
	*/
	if ( FALSE == bResult )
	{
		OS::SocketClose( this->hHandle );

        // 这里不能够立刻就返回，否则会发生Acceptor再也不会有IO事件被捕获，要到
        // OnInput中对OnComplete的结果作判断在Post下一个IO请求后再返回
        // 01/26/2015 by fangyu
		//return;
	}

	ME_ASSERTE_RETURN_VOID( m_pAioSink );

	m_pAioSink->OnInput(
		this,
		iActiveEvent );
}

ME_NAME_SPACE_END

#endif	// ME_WIN

