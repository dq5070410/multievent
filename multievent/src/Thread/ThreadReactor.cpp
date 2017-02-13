/**
 *  以分离器作为运行方式的线程对象类实现
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

    /* 如果线程处于退出状态，那么，外面将不能够再进行事件投递 */
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
    // 线程对象是阻塞模式或是非阻塞模式下但未运行时不允许阻塞式事件投递
    ME_ASSERTE_RETURN( (NULL != m_hThreadHandle), ME_ERROR_NOT_AVAILABLE );
    // 在线程对象所在的线程中不允许阻塞式事件投递
    ME_ASSERTE_RETURN( (OS::GetThreadID() != m_ThreadID), ME_ERROR_NOT_AVAILABLE );
    ME_ASSERTE_RETURN( (NULL != m_pEventQueue && NULL != m_pReactor), ME_ERROR_NULL_POINTER );
    ME_ASSERTE_RETURN( (NULL != pEvent), ME_ERROR_INVALID_ARG );

    /* 如果线程处于退出状态，那么，外面将不能够再进行事件投递 */
    {
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );
        if ( RUN_STATE_EXIT == m_iStatus )
        {
            return ME_ERROR_NOT_AVAILABLE;
        }
    }

    ME_Result hResult = ME_ERROR_FAILURE;

    /* 创建一个同步事件 */
    CEventQueue::SyncEvent seWaitEventDone;

    hResult = m_pEventQueue->PostEvent(
        pEvent,
        CEventQueue::NodeType::MODE_SEND,
        &seWaitEventDone );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    /* 通知分离器跳出等待 */
    hResult = m_pReactor->Notify( CMEReactor::NOTIFY_TYPE_WAKEUP );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    return seWaitEventDone.GetResult();     // 等待并返回事件执行结果
}

ME_Result CMEThreadReactor::ScheduleTimer(
    CMETimer* pTimer,
    ITimerSink* pSink,
    const CTimeValue& htvInterval,
    const CTimeValue& htvDelay,
    UINT nLoopTime /* = 0 */ )
{
    /* 应该不能直接输出类的实例，先注释 */
    ME_INFO_TRACE_THIS( "pTimer: " << pTimer
        << ", pSink: " << pSink
//         << ", htvInterval: " << htvInterval
//         << ", htvDelay: " << htvDelay
        << ", nLoopTime: " << nLoopTime );

    ME_ASSERTE_RETURN( (NULL != m_pTimerQueue && NULL != m_pReactor), ME_ERROR_NULL_POINTER );
    ME_ASSERTE_RETURN( (NULL != pSink && NULL != pTimer), ME_ERROR_INVALID_ARG);

    /* 如果线程处于退出状态，那么，外面将不能够再添加时钟 */
    {
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );
        if ( RUN_STATE_EXIT == m_iStatus )
        {
            return ME_ERROR_NOT_AVAILABLE;
        }
    }

    ME_Result hResult = ME_ERROR_FAILURE;

    /* 向时钟队列添加时钟 */
    hResult = m_pTimerQueue->Schedule(
        pTimer,
        pSink,
        htvInterval,
        htvDelay,
        nLoopTime );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    /* 通知分离器跳出等待 */
    hResult = m_pReactor->Notify( CMEReactor::NOTIFY_TYPE_WAKEUP );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    return hResult;
}

ME_Result CMEThreadReactor::CancelTimer( CMETimer* pTimer )
{
    ME_INFO_TRACE_THIS( "pTimer: " << pTimer );
    ME_ASSERTE_RETURN( (NULL != m_pTimerQueue && NULL != m_pReactor), ME_ERROR_NULL_POINTER );
    ME_ASSERTE_RETURN( (NULL != pTimer), ME_ERROR_INVALID_ARG);

    /* 如果线程处于退出状态，那么，外面将不能够再取消时钟 */
    {
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );
        if ( RUN_STATE_EXIT == m_iStatus )
        {
            return ME_ERROR_NOT_AVAILABLE;
        }
    }

    ME_Result hResult = ME_ERROR_FAILURE;

    /* 从时钟队列取消时钟 */
    hResult = m_pTimerQueue->Cancel( pTimer );
    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    /* 通知分离器跳出等待 */
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
        /* 非阻塞模式下，如果有多线程来调用，那么线程锁可以保护线程状态正确改变 */
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

        /* 先设置状态为退出状态 */
        m_iStatus = RUN_STATE_EXIT;
    }

    if ( NULL != m_hThreadHandle )
    {
        /* 如果带事件队列，则需要投递一个最终事件来标识结束 */
        if ( NULL != m_pEventQueue )
        {
            ICustomizeEvent* pEvent = new CEventQueueDestroy( m_pEventQueue );
            m_pEventQueue->PostEvent( pEvent );
        }

        ME_Result hResult = ME_ERROR_FAILURE;

        /* 通知分离器退出 */
        hResult = m_pReactor->Notify( CMEReactor::NOTIFY_TYPE_EXIT );

        ME_ASSERTE( ME_SUCCEEDED(hResult) );
    }

    /* 等待退出 */
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

    /* 这里线程的状态可能会变化，那么就要进行保护，防止线程状态意外变成RUNNING */
    do
    {
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

        /* 保证用户回调仅在线程第一次启动时才被调用一次 */
        if ( RUN_STATE_NONE != m_iStatus )
        {
            break;
        }

        m_iStatus = RUN_STATE_RUNNING;

        /* 如果用户设置了回调，那么通知用户线程对象已经启动 */
        if ( NULL != m_pThreadSink )
        {
            m_pThreadSink->OnStart();
        }

    } while ( FALSE );

    m_pReactor->AttachEventQueue(m_pEventQueue);
    m_pReactor->AttachTimerQueue(m_pTimerQueue);

    /* 阻塞模式则使用心跳模式运行分离器，非阻塞模式则使用循环模式运行分离器 */
    if ( m_bBlock )
    {
        m_pReactor->Run( CMEReactor::RUN_MODE_HEART_BEAT );
    }
    else
    {
        m_pReactor->Run( CMEReactor::RUN_MODE_LOOP );
    }

    /** 
     *  非阻塞模式下m_iStatus才可能是RUNNING，阻塞模式下一定是EXIT，所以暂时不考
     *  虑多线程加锁的问题
     *  如果用户设置了回调，那么在退出前通知用户线程即将退出
     */
    if ( RUN_STATE_EXIT == m_iStatus && NULL != m_pThreadSink )
    {
        m_pThreadSink->OnStop();
    }

    return 0;
}

ME_NAME_SPACE_END

