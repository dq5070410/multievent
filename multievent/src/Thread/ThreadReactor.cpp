/**
 *  �Է�������Ϊ���з�ʽ���̶߳�����ʵ��
 *
 *
 *  History
 *  02/05/2016 create file
 */


#include "ThreadReactor.h"
#include "Reactor.h"
#include "OSThread.h"
#include "EventQueueDestroy.h"


ME_NAME_SPACE_BEGIN

CMEThreadReactor::CMEThreadReactor(
    BOOL bBlock,
    INT iThreadID,
    INT iType,
    CMEReactor* pReactor,
    CMETimerQueue* pTimerQueue,
    CEventQueue* pEventQueue,
    IMEThreadSink* pThreadSink )
    : CMEThread(
        bBlock,
        iThreadID,
        iType,
        pThreadSink )
    , m_pReactor( pReactor )
    , m_pTimerQueue( pTimerQueue )
    , m_pEventQueue( pEventQueue )
{

}

CMEThreadReactor::~CMEThreadReactor()
{

}

ME_Result CMEThreadReactor::PostEvent( ICustomizeEvent* pEvent )
{
    ME_INFO_TRACE_THIS( "pEvent: " << pEvent );
    ME_ASSERTE_RETURN( (NULL != m_pEventQueue && NULL != m_pReactor), ME_ERROR_NULL_POINTER );
    ME_ASSERTE_RETURN( (NULL != pEvent), ME_ERROR_INVALID_ARG );

    /* ����̴߳����˳�״̬����ô�����潫���ܹ��ٽ����¼�Ͷ�� */
    {
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );
        if ( RUN_STATE_EXIT == m_iStatus )
        {
            return ME_ERROR_NOT_AVAILABLE;
        }
    }

    ME_Result hResult = ME_ERROR_FAILURE;
    
    hResult = m_pEventQueue->PostEvent( pEvent );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    hResult = m_pReactor->Notify( CMEReactor::NOTIFY_TYPE_WAKEUP );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    return hResult;
}

ME_Result CMEThreadReactor::SendEvent( ICustomizeEvent* pEvent )
{
    ME_INFO_TRACE_THIS( "pEvent: " << pEvent );
    // �̶߳���������ģʽ���Ƿ�����ģʽ�µ�δ����ʱ����������ʽ�¼�Ͷ��
    ME_ASSERTE_RETURN( (NULL != m_hThreadHandle), ME_ERROR_NOT_AVAILABLE );
    // ���̶߳������ڵ��߳��в���������ʽ�¼�Ͷ��
    ME_ASSERTE_RETURN( (OS::GetThreadID() != m_ThreadID), ME_ERROR_NOT_AVAILABLE );
    ME_ASSERTE_RETURN( (NULL != m_pEventQueue && NULL != m_pReactor), ME_ERROR_NULL_POINTER );
    ME_ASSERTE_RETURN( (NULL != pEvent), ME_ERROR_INVALID_ARG );

    /* ����̴߳����˳�״̬����ô�����潫���ܹ��ٽ����¼�Ͷ�� */
    {
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );
        if ( RUN_STATE_EXIT == m_iStatus )
        {
            return ME_ERROR_NOT_AVAILABLE;
        }
    }

    ME_Result hResult = ME_ERROR_FAILURE;

    /* ����һ��ͬ���¼� */
    CEventQueue::SyncEvent seWaitEventDone;

    hResult = m_pEventQueue->PostEvent(
        pEvent,
        CEventQueue::NodeType::MODE_SEND,
        &seWaitEventDone );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    /* ֪ͨ�����������ȴ� */
    hResult = m_pReactor->Notify( CMEReactor::NOTIFY_TYPE_WAKEUP );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    return seWaitEventDone.GetResult();     // �ȴ��������¼�ִ�н��
}

ME_Result CMEThreadReactor::ScheduleTimer(
    CMETimer* pTimer,
    ITimerSink* pSink,
    const CTimeValue& htvInterval,
    const CTimeValue& htvDelay,
    UINT nLoopTime /* = 0 */ )
{
    /* Ӧ�ò���ֱ��������ʵ������ע�� */
    ME_INFO_TRACE_THIS( "pTimer: " << pTimer
        << ", pSink: " << pSink
//         << ", htvInterval: " << htvInterval
//         << ", htvDelay: " << htvDelay
        << ", nLoopTime: " << nLoopTime );

    ME_ASSERTE_RETURN( (NULL != m_pTimerQueue && NULL != m_pReactor), ME_ERROR_NULL_POINTER );
    ME_ASSERTE_RETURN( (NULL != pSink && NULL != pTimer), ME_ERROR_INVALID_ARG);

    /* ����̴߳����˳�״̬����ô�����潫���ܹ������ʱ�� */
    {
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );
        if ( RUN_STATE_EXIT == m_iStatus )
        {
            return ME_ERROR_NOT_AVAILABLE;
        }
    }

    ME_Result hResult = ME_ERROR_FAILURE;

    /* ��ʱ�Ӷ������ʱ�� */
    hResult = m_pTimerQueue->Schedule(
        pTimer,
        pSink,
        htvInterval,
        htvDelay,
        nLoopTime );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    /* ֪ͨ�����������ȴ� */
    hResult = m_pReactor->Notify( CMEReactor::NOTIFY_TYPE_WAKEUP );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    return hResult;
}

ME_Result CMEThreadReactor::CancelTimer( CMETimer* pTimer )
{
    ME_INFO_TRACE_THIS( "pTimer: " << pTimer );
    ME_ASSERTE_RETURN( (NULL != m_pTimerQueue && NULL != m_pReactor), ME_ERROR_NULL_POINTER );
    ME_ASSERTE_RETURN( (NULL != pTimer), ME_ERROR_INVALID_ARG);

    /* ����̴߳����˳�״̬����ô�����潫���ܹ���ȡ��ʱ�� */
    {
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );
        if ( RUN_STATE_EXIT == m_iStatus )
        {
            return ME_ERROR_NOT_AVAILABLE;
        }
    }

    ME_Result hResult = ME_ERROR_FAILURE;

    /* ��ʱ�Ӷ���ȡ��ʱ�� */
    hResult = m_pTimerQueue->Cancel( pTimer );
    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    /* ֪ͨ�����������ȴ� */
    hResult = m_pReactor->Notify( CMEReactor::NOTIFY_TYPE_WAKEUP );
    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    return hResult;
}

CMEReactor* CMEThreadReactor::GetReactor()
{
    ME_INFO_TRACE_THIS( "" );

    return m_pReactor;
}

ME_Result CMEThreadReactor::Join(
    UINT iTimeout,
    BOOL bTerminate )
{
    ME_INFO_TRACE_THIS( "iTimeout: " << iTimeout
        << ", bTerminate: " << bTerminate );

    ME_ASSERTE_RETURN( (NULL != m_pReactor), ME_ERROR_NULL_POINTER );

    {
        /* ������ģʽ�£�����ж��߳������ã���ô�߳������Ա����߳�״̬��ȷ�ı� */
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

        /* ������״̬Ϊ�˳�״̬ */
        m_iStatus = RUN_STATE_EXIT;
    }

    if ( NULL != m_hThreadHandle )
    {
        /* ������¼����У�����ҪͶ��һ�������¼�����ʶ���� */
        if ( NULL != m_pEventQueue )
        {
            ICustomizeEvent* pEvent = new CEventQueueDestroy( m_pEventQueue );
            m_pEventQueue->PostEvent( pEvent );
        }

        ME_Result hResult = ME_ERROR_FAILURE;

        /* ֪ͨ�������˳� */
        hResult = m_pReactor->Notify( CMEReactor::NOTIFY_TYPE_EXIT );

        ME_ASSERTE( ME_SUCCEEDED(hResult) );
    }

    /* �ȴ��˳� */
    return CMEThread::Join(
        iTimeout,
        bTerminate );
}

ME_THREAD_RETURN_TYPE CMEThreadReactor::Worker()
{
    ME_INFO_TRACE_THIS( "" );

	//ME_ASSERTE_RETURN( (NULL != m_pReactor), ME_ERROR_NULL_POINTER );
#ifdef ME_WIN
	ME_ASSERTE_RETURN( (NULL != m_pReactor), ME_ERROR_NULL_POINTER );

	if ( CMEReactor::REACTOR_TYPE_ASYCH_IOCP == m_pReactor->GetType() )
	{
		OS::ThreadSetPriority(
			OS::GetThreadHandle(),
			THREAD_PRIORITY_ABOVE_NORMAL );
	}

#else
	ME_ASSERTE_RETURN( (NULL != m_pReactor), NULL );

#endif	// ME_WIN

    /* �����̵߳�״̬���ܻ�仯����ô��Ҫ���б�������ֹ�߳�״̬������RUNNING */
    do
    {
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

        /* ��֤�û��ص������̵߳�һ������ʱ�ű�����һ�� */
        if ( RUN_STATE_NONE != m_iStatus )
        {
            break;
        }

        m_iStatus = RUN_STATE_RUNNING;

        /* ����û������˻ص�����ô֪ͨ�û��̶߳����Ѿ����� */
        if ( NULL != m_pThreadSink )
        {
            m_pThreadSink->OnStart();
        }

    } while ( FALSE );

    m_pReactor->AttachEventQueue(m_pEventQueue);
    m_pReactor->AttachTimerQueue(m_pTimerQueue);

    /* ����ģʽ��ʹ������ģʽ���з�������������ģʽ��ʹ��ѭ��ģʽ���з����� */
    if ( m_bBlock )
    {
        m_pReactor->Run( CMEReactor::RUN_MODE_HEART_BEAT );
    }
    else
    {
        m_pReactor->Run( CMEReactor::RUN_MODE_LOOP );
    }

    /** 
     *  ������ģʽ��m_iStatus�ſ�����RUNNING������ģʽ��һ����EXIT��������ʱ����
     *  �Ƕ��̼߳���������
     *  ����û������˻ص�����ô���˳�ǰ֪ͨ�û��̼߳����˳�
     */
    if ( RUN_STATE_EXIT == m_iStatus && NULL != m_pThreadSink )
    {
        m_pThreadSink->OnStop();
    }

    return 0;
}

ME_NAME_SPACE_END

