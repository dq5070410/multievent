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
		* 这里不能返回错误，或者上层会停止所有发送动作，并等待一个OnCanSendAgain
		* 而在这种情况下，OnCanSendAgain是不会来的
		*
		* 程序能够跑到这个位置，只能说明一个问题：内存用完了
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
		* 能走到这里一般都是ERROR_NO_MORE_ITEMS(259)，也就是连接关闭了
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
	* 这里不检查指针合法性
	* 1. 在操作类中的Post()函数中已有检查
	* 2. pFromAddress不允许为NULL，如果使用者因为某种情况传递一个空指针进来就让它crash，便于调试
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
		* 根据OVERLAPPED结构中的<InternalHigh> - 对应于读操作，它表示
		* 本次IO完成时所读取的数据长度，移动MB的写指针，以便于上层
		* 能够正确的获取数据
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








