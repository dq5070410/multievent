/**
 *  �¼���·������ʵ�ֻ����ඨ��
 *
 *
 *  History
 *  01/30/2016 create file
 */

#include "Reactor.h"
#include "ReactorBase.h"
#include "LockGuardT.h"
#include "WakenerSocketPair.h"
#include "WakenerEvent.h"
#include "ConnectionDefines.h"

ME_NAME_SPACE_BEGIN

CMEReactorBase::CMEReactorBase(
    IMEWakener* pWakener,
    INT iType )
    : m_iActiveIOItemNum( 0 )
    , m_pWakener( pWakener )
    , m_iNewIndex( 0 )
    , m_iStatus( CMEReactor::RUN_STATE_RUNNING )
    , m_iType( iType )
    , m_pEventQueue( NULL )
    , m_pTimerQueue( NULL )
{

}

CMEReactorBase::~CMEReactorBase()
{
}

ME_Result CMEReactorBase::Notify( INT iNotifyType )
{
    ME_INFO_TRACE_THIS( "iNotifyType: " << iNotifyType );
    ME_ASSERTE_RETURN( (NULL != m_pWakener), ME_ERROR_NULL_POINTER );

    //ME_Result hResult = ME_ERROR_FAILURE;

    /* ����ģʽ */
    if ( CMEReactor::NOTIFY_TYPE_WAKEUP == iNotifyType )
    {
        return m_pWakener->Notify();
    }

    /* ֹͣģʽ */
    /* ������ô����������ֹͣ״̬ */
    m_iStatus = CMEReactor::RUN_STATE_EXIT;

    return m_pWakener->Notify();    // ����Reactor
}

ME_Result CMEReactorBase::Register(
    IMEIO* pIO,
    IMEIOSink* pIOSink,
    INT iEvent,
    INT& iIndex )
{
    ME_INFO_TRACE_THIS( "pIO: " << pIO
        << ", pIOSink: " << pIOSink
        << ", iEvent: " << iEvent
        << ", iIndex: " << iIndex );

    ME_ASSERTE_RETURN( (ME_INVALID_HANDLE != pIO->GetHandle()), ME_ERROR_INVALID_ARG );

    CMELockGuardT<CMELockThreadMutex> hGuard( &m_hThreadMutex );

    ME_Result hResult = ME_ERROR_FAILURE;

    if ( -1 != iIndex )
    {
        HikIOItemMap::iterator iterIOItem = m_ioItemMap.find( iIndex );
        ME_ASSERTE_RETURN( (m_ioItemMap.end() != iterIOItem), ME_ERROR_NOT_FOUND );

        /* ͨ��ע��ֻ�����޸�Event���ͣ������ͼ�޸�IO�������IO Sink���󣬶����� */
        ME_ASSERTE_RETURN( (iterIOItem->second.pIO == pIO), ME_ERROR_NOT_FOUND );
        ME_ASSERTE_RETURN( (iterIOItem->second.pIOSink == pIOSink), ME_ERROR_FOUND );

        hResult = AddWaitableObject(
            pIO,
            iEvent,
            iterIOItem->first );

        /* ����ɹ������޸�IO Item�����ٻ��� */
        if ( ME_SUCCEEDED(hResult) )
        {
            iterIOItem->second.iEvent = iEvent;

            /* modify by fy, 2013/05/13, fix bug #BG00B0038 */
            m_pWakener->Notify();
            /* modify by fy end */
        }

        iIndex = iterIOItem->first;
    }
    /* iIndexָ��Ϊ-1���������� */
    else
    {
        if ( IsFull() )
        {
            return ME_ERROR_REACH_MAX;
        }

        /* ��һ���µ�Index�Ժ��ٵ���AddWaitableObject */
        iIndex = m_iNewIndex++;
        hResult = AddWaitableObject(
            pIO,
            iEvent,
            iIndex );

        if ( ME_SUCCEEDED(hResult) )
        {
            /* ��ʼ��io item��Ŀǰֻ��������Ż�����IOSink */
            ME_IO_ITEM stIOItem;
            stIOItem.pIO = pIO;
            stIOItem.pIOSink = pIOSink;
            stIOItem.iEvent = iEvent;
            stIOItem.iActiveEvent = CMEReactor::EVENT_TYPE_NULL;
            /* ����io item�� */
            m_ioItemMap.insert( std::make_pair(
                iIndex,     // fix��iEvent->iIndex
                stIOItem) );

            m_pWakener->Notify();
        }
        else
        {
            iEvent = -1;    //lint -e438 iEvent is Output Parameter
        }
    }
    
    return hResult;
}

ME_Result CMEReactorBase::UnRegister(
    IMEIO* pIO,
    INT iEvent,
    INT iIndex )
{
    ME_INFO_TRACE_THIS( "pIO: " << pIO
        << ", iEvent: " << iEvent
        << ", iIndex: " << iIndex );

    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    ME_Result hResult = ME_ERROR_NOT_FOUND;

    /* �����ṩ��Index���� */
    HikIOItemMap::iterator iterIOItem = m_ioItemMap.find( iIndex );
    if ( m_ioItemMap.end() == iterIOItem )
    {
        /* Ŀǰ�����ע����ʱ���Ҳ������IO Item����ô����Ϊ�Ѿ��ɹ��� */
        ME_WARNING_TRACE_THIS( "Unregister but not found io item, index: " << iIndex );

        return ME_OK;
    }

    ME_ASSERTE_RETURN( (iterIOItem->second.pIO == pIO), ME_ERROR_FAILURE );

    /**
     *  ��ΪĿǰ��·������Ƶȴ���Ҳ�Ǹ��ϵ��¼����ͣ�ȡ��ʱ��Ҫ����һ���Ĵ����
     *  ��ȡ��������߹��٣����⻹��Ҫ����select��WaitForMultipleObjects�����ԣ�
     *  �����������ߵ�RemoveWaitableObject�ӿڣ����������ʵ�ֺ��п��ܻ��ں�����
     *  �������з����ı䣬��Ϊ������Ҫ���м����ԵĿ������ÿ����Ƿ��и��õĽ����
     *  ��
     */

    /* ���¼����ͽ���һ����Ԥ���� */
    INT iResultEvent( 0 );
    INT iRemoveEvent( 0 );
    ME_SET_BITS( iResultEvent, (iterIOItem->second.iEvent & ~((DWORD)iEvent)) );

    /* ��������������Ҫ����RemoveWaitableObject���û�ȡ������Щ���� */
    ME_SET_BITS( iRemoveEvent, iEvent );

    /* �������ע�ɶ��Լ����ܿͻ��˵����ͣ���ô��Ҫ����Ϊ���������Ͷ�ȡ�� */
    /* �������������֪�����ָ��������Ѿ���ȫ����Ҫ��ע�� */
    if ( ME_BIT_DISABLED(iResultEvent, CMEReactor::EVENT_TYPE_IN) )
    {
        ME_SET_BITS( iRemoveEvent, CMEReactor::EVENT_TYPE_IN );
    }

    if ( ME_BIT_DISABLED(iResultEvent, CMEReactor::EVENT_TYPE_OUT) )
    {
        ME_SET_BITS( iRemoveEvent, CMEReactor::EVENT_TYPE_OUT );
    }

    if ( ME_BIT_DISABLED(iResultEvent, CMEReactor::EVENT_TYPE_ERR) )
    {
        ME_SET_BITS( iRemoveEvent, CMEReactor::EVENT_TYPE_ERR );
    }

    /**
     *  ��������RemoveWaitableObject��Ҫ���ǣ����ĳһ��������ʱ�������ֶ�Ҫ
     *  ��ȡ��ʱ��������ȡ�� 
     */
    hResult = RemoveWaitableObject(
        pIO,
        iRemoveEvent,
        iterIOItem->first );

    /* ����ɹ��Ÿı�IO Item */
    if ( ME_SUCCEEDED(hResult) )
    {
        /* Ԥ���Ƿ����ȫȡ�� */
        if ( ME_BIT_CMP_MASK(iterIOItem->second.iEvent, iEvent, iterIOItem->second.iEvent) )
        {
            m_ioItemMap.erase( iterIOItem );
        }
        else
        {
            ME_CLR_BITS( iterIOItem->second.iEvent, (DWORD)iEvent );
        }
    }

    return hResult;
}

ME_Result CMEReactorBase::AttachEventQueue( CMEEventQueue* pEq )
{
    ME_INFO_TRACE_THIS( "pEq: " << pEq );
    ME_ASSERTE_RETURN( (NULL == m_pEventQueue), ME_ERROR_FOUND );

    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );
    ME_ASSERTE_RETURN( (NULL == m_pEventQueue), ME_ERROR_FOUND );
    m_pEventQueue = pEq;

    return ME_OK;
}

ME_Result CMEReactorBase::AttachTimerQueue( CMETimerQueue* pTq )
{
    ME_INFO_TRACE_THIS( "pTq: " << pTq );
    ME_ASSERTE_RETURN( (NULL == m_pTimerQueue), ME_ERROR_FOUND );

    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );
    ME_ASSERTE_RETURN( (NULL == m_pTimerQueue), ME_ERROR_FOUND );
    m_pTimerQueue = pTq;

    return ME_OK;
}

ME_Result CMEReactorBase::Dispatch( BOOL bExit /* = FALSE */ )
{
    ME_INFO_TRACE_THIS( "bExit: " << bExit );
    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    /** 
     *  IO�¼��ص���������Ҫһ����ʱ�Ŀռ���������з����¼���IO���󣬷����ڻ�
     *  �����������������ע������ô�Ʊػ����IO Item Map�ı仯�����е�Iterator
     *  ����ʧЧ��Ȼ����Ǳ���
     *
     */

    std::vector<ME_IO_ITEM*> vecActiveIOItem;
    std::vector<ME_IO_ITEM*>::iterator iterActiveIOItem;

    /* �Ȱѷ����¼���IO�����ҳ�������Active����Ҳ����飬���ٱ������� */
    HikIOItemMap::iterator iterIOItem = m_ioItemMap.begin();
    for ( ; iterIOItem != m_ioItemMap.end() && m_iActiveIOItemNum > 0; ++iterIOItem )
    {
        if ( iterIOItem->second.iActiveEvent )
        {
            vecActiveIOItem.push_back( &(iterIOItem->second) );
            /* Active������1 */
            --m_iActiveIOItemNum;
        }
    }

    /* �ٶ����з����¼���IO������лص� */
    for ( iterActiveIOItem = vecActiveIOItem.begin();
          iterActiveIOItem != vecActiveIOItem.end();
          ++iterActiveIOItem )
    {
        /** 
         *  �ڻص�ǰ�������ã�WaitForMultipleObjects��Select�����¼����͵ķ�ʽ��
         *  ͬ��ǰ�߲���������е�io item�Զ���ɷ���ǰ���ã����߿��ԣ��������
         *  �б�Ҫ��������Ĺ���
         *  ������ME_SET_BITS��ȥ����0
         */
        INT iActiveEvent = (*iterActiveIOItem)->iActiveEvent;
        ME_CLR_BITS( (*iterActiveIOItem)->iActiveEvent, CMEReactor::EVENT_TYPE_ALL );

        if ( ME_BIT_ENABLED(iActiveEvent, CMEReactor::EVENT_TYPE_IN) )
        {
            (*iterActiveIOItem)->pIOSink->OnInput(
                (*iterActiveIOItem)->pIO->GetHandle(),
                iActiveEvent );
        }

        if ( ME_BIT_ENABLED(iActiveEvent, CMEReactor::EVENT_TYPE_OUT) )
        {
            (*iterActiveIOItem)->pIOSink->OnOutput(
                (*iterActiveIOItem)->pIO->GetHandle(),
                iActiveEvent );
        }

        if ( ME_BIT_ENABLED(iActiveEvent, CMEReactor::EVENT_TYPE_ERR) )
        {
            (*iterActiveIOItem)->pIOSink->OnClose(
                (*iterActiveIOItem)->pIO->GetHandle(),
                iActiveEvent );
        }
    }

    /* ֪ͨ�¼����д����¼� */
    if ( NULL != m_pEventQueue )
    {
        /* ��HikReactor.cpp��Run��ϣ�����˳�����ô���һ�η���Ҫ�㶨�����¼� */
        do 
        {
            BOOL bEventQueueExit = FALSE;
            m_pEventQueue->HandleEvent( bEventQueueExit );

            /* ����������TRUE����ô��ʾ�¼����ص����ˣ�����ѭ�� */
            if ( bEventQueueExit )
            {
                break;
            }

        } while ( bExit );
    }

    /*
     *  ����ڴ���ʱ�ӣ���Ϊ��Dispatch��ʱ�������¼��������
     *  �¼���������һ��û�д����������¼�������û��ϵͳIO������£�ֱ�ӳ�ʱ����
     *  Dispatch����Ϊ�ȴ��ĳ�ʱʱ����0�������ڽ���Dispatch��ʱ�򣬺ܿ���û����
     *  ��ʱ�ӵ��ڣ��ȴ�����һЩ�¼��Ժ��ټ��ʱ�Ӷ��У�������ڵ���ʱ�ӣ���ô��
     *  һ�־��ܴ���������򣬺ܿ���Ҫ����һ�ֲŻᴦ����ʱ��
     *  �ڵȴ���һ������ʱ�ӵĳ�ʱʱ���ʱ����Dispatch����ʱͨ��û���Զ����¼�
     *  Ҳû��ϵͳIO�¼�����ôִ�е�����Ӧ�ò��Ứȥ����ʱ�䣬��ʱʱ�ӻ����ܹ���
     *  ʱ�������
     *  ����ϵͳIO�¼����п�������ϵͳIO��Ҫ����Ҳ�п��������µ��Զ��¼���ʱ
     *  �ӣ�����Dispatch��ʱ�Ӷ���ԭ�е�ʱ�ӵ��ڵĿ����Բ��Ǻܴ󣬶�����ӵ�ʱ
     *  ����������Ҳֻ����һ�����ʣ����Ի��ǿ����ȸ㶨�Զ����¼���ϵͳIO�¼��ٴ�
     *  ��ʱ�Ӷ��У����ʱ������е���ʱ�ӣ���ô�����ܹ�����һ�ִ������
     */
    if ( NULL != m_pTimerQueue )
    {
        if ( !bExit )
        {
            m_pTimerQueue->Expire();
        }
        else
        {
            m_pTimerQueue->Destroy();
        }
    }

    return m_iStatus;   // ���������״̬�����仯����ô�����ﷵ����CMEReactor֪��
}

/* 2013.9.4 added by Τ�� */
int CMEReactorBase::GetType() const
{
	return m_iType;
}

UINT CMEReactorBase::GetWaitTime()
{
    ME_INFO_TRACE_THIS( "" );

    UINT iWaitTime = ME_INFINITE;
    do
    {
        /* �¼����в��վ����õȴ�ʱ��Ϊ0 */
        if ( NULL != m_pEventQueue )
        {
            if ( FALSE == m_pEventQueue->IsEmpty() )
            {
                iWaitTime = 0;
                break;
            }
        }

        /* �¼����пվͿ�ʱ�Ӷ���������ڵ�ʱ�� */
        if ( NULL != m_pTimerQueue )
        {
            iWaitTime = m_pTimerQueue->GetNextTimeout();
        }

    } while ( FALSE );

    return iWaitTime;
}

BOOL CMEReactorBase::IsFull()
{
    ME_INFO_TRACE_THIS( "" );

    if ( (CMEReactor::REACTOR_TYPE_SELECT == m_iType && m_ioItemMap.size() >= ME_SELECT_MAX_WAIT_NUM) ||
         (CMEReactor::REACTOR_TYPE_WFMO == m_iType && m_ioItemMap.size() >= ME_WFMO_MAX_WAIT_NUM) )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

ME_NAME_SPACE_END

