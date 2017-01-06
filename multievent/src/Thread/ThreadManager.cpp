/**
 *  �̶߳��������ʵ���ඨ��
 *
 *
 *  History
 *  02/18/2016 create file
 *
 *  03/09/2016 fix deadlock
 *             ���̶߳�������������ڼ䣬���ڶ��ֲ��������̱߳�����ע���ע��
 *             ���ֲ�����ı��̱߳�����ݣ���Ȼ��ʼ���ͷ���ʼ��Ҳ�ᣬ����������
 *             �������ڷ������ڼ䣩����JoinAll�Լ�GetThread��Ҫ���̱߳���б���
 *             ��ֹͣ������̶߳��󣬹��������Ĺ����ж��������߳���������ͬ����
 *             ��������JoinAll���̣����̣߳��У������̶߳����ڻص��Զ����¼�ʱ��
 *             ��Ҫ����GetThread���̶߳��������̣߳���ʱ�ͻ����JoinAll�Ȼ����
 *             ���ȴ��̶߳���ֹͣ���̶߳�����GetThread����ס���޷�ֹͣ��������
 *             ��
 *             �������ֻ�м������ķ�Χ�������û���˵����ʼ���ͷ���ʼ�����ǲ���
 *             ֱ�ӷ��ʵģ��������������ʼ���ͷ���ʼ���ڼ������̵߳��ã�ֻ����
 *             ���ڼ�Ĵ��������ٹ��̻����ע���ע�������̱߳���JoinAll������
 *             ������˳��ڼ����Ϊ����ʱҲӦ���ܹ����ⲿ��ע���ע����Ϊ����ʱ
 *             ���ܹ�������JoinAll�ڼ�ʹ�����������̱߳���ôӦ�þ��ܹ���������
 *             ����������
 */


#include "ThreadManager.h"
#include "ThreadFactory.h"
#include "Thread.h"
#include "Reactor.h"
#include "OSThread.h"
#include "LockGuardT.h"


ME_NAME_SPACE_BEGIN

IMEThreadManager* IMEThreadManager::Instance()
{
    return CMEThreadManagerSingleton::Instance();
}

////////////////////////////////////////////////////////////////////////////////

CMEThreadManager::CMEThreadManager()
    : m_bEnableRegister( FALSE )
{

}

CMEThreadManager::~CMEThreadManager()
{

}

ME_Result CMEThreadManager::CreateThread(
    BOOL bBlock,
    INT iType,
    IMEThreadSink* pThreadSink,
    IMEThread*& pThread )
{
    ME_INFO_TRACE_THIS( "bBlock: " << bBlock
        << ", iType" << iType );

    ME_Result hResult = ME_ERROR_FAILURE;
    CMEThread* pCMEThread = NULL;
    do
    {
        /* �����̶߳��� */
        hResult = CreateThread(
            bBlock,
            iType,
            CMEThreadIDManager::CLASSIFICATION_TYPE_EXTERNAL,
            pThreadSink,
            pCMEThread );

        ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

        /* �û��ӿ���Ҫע���̶߳��� */
        hResult = Register( pCMEThread );
        if ( ME_FAILED(hResult) )
        {
            break;
        }

        /**
         * �������������£��û��ӿڻ���Ҫ�����̶߳�������ģʽ����������������
         * ����Reactor���̶߳���ᱻ����
         */
        if ( !bBlock )
        {
            hResult = pCMEThread->Run();
            if ( ME_FAILED(hResult) )
            {
                break;
            }
        }

        /* �ɹ�������²�����̶߳���ʵ�� */
        pThread = pCMEThread;

    } while ( FALSE );

    /* ������ע������������������������ô��Ҫע���������̶߳��� */
    if ( ME_FAILED(hResult) && NULL != pThread )
    {
        Unregister( pCMEThread );
        DestroyThread( pCMEThread );
    }

    return hResult;
}

ME_Result CMEThreadManager::DestroyThread( IMEThread* pThread )
{
    ME_INFO_TRACE_THIS( "" );

    CMEThread* pCMEThread = dynamic_cast<CMEThread*>( pThread );
    ME_ASSERTE_RETURN( (NULL != pCMEThread), ME_ERROR_INVALID_ARG );
    /* �û��ӿڲ��������ٷ��ⲿ�߳� */
    ME_ASSERTE_RETURN( m_ThreadIDManager.IsExternalThread(pCMEThread->GetThreadID()), ME_ERROR_NOT_AVAILABLE );
    /* û��ֹͣ���̶߳����������� */
    ME_ASSERTE_RETURN( (CMEThread::RUN_STATE_EXIT == pCMEThread->m_iStatus), ME_ERROR_NOT_AVAILABLE );

    ME_Result hResult = ME_ERROR_FAILURE;
    do
    {
        /* ��������hResult���뷨�Ǿ�������̶߳�������٣�todo �Ƿ�����ٿ��� */
        /* ע���̶߳��� */
        hResult = Unregister( pCMEThread );
        /* �����̶߳��� */
        hResult = DestroyThread( pCMEThread );

    } while ( FALSE );

    return hResult;
}

ME_Result CMEThreadManager::Join( IMEThread* pThread )
{
    ME_INFO_TRACE_THIS( "" );
    CMEThread* pCMEThread = dynamic_cast<CMEThread*>( pThread );
    ME_ASSERTE_RETURN( (NULL != pCMEThread), ME_ERROR_INVALID_ARG );
    /* �û�ģʽ������ֹͣ���ⲿ�߳� */
    ME_ASSERTE_RETURN( m_ThreadIDManager.IsExternalThread(pThread->GetThreadID()), ME_ERROR_INVALID_ARG );

    ME_Result hResult = Join(
        pCMEThread,
        ME_INFINITE );

    return hResult;
}

IMEThread* CMEThreadManager::GetCurrentThread()
{
    ME_INFO_TRACE_THIS( "" );

    /* �õ�ǰ��ϵͳIDȥ���̶߳��� */
    ME_THREAD_ID hHikThreadID = OS::GetThreadID();

    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    ThreadMapType::iterator iterThread = m_mapThreads.begin();
    for ( ; iterThread != m_mapThreads.end(); ++iterThread )
    {
        if ( iterThread->second->m_ThreadID == hHikThreadID )
        {
            return iterThread->second;
        }
    }

    return NULL;
}

IMEThread* CMEThreadManager::GetNetworkThread()
{
    ME_INFO_TRACE_THIS( "" );

    return GetThread( CMEThreadIDManager::CLASSIFICATION_TYPE_NETWORK );
}

IMEThread* CMEThreadManager::GetUserThread()
{
    ME_INFO_TRACE_THIS( "" );

    return GetThread( CMEThreadIDManager::CLASSIFICATION_TYPE_USER );
}

/*******************************************************************************
 *  ���¶����̶߳��������������������Ľӿ�
 ******************************************************************************/

ME_Result CMEThreadManager::Initialize(
    IMEThreadSink* pThreadSink /* = NULL */,
    BOOL bBlockMode /* = FALSE */ )
{
    ME_INFO_TRACE_THIS( "bBlockMode: " << bBlockMode );

    ME_Result hResult = ME_ERROR_FAILURE;

    CMEThread* pThread = NULL;

    /* ����m_bRunning�������ڳ�ʼ���ڼ䲻������������ע���ע����Ϊ */
    CMELockGuardT<CMELockThreadMutex> Gurad( &m_hThreadMutex );

    /* ����ע��ע�� */
    m_bEnableRegister = TRUE;

    do 
    {
        hResult = CreateThread(
            bBlockMode,
            ME_THREAD_EVENT_QUEUE_TIMER_QUEUE_REACTOR,
            CMEThreadIDManager::CLASSIFICATION_TYPE_USER,
            pThreadSink,
            pThread );

        ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

        hResult = Register( pThread );
        if ( ME_FAILED(hResult) )
        {
            break;
        }

        /* ������ģʽ���ڴ������Ժ�������̣߳�����ģʽ�����û��Լ�ȥ���� */
        if ( !bBlockMode )
        {
            hResult = pThread->Run();
            if ( ME_FAILED(hResult) )
            {
                break;
            }
        }

    } while ( FALSE );

    /* ע���������ʧ�ܶ��ᵽ������մ����������̶߳��� */
    if ( ME_FAILED(hResult) )
    {
        Unregister( pThread );
        DestroyThread( pThread );
        m_bEnableRegister = FALSE;
    }

    return hResult;
}

ME_Result CMEThreadManager::CreateThread(
    INT iNetworkThreadNum,
    INT iUserThreadNum /* = 1 */ )
{
    ME_INFO_TRACE_THIS( "iNetWorkThreadNum: " << iNetworkThreadNum
        << ", iUserThreadNum: " << iUserThreadNum );

    /* ��ǰ��������һ���û��̶߳��� */
    ME_ASSERTE_RETURN( (1 == iUserThreadNum), ME_ERROR_INVALID_ARG );

    ME_Result hResult = ME_ERROR_FAILURE;

    /* �����Initialize��ʱ�򴴽���һ���û��̶߳�����ô�����Ҫ��ȥ */
    iUserThreadNum -= m_mapThreads.size();

    /* �����û��߳� */
    while ( 0 < iUserThreadNum )
    {
        CMEThread* pThread = NULL;

        /* �����û��߳� */
        hResult = CreateThread(
            FALSE,
            ME_THREAD_EVENT_QUEUE_TIMER_QUEUE_REACTOR,
            CMEThreadIDManager::CLASSIFICATION_TYPE_USER,
            NULL,
            pThread );

        ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

        /* ע���û��߳� */
        hResult = Register( pThread );
        if ( ME_FAILED(hResult) )
        {
            DestroyThread( pThread );

            return hResult;
        }

        /* �����û��߳� */
        hResult = pThread->Run();
        if ( ME_FAILED(hResult) )
        {
            Unregister( pThread );
            DestroyThread( pThread );

            return hResult;
        }

        --iUserThreadNum;
    }

    /* ��ʼ�������߳� */
    while ( 0 < iNetworkThreadNum )
    {
        CMEThread* pThread = NULL;

        /* ���������߳� */
        hResult = CreateThread(
            FALSE,
            ME_THREAD_EVENT_QUEUE_TIMER_QUEUE_REACTOR,
            CMEThreadIDManager::CLASSIFICATION_TYPE_NETWORK,
            NULL,
            pThread );

        ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

        /* ע�������߳� */
        hResult = Register( pThread );
        if ( ME_FAILED(hResult) )
        {
            DestroyThread( pThread );

            return hResult;
        }

        /* ���������߳� */
        hResult = pThread->Run();
        if ( ME_FAILED(hResult) )
        {
            Unregister( pThread );
            DestroyThread( pThread );

            return hResult;
        }

        -- iNetworkThreadNum;
    }

    return ME_OK;
}

ME_Result CMEThreadManager::Finalize()
{
    ME_INFO_TRACE_THIS( "" );

    /* ������ô��������JoinAllһ�飬ȷ���̶߳����Ѿ���ֹͣ */
    JoinAll();

    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    ME_Result hLastError = ME_OK;
    ME_Result hResult = ME_ERROR_FAILURE;

    /* �̶߳����Ѿ�ֹͣ�ˣ�������һ���ٲ�����̶߳������ */
    ThreadMapType::iterator iterThread = m_mapThreads.begin();
    for ( ; iterThread != m_mapThreads.end(); ++iterThread )
    {
        hResult = DestroyThread( iterThread->second );
        if ( ME_FAILED(hResult) )
        {
            hLastError = hResult;
        }
    }
    m_mapThreads.clear();

    return hLastError;
}

ME_Result CMEThreadManager::CreateThread(
    BOOL bBlock,
    INT iThreadType,
    INT iClassificationType,
    IMEThreadSink* pThreadSink,
    CMEThread*& pThread )
{
    ME_INFO_TRACE_THIS( "bBlock: " << bBlock
        << ", iThreadType: " << iThreadType
        << ", iReactorType: " << iClassificationType );

    /* �������̶߳���ID */
    INT iThreadID = m_ThreadIDManager.GenerateThreadID( iClassificationType );
    ME_ASSERTE_RETURN( (0 != iThreadID), ME_ERROR_REACH_MAX );

    /* Ĭ�Ͼ��û���WaitForMultipleObjects�ķ����� */
    INT iReactorType = CMEReactor::REACTOR_TYPE_WFMO;

#if (defined ME_LINUX)
    /* Linux��û��WaitForMultipleObjects������Ҫ���ɻ���select��Reactor */
    iReactorType = CMEReactor::REACTOR_TYPE_SELECT;
#endif

    /* �����߳��ڵ�ǰ�汾Ҳ��Ҫ�û���select��Reactor */
    if ( CMEThreadIDManager::CLASSIFICATION_TYPE_NETWORK == iClassificationType )
    {
        iReactorType = CMEReactor::REACTOR_TYPE_SELECT;
    }

    return CMEThreadFactorySingleton::Instance()->Create(
        bBlock,
        iThreadID,
        iThreadType,
		iClassificationType,
        iReactorType,
        pThreadSink,
        pThread );
}

ME_Result CMEThreadManager::DestroyThread( CMEThread* pThread )
{
    ME_INFO_TRACE_THIS( "" );
    ME_ASSERTE_RETURN( (NULL != pThread), ME_ERROR_INVALID_ARG );
    /* �̶߳����ܹ���Destroy��������������һ���ǲ��ܴ�������״̬����һ�����߳̾��Ϊ�� */
    ME_ASSERTE_RETURN( (CMEThread::RUN_STATE_RUNNING != pThread->m_iStatus), ME_ERROR_NOT_AVAILABLE );
    ME_ASSERTE_RETURN( (NULL == pThread->GetThreadHandle()), ME_ERROR_NOT_AVAILABLE );

    /* һ���ж�ͨ������ô������ζ��������߳������� */
    pThread->Join(
        ME_INFINITE,
        FALSE );

    return CMEThreadFactorySingleton::Instance()->Destroy( pThread );
}

ME_Result CMEThreadManager::Register( CMEThread* pThread )
{
    ME_INFO_TRACE_THIS( "" );
    ME_ASSERTE_RETURN( (NULL != pThread), ME_ERROR_INVALID_ARG );

    /* �����ڼ���̶߳������в����������������� */
    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    /* ��ֹע����ֱ�ӷ��أ�����JoinAll���̾Ͳ���Ҫ���������̶߳������ */
    ME_ASSERTE_RETURN( m_bEnableRegister, ME_ERROR_NOT_INITIALIZED );

    /* ����ҵ����ظ�ע�� */
    ThreadMapType::iterator iterThread = m_mapThreads.find( pThread->m_iThreadID );
    if ( iterThread != m_mapThreads.end() )
    {
        return ME_ERROR_FOUND;
    }
    else
    {
        m_mapThreads.insert( std::make_pair(pThread->m_iThreadID, pThread) );

        return ME_OK;
    }
}

ME_Result CMEThreadManager::Unregister( CMEThread* pThread )
{
    ME_INFO_TRACE_THIS( "" );
    ME_ASSERTE_RETURN( (NULL != pThread), ME_ERROR_INVALID_ARG );

    /* �����ڼ���̶߳������в����������������� */
    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    /* ��ֹע����ֱ�ӷ��أ�����JoinAll���̾Ͳ���Ҫ���������̶߳������ */
    ME_ASSERTE_RETURN( m_bEnableRegister, ME_ERROR_NOT_INITIALIZED );

    ThreadMapType::iterator iterThread = m_mapThreads.find( pThread->m_iThreadID );
    if ( iterThread == m_mapThreads.end() )
    {
        return ME_ERROR_NOT_FOUND;
    }
    else
    {
        m_mapThreads.erase( iterThread );

        return ME_OK;
    }
}

ME_Result CMEThreadManager::Join(
    CMEThread* pThread,
    UINT iTimeout )
{
    ME_INFO_TRACE_THIS( "iTimeout: " << iTimeout );
    ME_ASSERTE_RETURN( (NULL != pThread), ME_ERROR_INVALID_ARG );

    return pThread->Join(
        iTimeout,
        iTimeout == (UINT)ME_INFINITE ? FALSE : TRUE );
}

ME_Result CMEThreadManager::JoinAll( UINT iTimeout )
{
    ME_INFO_TRACE_THIS( "iTimeout: " << iTimeout );

    {
        /** 
         *  �����ķ�Χ���Ƶ���С����JoinAll��ʱ���ȳ��Խ�״̬��Ϊ������״̬
         *  ֻҪִ���굱ǰ����飬��ô��ǰ�����У�֮��Ҳ�������̵߳���ע�����ע
         *  �����̣�JoinAll����Ҳ�Ͳ��ص������̸߳ı��̶߳�����ˣ�����ĳ�ʼ��
         *  �ͷ���ʼ����ֻ�����̵߳��õ�����£���Ҳ��û��Ҫ�����������̶߳����
         *  ��
         */
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

        /* ��ֹע���ע��������ע���ע������ֱ�ӷ��ش������������̶߳���� */
        m_bEnableRegister = FALSE;
    }

    /* ���Ȱ��ⲿ�̶߳�ͣ�� */
    INT iClassification = CMEThreadIDManager::CLASSIFICATION_TYPE_EXTERNAL;
    do
    {
        ThreadMapType::iterator iterThread = m_mapThreads.begin();
        for ( ; iterThread != m_mapThreads.end(); ++iterThread )
        {
            if ( CMEThread::RUN_STATE_EXIT != iterThread->second->m_iStatus &&
                 m_ThreadIDManager.GetThreadClassificationType(iterThread->second->GetThreadID()) == iClassification )
            {
                iterThread->second->Join(
                    iTimeout,
                    TRUE );
            }
        }

        /* ����Ҫ��ͣ�����߳���ͣ�û��߳� */
        if ( CMEThreadIDManager::CLASSIFICATION_TYPE_EXTERNAL == iClassification )
        {
            iClassification = CMEThreadIDManager::CLASSIFICATION_TYPE_NETWORK;
        }
        else if (CMEThreadIDManager::CLASSIFICATION_TYPE_NETWORK == iClassification)
        {
            iClassification = CMEThreadIDManager::CLASSIFICATION_TYPE_USER;
        }
        else
        {
            break;
        }
    } while ( TRUE );

    return ME_OK;
}

IMEThread* CMEThreadManager::GetThread( INT iClassification )
{
    ME_INFO_TRACE_THIS( "iClassification: " << iClassification );

    /** 
     *  ��ΪGetThread�������ڼ��ǻᱻ���õģ���ʱҲ����ע���ע���Ĺ��̣�������
     *  Ҫ���̶߳������б�������ֹ�ڱ������ҹ�������Ϊ�̶߳����ı䵼�µ�����
     *  ʧЧ
     */
    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    /** 
     *  �ȼ�ʵ�֣�����ѡ���ҵ���Ŀ�������׸��̶߳����Ժ��и��ؾ������ʱ��
     *  ������д�ⲿ���㷨
     */
    ThreadMapType::iterator iterThread = m_mapThreads.begin();
    for ( ; iterThread != m_mapThreads.end(); ++iterThread )
    {
        if ( m_ThreadIDManager.GetThreadClassificationType(iterThread->first) == iClassification )
        {
            return iterThread->second;
        }
    }

    return NULL;
}

ME_NAME_SPACE_END

