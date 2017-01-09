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

	/* Ԥ�ȷ���һ����Ч��� */
	ME_Result hResult = hsSocketAccept.Open( TRUE );
	if ( ME_FAILED(hResult) )
	{
		hsSocketAccept.Close();

		return hResult;
	}

	Result* pIoResult = new Result( 
		pSink,
		sizeof(ME_SOCK_ADDR) + ME_MS_ACCEPTEX_FUNC_REMAIN_LENGTH,	/* MSDNҪ�󣬱���+16 */
		sizeof(ME_SOCK_ADDR) + ME_MS_ACCEPTEX_FUNC_REMAIN_LENGTH	/* MSDNҪ�󣬱���+16 */ );

	pIoResult->hHandle = hsSocketAccept.GetHandle();
	DWORD dwBytes = 0;

	hResult = g_lpfnAcceptEx(
		(ME_SOCKET)hListenHandle,
		(ME_SOCKET)hsSocketAccept.GetHandle(),
		&pIoResult->s_cBuf,				// ����дNULL
		0,								// 0��ʾ���ý������ݣ����������ӽ�����call��������ֵ��Ϊ0����ôϵͳ��ȵ��յ���һ�����ݰ��Ż�call
		pIoResult->m_dwLocalAddrLen,	// MSDNҪ�󣬱���+16
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
    // �Ȱѽ������������OnInput�Ĺ��������жϷ���
    // 01/26/2015 by fangyu
    m_bResult = bResult;

	/**
	* ������Ҫȷ�����ǲ����� bResult == FALSE�����
	* ��Ȼ�Ļ�����Ҫ�ر�CMEAsyncIoAcceptResult::hHandle
	*/
	if ( FALSE == bResult )
	{
		OS::SocketClose( this->hHandle );

        // ���ﲻ�ܹ����̾ͷ��أ�����ᷢ��Acceptor��Ҳ������IO�¼�������Ҫ��
        // OnInput�ж�OnComplete�Ľ�����ж���Post��һ��IO������ٷ���
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

