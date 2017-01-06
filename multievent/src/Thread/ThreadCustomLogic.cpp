/**
 *  �û��Զ����߼��̶߳����ඨ��
 *
 *  History
 *  02/01/2016 create file
 */


#include "ThreadCustomLogic.h"

ME_NAME_SPACE_BEGIN

CMEThreadCustomLogic::CMEThreadCustomLogic(
    BOOL bBlock,
    INT iThreadID,
    INT iType,
    IMEThreadSink* pThreadSink )
    : CMEThread(
        bBlock,
        iThreadID,
        iType,
        pThreadSink )
{

}

CMEThreadCustomLogic::~CMEThreadCustomLogic()
{

}

ME_THREAD_RETURN_TYPE CMEThreadCustomLogic::Worker()
{
    ME_INFO_TRACE_THIS( "" );

    //ME_ASSERTE_RETURN( (NULL != m_pThreadSink), ME_ERROR_FAILURE );
#ifdef ME_WIN
	ME_ASSERTE_RETURN( (NULL != m_pThreadSink), ME_ERROR_FAILURE );

#else
	ME_ASSERTE_RETURN( (NULL != m_pThreadSink), NULL );

#endif	// ME_WIN

    /** 
     *  ����ֱ�������ģʽ�ͷ�����ģʽ�������������Ϊ������ģʽ����Ϊ�û���Ҫ
     *  �����ѭ������Run�Դﵽִ���û��߼���Ŀ�ģ�����OnStart�ǻᱻ���ö�εģ�
     *  ��ʱWorker���ز���Ϊ���߳̽�����ֻ���ڱ�־�˳�״̬ʱ���ٵ���Run����ʱ��
     *  ��ִ��OnStop��������ģʽ����Ϊ����ȫ��һ����Runֻ��Ҫ����һ�Σ�WorkerҲ
     *  ֻ�����һ�Σ�ͬ���ģ�OnStartҲ��һ�Σ���OnStart�˳�ʱ������Ϊ�߳���ʱ��
     *  Ҫ�˳�����ô�ͱ������OnStop����ͨ������£��û�����OnStart�˳�ʱ������
     *  ����Join֮ǰ����ʱ�̵߳�״̬����������Ϊ�˳�״̬�����Խ��ж��˳�״̬�ŵ�
     *  ��OnStop�ܿ��ܻᵼ��OnStopû�л��ᱻ���ã����ԣ�������������ж���������
     *  ������������ģʽ�ͷ�����ģʽ
     */

    {
        /** 
         *  ������ģʽ�´��ڶ��̵߳��õ�����£���������ֹ�̶߳����״̬����ı�
         *  ��RUNNING�������������̵߳�״ֻ̬�ܰ�NULL->RUNNING->EXIT˳��ı䣬��
         *  �̶߳���Join�Ժ󣬾������ٱ��RUNNING״̬
         */
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

        if ( RUN_STATE_NONE == m_iStatus )
        {
            m_iStatus = RUN_STATE_RUNNING;
        }
    }

    /* ����ģʽ */
    if ( m_bBlock )
    {
        if ( RUN_STATE_RUNNING == m_iStatus )
        {
            m_pThreadSink->OnStart();
        }
        if ( RUN_STATE_EXIT == m_iStatus )
        {
            m_pThreadSink->OnStop();
        }
    }
    /* ������ģʽ */
    else
    {
        m_pThreadSink->OnStart();

        m_pThreadSink->OnStop();
    }

    //return 0;
#ifdef ME_WIN
	return ME_OK;

#else
	return NULL;

#endif	// ME_WIN
}

ME_NAME_SPACE_END

