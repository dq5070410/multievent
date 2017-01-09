#include "AsyncIoSend.h"
#include "Reactor.h"
#include "MessageBlock.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

ME_Result CMEAsyncIoSend::Post( 
	IMEAsyncIoSink* pSink, 
	ME_HANDLE hSendHandle, 
	CMEMessageBlock* pmbBlock )
{
	ME_ASSERTE_RETURN( pSink && pmbBlock, ME_ERROR_INVALID_ARG );
	ME_ASSERTE_RETURN( hSendHandle && (ME_INVALID_HANDLE != hSendHandle), ME_ERROR_INVALID_ARG );

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
		pSink,
		hSendHandle,
		pmbBlock );

	DWORD dwSendBytes = 0;
	DWORD dwFlags = 0;

	/**
	* 加锁是为了获取本次::WSASend()的操作并且成功
	* 给变量赋值，如果没有这把锁，会导致有可能::WSASend()
	* 刚刚返回，还没有到赋值，另外一个线程已经被系统唤醒
	* 去执行OnCompleteImpl()操作，从而引起某些情况下，OnCanSendAgain()
	* 没有能够通知上层
	*/
	pIoResult->m_Lock.Lock();

	ME_Result hResult = ::WSASend(
		(ME_SOCKET)hSendHandle,
		(WSABUF*)szData,
		iVecCount,
		&dwSendBytes,
		0,
		pIoResult,
		NULL );

	/* 存放本次操作的结果，以便于IO完成时做相应的判断 - 需不需要callback OnOutput()，即CanSendAgain */
	pIoResult->m_iLastResult = hResult;
	pIoResult->m_dwLastError = ::WSAGetLastError();

	DWORD dwLastError = pIoResult->m_dwLastError;

	/* 根据已经发送成功 -- 事实上只是进入了tcp/ip的sendbuf里，的长度移动读指针 */
	if ( ME_SUCCEEDED(hResult) || (WSA_IO_PENDING == dwLastError ) )
	{
		pmbBlock->MoveReadPos(
			CMEMessageBlock::DIRECTION_TYPE_FORWORD,
			pmbBlock->GetLength() );
	}

	pIoResult->m_Lock.UnLock();

	if ( ME_FAILED(hResult) )
	{
		/** 
		* 这里不能用pIoResult->m_dwLastError来判断，因为pIoResult有可能在
		* 另一个线程中被删除了，同时，也不能用::WSAGetLastError()来判断，
		* 因为有可能在另一个线程中执行了类似SendData()函数，有可能会使::WSAGetLastError()
		* 被覆盖
		*/
		if ( WSA_IO_PENDING == dwLastError )
		{
			return ME_ERROR_IO_PENDING;
		}
		else
		{
			/**
			* 这里不需要任何callback，因为HikUtility统一是由“读”这个动作
			* 来判定连接是否异常，所以，这块的工作留给其他，比如，CMEAsyncIoRecv
			* 来做
			*
			* 当一个连接也就是transport产生时，recv(读)这个动作是恒定存在的，
			* 一定会先post
			*/
			ME_DELETE( pIoResult );

			return ME_ERROR_NETWORK_SOCKET_CLOSE;
		}
	}
	
	return ME_OK;
}

void CMEAsyncIoSend::Cancel( ME_HANDLE hHandle )
{
	::CancelIo( hHandle );
}

CMEAsyncIoSend::CMEAsyncIoSendResult::CMEAsyncIoSendResult( 
	IMEAsyncIoSink* pSink, 
	ME_HANDLE hRecvHandle, 
	CMEMessageBlock* pmbBlock )
	: CMEAsyncIoResult(
		CMEReactor::EVENT_TYPE_WRITE,
		pSink,
		hRecvHandle )

	, m_pmbBlock( pmbBlock )

	, m_iLastResult( 0 )
	, m_dwLastError( 0 )
{
}

CMEAsyncIoSend::CMEAsyncIoSendResult::~CMEAsyncIoSendResult()
{
}

void CMEAsyncIoSend::CMEAsyncIoSendResult::OnCompleteImpl( 
	BOOL bResult,
	int iActiveEvent,
	ME_HANDLE hInterface )
{
	ME_ASSERTE_RETURN_VOID( m_pAioSink );
	
	/**
	* 只是简单的尝试获取这把锁，如果能获取，那么说明
	* <m_iLastResult>与<m_dwLastError>已经被赋值
	*/
	m_Lock.Lock();
	m_Lock.UnLock();

	/**
	* 仅仅是在上一个IO发送失败且明确是WSA_IO_PENDING(与WSAWOULDBLOCK同一个道理)时
	* 才需要通知上层，以方便上层进行OnCanSendAgain之类的
	* 
	* 其他情况，比如发送成功等等就不用通知了，因为在当时IO投递成功之后就已经能够从
	* CMEAsyncIoSend::Post()函数的返回值直接就知道是成功了
	*/
	if ( (SOCKET_ERROR == m_iLastResult) && (WSA_IO_PENDING == m_dwLastError) )
	{
		m_pAioSink->OnOutput( 
			this,
			iActiveEvent );
	}
}

ME_NAME_SPACE_END

#endif	// ME_WIN









