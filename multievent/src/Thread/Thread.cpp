/**
 *  线程对象基类定义
 *
 *  History
 *  02/01/2016 create file
 */


#include "Thread.h"
#include "OSThread.h"


ME_NAME_SPACE_BEGIN

CMEThread::CMEThread(
    BOOL bBlock,
    INT iThreadID,
    INT iType,
    IMEThreadSink* pThreadSink )
    : m_bBlock( bBlock )
    , m_iStatus( RUN_STATE_NONE )
    , m_pThreadSink( pThreadSink )
    , m_hThreadHandle( NULL )
    , m_ThreadID( 0 )
    , m_iThreadID( iThreadID )
    , m_iThreadType( iType )
{
}

CMEThread::~CMEThread()
{
}

INT CMEThread::GetThreadID() const
{
    return m_iThreadID;
}

INT CMEThread::GetThreadType() const
{
    return m_iThreadType;
}

ME_Result CMEThread::Run()
{
    ME_INFO_TRACE_THIS( "" );

	
#ifdef ME_LINUX
	::sigset_t sigMask;
	::sigemptyset( &sigMask );

	::sigaddset(
		&sigMask, 
		SIGPIPE );

	int iResult = ::pthread_sigmask(
		SIG_BLOCK, 
		&sigMask, 
		NULL );

	if ( 0 != iResult ) 
	{
		ME_WARNING_TRACE_THIS( "can't disable signal SIGPIPE, error = " << errno );
	}

#endif	// ME_LINUX

    /* 阻塞模式就直接运行Worker，暂时不考虑多线程调用的问题 */
    if ( m_bBlock )
    {
        if ( RUN_STATE_NONE == m_iStatus )
        {
            m_ThreadID = OS::GetThreadID();    // 阻塞模式下也需要得到实际线程ID
        }

        Worker();

        return ME_OK;
    }

    /** 
     *  非阻塞模式用锁来控制多线程调用
     *  多线程如果同时Run和Join，那么最终的结果一定是线程对象是停掉的（包括启动
     *  了被停止和没启动）
     */
    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    /* 非阻塞模式则需要检查下线程的状态，启动的情况下就可以返回了 */
    if ( RUN_STATE_RUNNING == m_iStatus || NULL != m_hThreadHandle )
    {
        ME_WARNING_TRACE( "Thread has already started" );

        return ME_ERROR_ALREADY_INITIALIZED;
    }

    /* 线程停止状态或者停止过程中也需要立即返回错误 */
    if ( RUN_STATE_EXIT == m_iStatus )
    {
        ME_ERROR_TRACE( "Thread is on exit" );

        return ME_ERROR_FAILURE;
    }

    /* 非阻塞模式则尝试创建一个线程来运行Worker */
    ME_Result hResult = ME_ERROR_FAILURE;
    hResult = OS::ThreadCreate(
        ThreadFunc,
        this,
        0,
        m_ThreadID,
        &m_hThreadHandle );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	m_lockEvent.Wait();

    return hResult;
}

ME_Result CMEThread::Join(
    UINT iTimeout,
    BOOL bTerminate )
{
    ME_INFO_TRACE_THIS( "iTimeout: " << iTimeout
        << ", bTerminate: " << bTerminate );
    {
        /* 这把锁暂时只用来保护m_iStatus */
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

        /* 不论怎样都将线程对象的状态置为exit */
        m_iStatus = RUN_STATE_EXIT;
    }

    /* 没有线程句柄表明线程对象已经停了或者是阻塞模式，这两种情况都不需要等待 */
    if ( NULL == m_hThreadHandle )
    {
        ME_WARNING_TRACE( "CMEThread( " << m_iThreadID << " ) not run" );

        return ME_OK;
    }

    /* OS层接口所需的超时时间和线程状态参数 */
    CMETimeValue hTimeValue;
    ME_THREAD_STAT hThreadStat;

    /* 只有存在有限超时时间的情况下才设置超时时间 */
    if ( (UINT)ME_INFINITE != iTimeout )
    {
        hTimeValue.Set(
            (iTimeout / 1000),
            (iTimeout % 1000 * 1000) );
    }

    /* 等待线程对象退出 */
    ME_Result hResult = OS::ThreadJoin(
        m_hThreadHandle,
        (UINT)ME_INFINITE != iTimeout ? &hTimeValue : NULL,
        &hThreadStat );

    /* Join之后，那么就保护线程句柄，不能存在两个线程都来Destroy的情况，TODO：Linux环境需要考虑是否会有异常 */
    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    /* 停止超时，当允许强停的时候才能强停 */
    if ( ME_ERROR_FAILURE == hResult  && bTerminate )
    {
        hResult = OS::ThreadDestroy(
            m_hThreadHandle,
            0 );

        ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

        m_hThreadHandle = NULL;
        m_ThreadID = 0;

        return ME_ERROR_TIMEOUT;   // 即使强停成功也要告诉外面线程对象停止超时了
    }

    /* 一切正常 */
    m_hThreadHandle = NULL;
    m_ThreadID = 0;

    return ME_OK;
}

ME_Result CMEThread::PostEvent( ICustomizeEvent* pEvent )
{
    ME_INFO_TRACE_THIS( "pEvent: " << pEvent );

    return ME_ERROR_NOT_AVAILABLE;
}

ME_Result CMEThread::SendEvent( ICustomizeEvent* pEvent )
{
    ME_INFO_TRACE_THIS( "pEvent: " << pEvent );

    return ME_ERROR_NOT_AVAILABLE;
}

ME_Result CMEThread::ScheduleTimer(
    CTimer* pTimer,
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

    return ME_ERROR_NOT_AVAILABLE;
}

ME_Result CMEThread::CancelTimer( CTimer* pTimer )
{
    ME_INFO_TRACE_THIS( "pTimer: " << pTimer );

    return ME_ERROR_NOT_AVAILABLE;
}

CReactor* CMEThread::GetReactor()
{
    ME_INFO_TRACE_THIS( "" );

    return NULL;
}

ME_THREAD_RETURN_TYPE ME_STDCALL CMEThread::ThreadFunc( LPVOID pParam )
{
    ME_INFO_TRACE( "" );

    CMEThread* pThread = static_cast<CMEThread*>( pParam );
    if ( NULL != pThread )
    {
		pThread->m_lockEvent.Signal();

#ifdef ME_LINUX
		::sigset_t sigMask;
		::sigemptyset( &sigMask );

		::sigaddset(
			&sigMask, 
			SIGPIPE );

		int iResult = ::pthread_sigmask(
			SIG_BLOCK, 
			&sigMask, 
			NULL );

		if ( 0 != iResult ) 
		{
			ME_WARNING_TRACE_THIS( "can't disable signal SIGPIPE, error = " << errno );
		}

#endif	// ME_LINUX

        return pThread->Worker();
    }

	
#ifdef ME_WIN
    return ME_ERROR_FAILURE;

#else
	return NULL;

#endif	// ME_WIN
}

ME_THREAD_ID CMEThread::GetThreadOSID() const
{
    return m_ThreadID;
}

ME_THREAD_HANDLE CMEThread::GetThreadHandle() // const
{
    /* 好吧，如果要锁住，就不能用const */
    CLockGuardT<CLockThreadMutex> Guard( &m_hThreadMutex );

    return m_hThreadHandle;
}

ME_NAME_SPACE_END

