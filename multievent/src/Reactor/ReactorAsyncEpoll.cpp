#include "ReactorAsyncEpoll.h"
#include "Reactor.h"
#include "Wakener.h"
#include "AsyncIoResult.h"
#include "AsyncIoAccept.h"
#include "AsyncIoConnect.h"
#include "AsyncIoSend.h"
#include "AsyncIoRecv.h"
#include "AsyncIoNotify.h"
#include "SingletonT.h"

#include <iostream>
using namespace std;

#ifdef ME_LINUX

ME_NAME_SPACE_BEGIN

#define ME_EPOLL_STATIC_MASK (/*EPOLLET | */EPOLLONESHOT)

CMEReactorAsyncEpoll* CMEReactorAsyncEpoll::Instance()
{
	return CMESingletonT<CMEReactorAsyncEpoll>::Instance();
}

CMEReactorAsyncEpoll::CMEReactorAsyncEpoll()
	: m_pWakener( NULL )
	, m_hStatus( CMEReactor::RUN_STATE_NONE )
	, m_hFdEpoll( 0 )
	, m_dwThreadNum( 0 )

	, m_nIndex( 0 )

	, m_pEventQueue( NULL )
	, m_pTimerQueue( NULL )
{

}

CMEReactorAsyncEpoll::CMEReactorAsyncEpoll( 
	IMEWakener* pWakener, 
	ME_HANDLE hFdEpoll, 
	DWORD dwThreadNum /* = 0 */ )
	: m_pWakener( pWakener )
	, m_hStatus( CMEReactor::RUN_STATE_NONE )
	, m_hFdEpoll( hFdEpoll )
	, m_dwThreadNum( dwThreadNum )

	, m_pEventQueue( NULL )
	, m_pTimerQueue( NULL )
{
	RegisterWakener( m_pWakener );
}

CMEReactorAsyncEpoll::~CMEReactorAsyncEpoll()
{

}

ME_Result CMEReactorAsyncEpoll::Notify( INT iNotifyType )
{
	ME_ASSERTE_RETURN( m_pWakener, ME_ERROR_NULL_POINTER );

	if ( CMEReactor::NOTIFY_TYPE_EXIT == iNotifyType )
	{
		m_hStatus = CMEReactor::RUN_STATE_EXIT;
	}

	return m_pWakener->Notify();
}

ME_Result CMEReactorAsyncEpoll::Register( 
	IMEIO* pIO, 
	IMEIOSink* pIOSink, 
	INT iEvent, 
	INT& iIndex ) 
{
	ME_ASSERTE_RETURN( pIO && pIOSink, ME_ERROR_INVALID_ARG );

	CMELockGuardT<MutexType> Guard( &m_hQueueMutex );

	CMEAsyncIoResult* pIoResult = dynamic_cast<CMEAsyncIoResult*>( pIOSink );
	ME_Result hResult = ME_ERROR_FAILURE;

	/* ��ע�� */
	if ( -1 >= iIndex )
	{
		pIoResult->Internal = m_nIndex++;

		hResult = SetEpollEvent( 
			EPOLL_CTL_ADD,
			iEvent,
			pIO->GetHandle(), 
			pIoResult );

		ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

		//iIndex = m_nIndex++;
		iIndex = pIoResult->Internal;

		m_mapHandle.insert( std::make_pair(
			iIndex, 
			pIoResult) );
	}
	else
	{
		MapType::iterator iter = m_mapHandle.find( iIndex );
		ME_ASSERTE_RETURN( iter != m_mapHandle.end(), ME_ERROR_NOT_FOUND );

		hResult = SetEpollEvent(
			EPOLL_CTL_MOD,
	        iEvent,
	        pIO->GetHandle(),
	        pIoResult );
	}

	return hResult;
}

ME_Result CMEReactorAsyncEpoll::UnRegister( 
	IMEIO* pIO, 
	INT iEvent, 
	INT iIndex )
{
	ME_ASSERTE_RETURN( pIO, ME_ERROR_INVALID_ARG );

	CMELockGuardT<MutexType> Guard( &m_hQueueMutex );

	MapType::iterator iter = m_mapHandle.find( iIndex );
	ME_ASSERTE_RETURN( iter != m_mapHandle.end(), ME_OK );

	CMEAsyncIoResult* pIoResult = iter->second;
	ME_ASSERTE( pIoResult );

	ME_Result hResult = ME_ERROR_FAILURE;

	/**
	* ��������Ƴ�����ע�����ͻ���һ��ע����������ͣ�
	* �����ζ������ɾ��
	*/
	if ( ME_BIT_CMP_MASK(iEvent,  pIoResult->iIoType,  pIoResult->iIoType) ) 
	{
		m_mapHandle.erase( iter );

		hResult = SetEpollEvent(
			EPOLL_CTL_DEL,
			iEvent,
			pIoResult->hHandle,
			pIoResult );
	}
	/* ���ֻ���Ƴ�ע�������е�ĳһ���� */
	else
	{
		hResult = SetEpollEvent( 
			EPOLL_CTL_MOD,
			ME_CLR_BITS(pIoResult->iIoType, iEvent),
			pIoResult->hHandle, 
			pIoResult );
	}

	return hResult;
}

ME_Result CMEReactorAsyncEpoll::AttachEventQueue( CMEEventQueue* pEq )
{
	ME_ASSERTE_RETURN( pEq, ME_ERROR_INVALID_ARG );

	CMELockGuardT<MutexType> Guard( &m_hQueueMutex );
	ME_ASSERTE_RETURN( !m_pEventQueue, ME_ERROR_ALREADY_INITIALIZED );

	m_pEventQueue = pEq;

	return ME_OK;
}

ME_Result CMEReactorAsyncEpoll::AttachTimerQueue( CMETimerQueue* pTq )
{
	ME_ASSERTE_RETURN( pTq, ME_ERROR_INVALID_ARG );

	CMELockGuardT<MutexType> Guard( &m_hQueueMutex );
	ME_ASSERTE_RETURN( !m_pTimerQueue, ME_ERROR_ALREADY_INITIALIZED );

	m_pTimerQueue = pTq;

	return ME_OK;
}

ME_Result CMEReactorAsyncEpoll::HandleEvent()
{
	struct epoll_event eeEvent;
	int iResult = ::epoll_wait(
		m_hFdEpoll,
		&eeEvent,
		1,
		GetWaitTime() );

	if ( 1 <= iResult )
	{
		CMEAsyncIoResult* pIoResult = (CMEAsyncIoResult*)eeEvent.data.ptr;

		pIoResult->OnComplete(
			!ME_BIT_ENABLED(eeEvent.events, EPOLLERR),
			eeEvent.events,
			m_hFdEpoll );

		return ME_OK;
	}
	else if ( 0 == iResult )
	{
		return ME_ERROR_TIMEOUT;
	}

	return ME_ERROR_FAILURE;
}

ME_Result CMEReactorAsyncEpoll::Dispatch( BOOL bExit /* = FALSE */ )
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

int CMEReactorAsyncEpoll::GetType() const
{
	return CMEReactor::REACTOR_TYPE_ASYCH_EPOLL;
}

ME_Result CMEReactorAsyncEpoll::Initialize()
{
	m_hStatus = CMEReactor::RUN_STATE_NONE;

	return ME_OK;
}

CMEReactorAsyncEpoll* CMEReactorAsyncEpoll::operator = ( const CMEReactorAsyncEpoll& targetReactor )
{
	m_pWakener = targetReactor.m_pWakener;
	m_hStatus = targetReactor.m_hStatus;

	m_hFdEpoll = targetReactor.m_hFdEpoll;
	m_dwThreadNum = targetReactor.m_dwThreadNum;

	m_mapHandle = targetReactor.m_mapHandle;

	m_pEventQueue = targetReactor.m_pEventQueue;
	m_pTimerQueue = targetReactor.m_pTimerQueue;

	int iIndex = -1;
	Register(
		m_pWakener,
		m_pWakener,
		CMEReactor::EVENT_TYPE_READ,
		iIndex );

	return this;
}

UINT CMEReactorAsyncEpoll::GetWaitTime()
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

ME_Result CMEReactorAsyncEpoll::SetEpollEvent(
	int iOption,
	int iEvent, 
	ME_HANDLE hSocket, 
	CMEAsyncIoResult* pIoResult,
	BOOL bOneShot /* = TRUE */ )
{
	struct epoll_event eeEvent;
	eeEvent.data.ptr = pIoResult;
	eeEvent.events = 0;

	/**
	*  ���¼�����ת������HikUtility�¼�ת��Ϊepollʶ����¼�
	*/
	SetEpollMask(
		iEvent,
		eeEvent,
		bOneShot );

	/*��epollע���¼�*/
	int iResult = ::epoll_ctl( 
		m_hFdEpoll, 
		iOption, 
		hSocket, 
		&eeEvent );

	if ( 0 > iResult )
	{
		return ME_ERROR_FAILURE;
	}

	pIoResult->iIoType = iEvent;
	pIoResult->hHandle = hSocket;

	return ME_OK;
}

void CMEReactorAsyncEpoll::SetEpollMask(
	int iEvent,
	epoll_event& eeEvent, 
	BOOL bOneShot )
{
	int iMask = ME_EPOLL_STATIC_MASK;
	if ( FALSE == bOneShot )
	{
		ME_CLR_BITS( iMask, EPOLLONESHOT );
	}

	if ( ME_BIT_ENABLED(iEvent, CMEReactor::EVENT_TYPE_IN) )
	{
		ME_SET_BITS( eeEvent.events, EPOLLIN | iMask );
	}

	if ( ME_BIT_ENABLED(iEvent, CMEReactor::EVENT_TYPE_OUT) )
	{
		ME_SET_BITS( eeEvent.events, EPOLLOUT | iMask );
	}

	if ( ME_BIT_ENABLED(iEvent, CMEReactor::EVENT_TYPE_ERR) )
	{
		ME_SET_BITS( eeEvent.events, EPOLLERR | iMask );
	}
}

void CMEReactorAsyncEpoll::RegisterWakener( IMEWakener* pWakener )
{
	ME_ASSERTE_RETURN_VOID( pWakener );

	CMELockGuardT<MutexType> Guard( &m_hQueueMutex );

	CMEAsyncIoResult* pIoResult = dynamic_cast<CMEAsyncIoResult*>( pWakener );
	ME_Result hResult = ME_ERROR_FAILURE;

	pIoResult->Internal = m_nIndex++;

	hResult = SetEpollEvent( 
		EPOLL_CTL_ADD,
		CMEReactor::EVENT_TYPE_READ,
		pWakener->GetHandle(), 
		pIoResult,
		FALSE );

	ME_ASSERTE_RETURN_VOID( ME_SUCCEEDED(hResult) );

	int iIndex = pIoResult->Internal;

	m_mapHandle.insert( std::make_pair(
		iIndex, 
		pIoResult) );
}



ME_NAME_SPACE_END


#endif	// ME_LINUX

