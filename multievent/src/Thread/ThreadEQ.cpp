/**
 *  仅带有事件队列的线程对象类定义
 *
 *
 *  History
 *  02/01/2016 create file
 */


#include "ThreadEQ.h"
#include "OSThread.h"
#include "EventQueueDestroy.h"


ME_NAME_SPACE_BEGIN

CMEThreadEQ::CMEThreadEQ(
    BOOL bBlock,
    INT iThreadID,
    INT iType,
    CMEEventQueue* pEventQueue,
    IMEThreadSink* pThreadSink )
    : CMEThread(
        bBlock,
        iThreadID,
        iType,
        pThreadSink )
    , m_pEventQueue( pEventQueue )
{

}

CMEThreadEQ::~CMEThreadEQ()
{

}

ME_Result CMEThreadEQ::PostEvent( IMECustomizeEvent* pEvent )
{
    ME_INFO_TRACE_THIS( "pEvent: " << pEvent );
    ME_ASSERTE_RETURN( (NULL != m_pEventQueue), ME_ERROR_NULL_POINTER );
    ME_ASSERTE_RETURN( (NULL != pEvent), ME_ERROR_INVALID_ARG );

    {
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );
        if ( RUN_STATE_EXIT == m_iStatus )
        {
            return ME_ERROR_NOT_AVAILABLE;
        }
    }

    return m_pEventQueue->PostEvent( pEvent );
}

ME_Result CMEThreadEQ::SendEvent( IMECustomizeEvent* pEvent )
{
    ME_INFO_TRACE_THIS( "pEvent: " << pEvent );
    /* 线程对象是阻塞模式或是非阻塞模式下但未运行时不允许阻塞式事件投递 */
    ME_ASSERTE_RETURN( (NULL != m_hThreadHandle), ME_ERROR_NOT_AVAILABLE );
    /* 在线程对象所在的线程中不允许阻塞式事件投递 */
    ME_ASSERTE_RETURN( (OS::GetThreadID() != m_ThreadID), ME_ERROR_NOT_AVAILABLE );
    ME_ASSERTE_RETURN( (NULL != m_pEventQueue), ME_ERROR_NULL_POINTER );
    ME_ASSERTE_RETURN( (NULL != pEvent), ME_ERROR_INVALID_ARG );

    {
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );
        if ( RUN_STATE_EXIT == m_iStatus )
        {
            return ME_ERROR_NOT_AVAILABLE;
        }
    }

    ME_Result hResult = ME_ERROR_FAILURE;


    /* 创建一个同步事件 */
    CMEEventQueue::SyncEvent seWaitEventDone;

    /* 投递事件 */
    hResult = m_pEventQueue->PostEvent(
        pEvent,
        CMEEventQueue::NodeType::MODE_SEND,
        &seWaitEventDone );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    return seWaitEventDone.GetResult();     // 等待并返回事件执行结果
}

ME_Result CMEThreadEQ::Join(
    UINT iTimeout,
    BOOL bTerminate )
{
    ME_INFO_TRACE_THIS( "iTimeout: " << iTimeout
        << ", bTerminate: " << bTerminate );

    ME_ASSERTE_RETURN( (NULL != m_pEventQueue), ME_ERROR_NULL_POINTER );

    {
        /* 非阻塞模式下，如果有多线程来调用，那么线程锁可以保护线程状态正确改变 */
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

        /* 先设置状态为退出状态 */
        m_iStatus = RUN_STATE_EXIT;
    }

    ME_Result hResult = ME_ERROR_FAILURE;

    if ( NULL != m_hThreadHandle )
    {
        IMECustomizeEvent* pEvent = new CMEEventQueueDestroy( m_pEventQueue );
        /* 投递停止事件 */
        hResult = m_pEventQueue->PostEvent( pEvent );
        ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );
    }

    /* 等待退出 */
    return CMEThread::Join(
        iTimeout,
        bTerminate );
}

ME_THREAD_RETURN_TYPE CMEThreadEQ::Worker()
{
    ME_INFO_TRACE_THIS( "" );
    
	//ME_ASSERTE_RETURN( (NULL != m_pEventQueue), ME_ERROR_NULL_POINTER );
#ifdef ME_WIN
	ME_ASSERTE_RETURN( (NULL != m_pEventQueue), ME_ERROR_NULL_POINTER );

#else
	ME_ASSERTE_RETURN( (NULL != m_pEventQueue), NULL );

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

    do 
    {
        BOOL bExit = FALSE;
        m_pEventQueue->HandleEvent( bExit );
        /* 当HandleEvent得到Exit为真时，那么事件队列已经被清空，线程可以退出 */
        if ( bExit )
        {
            m_iStatus = RUN_STATE_EXIT;
            break;
        }

        /* 阻塞模式下需要跳出 */
        if ( m_bBlock )
        {
            break;
        }

    } while ( TRUE );

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

