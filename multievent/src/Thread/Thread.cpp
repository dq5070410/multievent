/**
 *  �̶߳�����ඨ��
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

    /* ����ģʽ��ֱ������Worker����ʱ�����Ƕ��̵߳��õ����� */
    if ( m_bBlock )
    {
        if ( RUN_STATE_NONE == m_iStatus )
        {
            m_ThreadID = OS::GetThreadID();    // ����ģʽ��Ҳ��Ҫ�õ�ʵ���߳�ID
        }

        Worker();

        return ME_OK;
    }

    /** 
     *  ������ģʽ���������ƶ��̵߳���
     *  ���߳����ͬʱRun��Join����ô���յĽ��һ�����̶߳�����ͣ���ģ���������
     *  �˱�ֹͣ��û������
     */
    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    /* ������ģʽ����Ҫ������̵߳�״̬������������¾Ϳ��Է����� */
    if ( RUN_STATE_RUNNING == m_iStatus || NULL != m_hThreadHandle )
    {
        ME_WARNING_TRACE( "Thread has already started" );

        return ME_ERROR_ALREADY_INITIALIZED;
    }

    /* �߳�ֹͣ״̬����ֹͣ������Ҳ��Ҫ�������ش��� */
    if ( RUN_STATE_EXIT == m_iStatus )
    {
        ME_ERROR_TRACE( "Thread is on exit" );

        return ME_ERROR_FAILURE;
    }

    /* ������ģʽ���Դ���һ���߳�������Worker */
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
        /* �������ʱֻ��������m_iStatus */
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

        /* �������������̶߳����״̬��Ϊexit */
        m_iStatus = RUN_STATE_EXIT;
    }

    /* û���߳̾�������̶߳����Ѿ�ͣ�˻���������ģʽ�����������������Ҫ�ȴ� */
    if ( NULL == m_hThreadHandle )
    {
        ME_WARNING_TRACE( "CMEThread( " << m_iThreadID << " ) not run" );

        return ME_OK;
    }

    /* OS��ӿ�����ĳ�ʱʱ����߳�״̬���� */
    CMETimeValue hTimeValue;
    ME_THREAD_STAT hThreadStat;

    /* ֻ�д������޳�ʱʱ�������²����ó�ʱʱ�� */
    if ( (UINT)ME_INFINITE != iTimeout )
    {
        hTimeValue.Set(
            (iTimeout / 1000),
            (iTimeout % 1000 * 1000) );
    }

    /* �ȴ��̶߳����˳� */
    ME_Result hResult = OS::ThreadJoin(
        m_hThreadHandle,
        (UINT)ME_INFINITE != iTimeout ? &hTimeValue : NULL,
        &hThreadStat );

    /* Join֮����ô�ͱ����߳̾�������ܴ��������̶߳���Destroy�������TODO��Linux������Ҫ�����Ƿ�����쳣 */
    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    /* ֹͣ��ʱ��������ǿͣ��ʱ�����ǿͣ */
    if ( ME_ERROR_FAILURE == hResult  && bTerminate )
    {
        hResult = OS::ThreadDestroy(
            m_hThreadHandle,
            0 );

        ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

        m_hThreadHandle = NULL;
        m_ThreadID = 0;

        return ME_ERROR_TIMEOUT;   // ��ʹǿͣ�ɹ�ҲҪ���������̶߳���ֹͣ��ʱ��
    }

    /* һ������ */
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
    /* Ӧ�ò���ֱ��������ʵ������ע�� */
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
    /* �ðɣ����Ҫ��ס���Ͳ�����const */
    CLockGuardT<CLockThreadMutex> Guard( &m_hThreadMutex );

    return m_hThreadHandle;
}

ME_NAME_SPACE_END

