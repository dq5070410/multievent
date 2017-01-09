#include "AsyncIoSendTo.h"
#include "Reactor.h"
#include "MessageBlock.h"
#include "InetAddress.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

ME_Result CMEAsyncIoSendTo::Post( 
	IMEAsyncIoSink* pSink, 
	ME_HANDLE hSendHandle, 
	CMEMessageBlock* pmbBlock, 
	CMEInetAddress* pToAddress )
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
		* ���ﲻ�ܷ��ش��󣬻����ϲ��ֹͣ���з��Ͷ��������ȴ�һ��OnCanSendAgain
		* ������������£�OnCanSendAgain�ǲ�������
		*
		* �����ܹ��ܵ����λ�ã�ֻ��˵��һ�����⣺�ڴ�������
		*/
		return ME_ERROR_OUT_OF_MEMORY;
	}

	Result* pIoResult = new Result(
		pSink,
		hSendHandle,
		pmbBlock,
		pToAddress );

	DWORD dwSendBytes = 0;
	DWORD dwFlags = 0;

	/**
	* ������Ϊ�˻�ȡ����::WSASend()�Ĳ������ҳɹ�
	* ��������ֵ�����û����������ᵼ���п���::WSASend()
	* �ոշ��أ���û�е���ֵ������һ���߳��Ѿ���ϵͳ����
	* ȥִ��OnCompleteImpl()�������Ӷ�����ĳЩ����£�OnCanSendAgain()
	* û���ܹ�֪ͨ�ϲ�
	*/
	pIoResult->m_Lock.Lock();

	ME_Result hResult = ::WSASendTo(
		(ME_SOCKET)hSendHandle,
		(WSABUF*)szData,
		iVecCount,
		&dwSendBytes,
		0,
		reinterpret_cast<sockaddr*>(const_cast<ME_SOCK_ADDR*>(pToAddress->GetRawAddress())),
		pToAddress->GetSize(),
		pIoResult,
		NULL );

	/* ��ű��β����Ľ�����Ա���IO���ʱ����Ӧ���ж� - �費��Ҫcallback OnOutput()����CanSendAgain */
	pIoResult->m_iLastResult = hResult;
	pIoResult->m_dwLastError = ::WSAGetLastError();

	DWORD dwLastError = pIoResult->m_dwLastError;

	/* �����Ѿ����ͳɹ� -- ��ʵ��ֻ�ǽ�����ϵͳ������ĳ����ƶ���ָ�� */
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
		* ���ﲻ����pIoResult->m_dwLastError���жϣ���ΪpIoResult�п�����
		* ��һ���߳��б�ɾ���ˣ�ͬʱ��Ҳ������::WSAGetLastError()���жϣ�
		* ��Ϊ�п�������һ���߳���ִ��������SendData()�������п��ܻ�ʹ::WSAGetLastError()
		* ������
		*/
		if ( WSA_IO_PENDING == dwLastError )
		{
			return ME_ERROR_IO_PENDING;
		}
		else
		{
			/**
			* ���ﲻ��Ҫ�κ�callback����ΪHikUtilityͳһ���ɡ������������
			* ���ж������Ƿ��쳣�����ԣ����Ĺ����������������磬CMEAsyncIoRecv
			* ����
			*
			* ��һ������Ҳ����transport����ʱ��recv(��)��������Ǻ㶨���ڵģ�
			* һ������post
			*/
			ME_DELETE( pIoResult );

			return ME_ERROR_NETWORK_SOCKET_CLOSE;
		}
	}

	return ME_OK;
}

void CMEAsyncIoSendTo::Cancel( ME_HANDLE hHandle )
{
	::CancelIo( hHandle );
}

CMEAsyncIoSendTo::CMEAsyncIoSendToResult::CMEAsyncIoSendToResult( 
	IMEAsyncIoSink* pSink, 
	ME_HANDLE hRecvHandle, 
	CMEMessageBlock* pmbBlock,
	CMEInetAddress* pToAddress )
	: CMEAsyncIoResult(
		CMEReactor::EVENT_TYPE_WRITE,
		pSink,
		hRecvHandle )

	, m_pmbBlock( pmbBlock )
	, m_pToAddress( pToAddress )

	, m_iLastResult( 0 )
	, m_dwLastError( 0 )
{
}

CMEAsyncIoSendTo::CMEAsyncIoSendToResult::~CMEAsyncIoSendToResult()
{
}

void CMEAsyncIoSendTo::CMEAsyncIoSendToResult::OnCompleteImpl( 
	BOOL bResult,
	int iActiveEvent,
	ME_HANDLE hInterface )
{
	ME_ASSERTE_RETURN_VOID( m_pAioSink );

	/**
	* ֻ�Ǽ򵥵ĳ��Ի�ȡ�����������ܻ�ȡ����ô˵��
	* <m_iLastResult>��<m_dwLastError>�Ѿ�����ֵ
	*/
	m_Lock.Lock();
	m_Lock.UnLock();

	/**
	* ����������һ��IO����ʧ������ȷ��WSA_IO_PENDING(��WSAWOULDBLOCKͬһ������)ʱ
	* ����Ҫ֪ͨ�ϲ㣬�Է����ϲ����OnCanSendAgain֮���
	* 
	* ������������緢�ͳɹ��ȵȾͲ���֪ͨ�ˣ���Ϊ�ڵ�ʱIOͶ�ݳɹ�֮����Ѿ��ܹ���
	* CMEAsyncIoSendTo::Post()�����ķ���ֱֵ�Ӿ�֪���ǳɹ���
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










































