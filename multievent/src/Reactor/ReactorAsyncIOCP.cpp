#include "ReactorAsyncIOCP.h"
#include "AsyncIoResult.h"
#include "Reactor.h"
#include "WakenerAsyncIOCP.h"
#include "SingletonT.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

CMEReactorAsyncIOCP* CMEReactorAsyncIOCP::Instance()
{
	return CMESingletonT<CMEReactorAsyncIOCP>::Instance();
}

CMEReactorAsyncIOCP::CMEReactorAsyncIOCP()
	: m_pWakener( NULL )
	, m_hStatus( CMEReactor::RUN_STATE_NONE )
	, m_hIOCP( NULL )
	, m_dwThreadNum( 0 )

	, m_pEventQueue( NULL )
	, m_pTimerQueue( NULL )
{

}

CMEReactorAsyncIOCP::CMEReactorAsyncIOCP( 
	CMEWakenerAsyncIOCP* pWakener, 
	ME_HANDLE hInterface, 
	DWORD dwThreadNum /* = 0 */ )
	: m_pWakener( pWakener )
	, m_hStatus( CMEReactor::RUN_STATE_NONE )
	, m_hIOCP( hInterface )
	, m_dwThreadNum( dwThreadNum )

	, m_pEventQueue( NULL )
	, m_pTimerQueue( NULL )
{
	m_pWakener->AddReference();
}

CMEReactorAsyncIOCP::~CMEReactorAsyncIOCP()
{
	m_pWakener->RemoveReference();
}

ME_Result CMEReactorAsyncIOCP::Notify( INT iNotifyType )
{
	ME_ASSERTE_RETURN( m_pWakener, ME_ERROR_NULL_POINTER );

	if ( CMEReactor::NOTIFY_TYPE_WAKEUP == iNotifyType )
	{
		return m_pWakener->Notify();
	}

	m_hStatus = CMEReactor::RUN_STATE_EXIT;

	/**
	* Ͷ��<m_dwThreadNum>�����Ļ���IO���������߳�
	* ȫ�����ѣ��Ա����˳�
	*/
	for ( DWORD i = 0; i < m_dwThreadNum; ++i )
	{
		m_pWakener->Notify();
	}

	return ME_OK;
}

ME_Result CMEReactorAsyncIOCP::Register( 
	IMEIO* pIO, 
	IMEIOSink* pIOSink, 
	INT iEvent, 
	INT& iIndex )
{
	ME_HANDLE hResult = ::CreateIoCompletionPort(
		pIO->GetHandle(),
		m_hIOCP,
		(ULONG_PTR)const_cast<ME_HANDLE>(pIO->GetHandle()),
		m_dwThreadNum );

	return NULL != hResult ? ME_OK : ME_ERROR_FAILURE;
}

ME_Result CMEReactorAsyncIOCP::UnRegister( 
	IMEIO* pIO, 
	INT iEvent, 
	INT iIndex )
{
	/**
	* ����IOCP��˵��û�н��������˵����һ��ʹ�ù�::CreateIoCompletionPort()��
	* ĳ����ɹ��󶨾��޷�����󶨡���ֻҪ����������Ͷ��IO�������������ر��˾Ϳ��ԣ���
	*/

	return ME_OK;
}

ME_Result CMEReactorAsyncIOCP::AttachEventQueue( CMEEventQueue* pEq )
{
	ME_ASSERTE_RETURN( pEq, ME_ERROR_INVALID_ARG );

	CMELockGuardT<MutexType> Guard( &m_hQueueMutex );
	ME_ASSERTE_RETURN( !m_pEventQueue, ME_ERROR_ALREADY_INITIALIZED );

	m_pEventQueue = pEq;

	return ME_OK;
}

ME_Result CMEReactorAsyncIOCP::AttachTimerQueue( CMETimerQueue* pTq )
{
	ME_ASSERTE_RETURN( pTq, ME_ERROR_INVALID_ARG );

	CMELockGuardT<MutexType> Guard( &m_hQueueMutex );
	ME_ASSERTE_RETURN( !m_pTimerQueue, ME_ERROR_ALREADY_INITIALIZED );

	m_pTimerQueue = pTq;

	return ME_OK;
}

ME_Result CMEReactorAsyncIOCP::HandleEvent()
{
	DWORD dwBytesTransfered = 0;
	OVERLAPPED* pData = NULL;
	BOOL bResult = FALSE;
	HANDLE lpCompletionKey = NULL;

	bResult = ::GetQueuedCompletionStatus(
		m_hIOCP,
		&dwBytesTransfered,
		(PULONG_PTR)&lpCompletionKey,
		(LPOVERLAPPED*)&pData,
		GetWaitTime() );

	if ( (FALSE == bResult) && (NULL == pData) )
	{
		if ( WAIT_TIMEOUT == ::GetLastError() )
		{
			return ME_ERROR_TIMEOUT;
		}

		return ME_ERROR_FAILURE;
	}
	else
	{
		CMEAsyncIoResult* pIoResult = (CMEAsyncIoResult*)pData;

		pIoResult->OnComplete(
			bResult,
			pIoResult->iIoType,
			m_hIOCP );
	}

	return ME_OK;
}

ME_Result CMEReactorAsyncIOCP::Dispatch( BOOL bExit /* = FALSE */ )
{
    if ( NULL != m_pEventQueue )
    {
        do 
        {
			/**
			* �����첽IO���������ԣ�����ķ����������߳�ͬʱ���У�
			* ��֮��EQ����Ĵ������� - ��ԭ��������<����>ActiveNode
			* �����ʱ���У��������ﲻ�ü���
			*/
            BOOL bEventQueueExit = FALSE;
            m_pEventQueue->HandleEvent( bEventQueueExit );

            if ( bEventQueueExit )
            {
                break;
            }

        } while ( bExit );
    }

	/* ʱ�Ӷ���ͬ�ϴ��������� */
    if ( NULL != m_pTimerQueue )
    {
        if ( FALSE == bExit )
        {
            m_pTimerQueue->Expire();
        }
        else
        {
            m_pTimerQueue->Destroy();
        }
    }

	return m_hStatus;
}

int CMEReactorAsyncIOCP::GetType() const
{
	return CMEReactor::REACTOR_TYPE_ASYCH_IOCP;
}

ME_Result CMEReactorAsyncIOCP::Initialize( 
	ME_HANDLE hInterface, 
	CMEWakenerAsyncIOCP* pWakener, 
	DWORD dwThreadNum )
{
	m_hStatus = CMEReactor::RUN_STATE_NONE;

	if ( m_hIOCP )
	{
		::CloseHandle( m_hIOCP );
	}

	m_hIOCP = hInterface;

	if ( m_pWakener )
	{
		m_pWakener->RemoveReference();
	}

	m_pWakener = pWakener;

	m_pWakener->AddReference();

	m_dwThreadNum = dwThreadNum;

	return ME_OK;
}

CMEReactorAsyncIOCP* CMEReactorAsyncIOCP::operator = ( const CMEReactorAsyncIOCP& targetReactor )
{
	m_pWakener = targetReactor.m_pWakener;
	m_pWakener->AddReference();
	m_hStatus = targetReactor.m_hStatus;

	m_hIOCP = targetReactor.m_hIOCP;
	m_dwThreadNum = targetReactor.m_dwThreadNum;

	m_pEventQueue = targetReactor.m_pEventQueue;
	m_pTimerQueue = targetReactor.m_pTimerQueue;

	return this;
}

UINT CMEReactorAsyncIOCP::GetWaitTime()
{
	UINT nWaitTime = ME_INFINITE;
	do
	{
		/* �¼����в��վ����õȴ�ʱ��Ϊ0 */
		if ( (NULL != m_pEventQueue) && (FALSE == m_pEventQueue->IsEmpty()) )
		{
			nWaitTime = 0;

			break;
		}

		/* �¼����пվͿ�ʱ�Ӷ���������ڵ�ʱ�� */
		if ( NULL != m_pTimerQueue )
		{
			nWaitTime = m_pTimerQueue->GetNextTimeout();
		}

	} while ( FALSE );

	return nWaitTime;
}

ME_NAME_SPACE_END

#endif	// ME_WIN



