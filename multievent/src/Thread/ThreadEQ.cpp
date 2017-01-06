/**
 *  �������¼����е��̶߳����ඨ��
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
    /* �̶߳���������ģʽ���Ƿ�����ģʽ�µ�δ����ʱ����������ʽ�¼�Ͷ�� */
    ME_ASSERTE_RETURN( (NULL != m_hThreadHandle), ME_ERROR_NOT_AVAILABLE );
    /* ���̶߳������ڵ��߳��в���������ʽ�¼�Ͷ�� */
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


    /* ����һ��ͬ���¼� */
    CMEEventQueue::SyncEvent seWaitEventDone;

    /* Ͷ���¼� */
    hResult = m_pEventQueue->PostEvent(
        pEvent,
        CMEEventQueue::NodeType::MODE_SEND,
        &seWaitEventDone );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    return seWaitEventDone.GetResult();     // �ȴ��������¼�ִ�н��
}

ME_Result CMEThreadEQ::Join(
    UINT iTimeout,
    BOOL bTerminate )
{
    ME_INFO_TRACE_THIS( "iTimeout: " << iTimeout
        << ", bTerminate: " << bTerminate );

    ME_ASSERTE_RETURN( (NULL != m_pEventQueue), ME_ERROR_NULL_POINTER );

    {
        /* ������ģʽ�£�����ж��߳������ã���ô�߳������Ա����߳�״̬��ȷ�ı� */
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

        /* ������״̬Ϊ�˳�״̬ */
        m_iStatus = RUN_STATE_EXIT;
    }

    ME_Result hResult = ME_ERROR_FAILURE;

    if ( NULL != m_hThreadHandle )
    {
        IMECustomizeEvent* pEvent = new CMEEventQueueDestroy( m_pEventQueue );
        /* Ͷ��ֹͣ�¼� */
        hResult = m_pEventQueue->PostEvent( pEvent );
        ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );
    }

    /* �ȴ��˳� */
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

    do 
    {
        BOOL bExit = FALSE;
        m_pEventQueue->HandleEvent( bExit );
        /* ��HandleEvent�õ�ExitΪ��ʱ����ô�¼������Ѿ�����գ��߳̿����˳� */
        if ( bExit )
        {
            m_iStatus = RUN_STATE_EXIT;
            break;
        }

        /* ����ģʽ����Ҫ���� */
        if ( m_bBlock )
        {
            break;
        }

    } while ( TRUE );

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

