#include "AsyncIoNotify.h"
#include "Reactor.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

ME_Result CMEAsyncIoNotify::Post( 
	IMEAsyncIoSink* pSink, 
	ME_HANDLE hInterface )
{
	ME_ASSERTE_RETURN( pSink && hInterface && (ME_INVALID_HANDLE != hInterface), ME_ERROR_INVALID_ARG );

	Result* pIoResult = new Result(
		pSink,
		hInterface );

	BOOL bResult = ::PostQueuedCompletionStatus(
		hInterface,
		0,
		NULL,
		pIoResult );

	if ( (FALSE == bResult) && (WSA_IO_PENDING != ::WSAGetLastError()) )
	{
		/* ����Ͳ�����hConnectHandle�ˣ�����ʹ�����Լ����� */
		ME_DELETE( pIoResult );

		return ME_ERROR_FAILURE;
	}

	return ME_OK;
}

CMEAsyncIoNotify::CMEAsyncIoNotifyResult::CMEAsyncIoNotifyResult( 
	IMEAsyncIoSink* pSink, 
	ME_HANDLE hInterface )
	: CMEAsyncIoResult(
		CMEReactor::NOTIFY_TYPE_WAKEUP,
		pSink,
		hInterface )
{

}

void CMEAsyncIoNotify::CMEAsyncIoNotifyResult::OnCompleteImpl( 
	BOOL bResult,
	int iActiveEvent,
	ME_HANDLE hInterface )
{
	ME_ASSERTE_RETURN_VOID( m_pAioSink );

	m_pAioSink->OnInput(
		this,
		iActiveEvent );
}

ME_NAME_SPACE_END

#endif	// ME_WIN

