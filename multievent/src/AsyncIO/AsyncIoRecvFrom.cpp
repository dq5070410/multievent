#include "AsyncIoRecvFrom.h"
#include "MessageBlock.h"
#include "Reactor.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

ME_Result CMEAsyncIoRecvFrom::Post( 
	IMEAsyncIoSink* pSink, 
	ME_HANDLE hRecvHandle, 
	CMEMessageBlock* pmbBlock,
	CMEInetAddress* pFromAddress )
{
	ME_ASSERTE_RETURN( pSink && pmbBlock && pFromAddress, ME_ERROR_INVALID_ARG );
	ME_ASSERTE_RETURN( hRecvHandle && (ME_INVALID_HANDLE != hRecvHandle), ME_ERROR_INVALID_ARG );

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
		return ME_ERROR_OUT_OF_MEMORY;
	}

	Result* pIoResult = new Result(
		CMEReactor::EVENT_TYPE_READ,
		pSink,
		hRecvHandle,
		pmbBlock,
		pFromAddress );

	DWORD dwBytes = 0;
	DWORD dwFlags = 0;
	
	ME_Result hResult = ::WSARecvFrom(
		(ME_SOCKET)hRecvHandle,
		(WSABUF*)szData,
		iVecCount,
		&dwBytes,
		&dwFlags,
		reinterpret_cast<sockaddr*>(const_cast<ME_SOCK_ADDR*>(pIoResult->m_pFromAddress->GetRawAddress())),
		&pIoResult->m_iFromAddressSize,
		pIoResult,
		NULL );

	if ( ME_FAILED(hResult) && (WSA_IO_PENDING != ::WSAGetLastError()) )
	{
		/**
		* ���ߵ�����һ�㶼��ERROR_NO_MORE_ITEMS(259)��Ҳ�������ӹر���
		*/
		pIoResult->OnComplete(
			FALSE,
			pIoResult->iIoType,
			NULL );

		return hResult;
	}

	return ME_OK;
}

void CMEAsyncIoRecvFrom::Cancel( ME_HANDLE hHandle )
{
	::CancelIo( hHandle );
}

CMEAsyncIoRecvFrom::CMEAsyncIoRecvFromResult::CMEAsyncIoRecvFromResult( 
	int iIoType, 
	IMEAsyncIoSink* pSink, 
	ME_HANDLE hRecvHandle, 
	CMEMessageBlock* pmbBlock,
	CMEInetAddress* pFromAddress )
	: CMEAsyncIoResult(
		iIoType,
		pSink,
		hRecvHandle )

	, m_pmbBlock( pmbBlock )
	, m_pFromAddress( pFromAddress )
	/**
	* ���ﲻ���ָ��Ϸ���
	* 1. �ڲ������е�Post()���������м��
	* 2. pFromAddress������ΪNULL�����ʹ������Ϊĳ���������һ����ָ�����������crash�����ڵ���
	*/
	, m_iFromAddressSize( pFromAddress->GetSize() )
{
	
}

CMEAsyncIoRecvFrom::CMEAsyncIoRecvFromResult::~CMEAsyncIoRecvFromResult()
{
	
}

void CMEAsyncIoRecvFrom::CMEAsyncIoRecvFromResult::OnCompleteImpl( 
	BOOL bResult,
	int iActiveEvent,
	ME_HANDLE hInterface )
{
	ME_ASSERTE_RETURN_VOID( m_pAioSink );

	if ( 0 < this->InternalHigh )
	{
		/**
		* ����OVERLAPPED�ṹ�е�<InternalHigh> - ��Ӧ�ڶ�����������ʾ
		* ����IO���ʱ����ȡ�����ݳ��ȣ��ƶ�MB��дָ�룬�Ա����ϲ�
		* �ܹ���ȷ�Ļ�ȡ����
		*/
		m_pmbBlock->SetWritePos( this->InternalHigh );

		m_pAioSink->OnInput(
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








