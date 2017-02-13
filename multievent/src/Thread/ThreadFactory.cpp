/**
 *  �̶߳��󹤳��ඨ��
 *
 *
 *  History
 *  02/19/2016 create file
 */


#include "ThreadFactory.h"
#include "ReactorFactory.h"
#include "Thread.h"
#include "ThreadCustomLogic.h"
#include "ThreadEQ.h"
#include "ThreadEQR.h"
#include "ThreadTQR.h"
#include "ThreadEQTQR.h"
#include "ThreadManager.h"
#include "EventQueueT.h"
#include "TimerQueueT.h"
#include "Reactor.h"

ME_NAME_SPACE_BEGIN

ME_Result CMEThreadFactory::Create(
    BOOL bBlock,
    INT iThreadID,
    INT iThreadType,
	INT iClassificationType,	
    INT iReactorType,
    IMEThreadSink* pThreadSink,
    CMEThread*& pThread )
{
    ME_INFO_TRACE_THIS(
        "bBlock: " << bBlock <<
        ", iThreadID: " << iThreadID <<
        ", iThreadType: " << iThreadType <<
        ", iReactorType: " << iReactorType );

    ME_ASSERTE_RETURN( (NULL == pThread), ME_ERROR_INVALID_ARG );

    /**
     *  ������ME_OK��Ϊ��ʼֵ����Ϊ��û��Reactor��ʱ����Ϊ�����̶߳����׼��
     *  ������������
     */
    ME_Result hResult = ME_OK;
    CEventQueue* pEventQueue = NULL;
    CMETimerQueue* pTimerQueue = NULL;
    CMEReactor* pReactor = NULL;

	/* 2013.9.3 added by Τ�� */
	CMEThreadReactor* pNetworkThread = (CMEThreadReactor*)CMEThreadManagerSingleton::Instance()->GetNetworkThread();

    /**
        *  �ȸ����̶߳��������е���������������Ҫ����Դ�������Щ��Դ����������
        *  �⣬��ô�����ܹ�ͨ���쳣���Ǵ������жϽ���������߼�����������߳�
        *  ����Ĵ���������try��catch�鶼������Ϊ����ֱ�ӷ��أ�������������
        *  �߼������Ѿ���������Դ
        */
    if ( ME_BIT_ENABLED(iThreadType, IMEThreadManager::ME_THREAD_EVENT_QUEUE) )
    {
        /* �¼����У�new��ʱ������׳�Exception���ܹ���catch�� */
        //pEventQueue = new CMEEventQueue;

		CreateQueue( 
			pEventQueue, 
			iClassificationType );
    }

    if ( ME_BIT_ENABLED(iThreadType, IMEThreadManager::ME_THREAD_TIMER_QUEUE) )
    {
        /* ʱ�Ӷ��У�new��ʱ������׳�Exception���ܹ���catch�� */
        //pTimerQueue = new CMETimerQueue;
	
		CreateQueue( 
			pTimerQueue, 
			iClassificationType );
    }

    if ( ME_BIT_ENABLED(iThreadType, IMEThreadManager::ME_THREAD_REACTOR) )
    {
        /* ����������Ϊ�����׳�Exception����ô����Ҫ�Է���ֵ�����ж� */
        hResult = CMEReactorFactorySingleton::Instance()->Create(
            iReactorType,
			iClassificationType,
            pReactor );
    }

    /* ��Ҫ�����жϵ���ҪReactorʱ��Reactor�Ƿ�ɹ����� */
    if ( ME_SUCCEEDED(hResult) )
    {
        /* �����̶߳�����������ֱ𴴽���Ӧ���̶߳���ʵ�� */
        switch ( iThreadType )
        {
        case IMEThreadManager::ME_THREAD_CUSTOM_LOGIC:
            {
                /* ��������ǿ��Ҫ�����û��߼��ص� */
                ME_ASSERTE_RETURN( (NULL != pThreadSink), ME_ERROR_INVALID_ARG );
                pThread = new CMEThreadCustomLogic(
                    bBlock,
                    iThreadID,
                    iThreadType,
                    pThreadSink );

                hResult = ME_OK;
                break;
            }

        case IMEThreadManager::ME_THREAD_EVENT_QUEUE:
            {
                pThread = new CMEThreadEQ(
                    bBlock,
                    iThreadID,
                    iThreadType,
                    pEventQueue,
                    pThreadSink );

                hResult = ME_OK;
                break;
            }

        case IMEThreadManager::ME_THREAD_EVENT_QUEUE_REACTOR:
            {
                pThread = new CMEThreadEQR(
                    bBlock,
                    iThreadID,
                    iThreadType,
                    pReactor,
                    pEventQueue,
                    pThreadSink );

                hResult = ME_OK;
                break;
            }

        case IMEThreadManager::ME_THREAD_TIMER_QUEUE_REACTOR:
            {
                pThread = new CMEThreadTQR(
                    bBlock,
                    iThreadID,
                    iThreadType,
                    pReactor,
                    pTimerQueue,
                    pThreadSink );

                hResult = ME_OK;
                break;
            }

        case IMEThreadManager::ME_THREAD_EVENT_QUEUE_TIMER_QUEUE_REACTOR:
            {
                pThread = new CMEThreadEQTQR(
                    bBlock,
                    iThreadID,
                    iThreadType,
                    pReactor,
                    pTimerQueue,
                    pEventQueue,
                    pThreadSink );

                hResult = ME_OK;
                break;
            }

        default:    // ���������������Ϊ���쳣���룬���ǲ���ֱ�ӷ���
            {
                hResult = ME_ERROR_INVALID_ARG;
                break;
            }
        }
    }

    /* ʧ�ܵ��������Ҫ������Դ���� */
    if ( ME_FAILED(hResult) )
    {
        ME_DELETE( pThread );
        ME_DELETE( pEventQueue );
        ME_DELETE( pTimerQueue );
        CMEReactorFactorySingleton::Instance()->Destroy( pReactor );
    }

    return hResult;
}

ME_Result CMEThreadFactory::Destroy( CMEThread*& pThread )
{
    ME_INFO_TRACE_THIS( "" );
    ME_ASSERTE_RETURN( (NULL != pThread), ME_ERROR_INVALID_ARG );

    CEventQueue* pEventQueue = NULL;
    CMETimerQueue* pTimerQueue = NULL;
    CMEReactor* pReactor = NULL;

    switch ( pThread->GetThreadType() )
    {
    case IMEThreadManager::ME_THREAD_CUSTOM_LOGIC:
        {
            ME_DELETE( pThread );
            break;
        }

    case IMEThreadManager::ME_THREAD_EVENT_QUEUE:
        {
            CMEThreadEQ* pThreadEQ = dynamic_cast<CMEThreadEQ*>( pThread );
            ME_ASSERTE_RETURN( (NULL != pThreadEQ), ME_ERROR_INVALID_ARG );
            pEventQueue = pThreadEQ->m_pEventQueue;

            ME_DELETE( pThread );
            ME_DELETE( pEventQueue );
            break;
        }

    case IMEThreadManager::ME_THREAD_EVENT_QUEUE_REACTOR:
    case IMEThreadManager::ME_THREAD_TIMER_QUEUE_REACTOR:
    case IMEThreadManager::ME_THREAD_EVENT_QUEUE_TIMER_QUEUE_REACTOR:
        {
            CMEThreadReactor* pThreadR = dynamic_cast<CMEThreadReactor*>( pThread );
            ME_ASSERTE_RETURN( (NULL != pThreadR), ME_ERROR_INVALID_ARG );
            pEventQueue = pThreadR->m_pEventQueue;
            pTimerQueue = pThreadR->m_pTimerQueue;
            pReactor = pThreadR->m_pReactor;

            ME_DELETE( pThread );
			//ME_DELETE( pEventQueue );
			//ME_DELETE( pTimerQueue );
			if ( ME_BIT_DISABLED(pReactor->GetType(), CMEReactor::REACTOR_TYPE_ASYNC) )
			{
				ME_DELETE( pEventQueue );
				ME_DELETE( pTimerQueue );

				/** 
				* �첽Reactorʱ���¼�EventQueue��TimerQueue��ReactorImpl��Ϊ������
				* ��CMEReactor��new�����ģ���ÿ�������߳̾���һ��CMEReactorʵ����
				* ��Ҫɾ����ReactorFactory�б�֤���첽ReactorֻɾCMEReactor����ɾ
				* ReactorImpl���������ֱ�ӵ���Destroy()����
				*/
				//CMEReactorFactorySingleton::Instance()->Destroy( pReactor );
			}

			CMEReactorFactorySingleton::Instance()->Destroy( pReactor );

            break;
        }

    default:
        {
            ME_ASSERTE_RETURN( FALSE, ME_ERROR_INVALID_ARG );
        }
    }

    return ME_OK;
}

template <class Queue>
void CMEThreadFactory::CreateQueue( 
	Queue*& pQueue,
	int iType )
{
	CMEThreadReactor* pNetworkThread = (CMEThreadReactor*)CMEThreadManagerSingleton::Instance()->GetNetworkThread();

	/* ����ǵ�һ�������������͵��߳� */
	if ( NULL == pNetworkThread )
	{
		pQueue = new Queue;
		if ( iType == CMEThreadIDManager::CLASSIFICATION_TYPE_NETWORK )
		{
			CMESingletonT<Queue>::Instance( pQueue );

			/**
			* Singleton::Instance( xxx )��������������һ������ʵ����
			* ����������ֻ����Ҫһ��ʵ�����������½���ɾ��ԭ��ʵ����
			* ���ȡΨһʵ���Ķ���
			*/
			ME_DELETE( pQueue );							// ɾ��ԭ��ʵ��
			pQueue = CMESingletonT<Queue>::Instance();	// ��ȡΨһʵ��
		}
	}
	else
	{
		/* ����Ѿ��������߳����͵��̴߳��ڣ����Ҵ˴������̵߳��������������߳����� */
		if ( iType == CMEThreadIDManager::CLASSIFICATION_TYPE_NETWORK )
		{
			pQueue = CMESingletonT<Queue>::Instance();
		}
		/* ������������߳����� */
		else
		{
			pQueue = new Queue;
		}
	}
}

ME_NAME_SPACE_END

