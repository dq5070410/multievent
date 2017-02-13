/**
 *  线程对象工厂类定义
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
     *  这里用ME_OK作为初始值，因为在没有Reactor的时候，认为构造线程对象的准备
     *  工作是正常的
     */
    ME_Result hResult = ME_OK;
    CEventQueue* pEventQueue = NULL;
    CMETimerQueue* pTimerQueue = NULL;
    CMEReactor* pReactor = NULL;

	/* 2013.9.3 added by 韦珂 */
	CMEThreadReactor* pNetworkThread = (CMEThreadReactor*)CMEThreadManagerSingleton::Instance()->GetNetworkThread();

    /**
        *  先根据线程对象所具有的属性来创建所需要的资源，如果这些资源创建出现问
        *  题，那么总是能够通过异常或是错误码判断进入错误处理逻辑而不会进行线程
        *  对象的创建，并且try和catch块都不能因为错误直接返回，必须进入错误处理
        *  逻辑回收已经创建的资源
        */
    if ( ME_BIT_ENABLED(iThreadType, IMEThreadManager::ME_THREAD_EVENT_QUEUE) )
    {
        /* 事件队列，new的时候如果抛出Exception，能够被catch到 */
        //pEventQueue = new CMEEventQueue;

		CreateQueue( 
			pEventQueue, 
			iClassificationType );
    }

    if ( ME_BIT_ENABLED(iThreadType, IMEThreadManager::ME_THREAD_TIMER_QUEUE) )
    {
        /* 时钟队列，new的时候如果抛出Exception，能够被catch到 */
        //pTimerQueue = new CMETimerQueue;
	
		CreateQueue( 
			pTimerQueue, 
			iClassificationType );
    }

    if ( ME_BIT_ENABLED(iThreadType, IMEThreadManager::ME_THREAD_REACTOR) )
    {
        /* 分离器，因为不会抛出Exception，那么就需要对返回值进行判断 */
        hResult = CMEReactorFactorySingleton::Instance()->Create(
            iReactorType,
			iClassificationType,
            pReactor );
    }

    /* 主要用来判断当需要Reactor时，Reactor是否成功创建 */
    if ( ME_SUCCEEDED(hResult) )
    {
        /* 根据线程对象的类型来分别创建对应的线程对象实例 */
        switch ( iThreadType )
        {
        case IMEThreadManager::ME_THREAD_CUSTOM_LOGIC:
            {
                /* 这种类型强制要求传入用户逻辑回调 */
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

        default:    // 其他所有情况都认为是异常输入，但是不能直接返回
            {
                hResult = ME_ERROR_INVALID_ARG;
                break;
            }
        }
    }

    /* 失败的情况下需要进行资源回收 */
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
				* 异步Reactor时，事件EventQueue、TimerQueue、ReactorImpl均为单例，
				* 但CMEReactor是new出来的，即每个网络线程均有一个CMEReactor实例，
				* 需要删除，ReactorFactory中保证了异步Reactor只删CMEReactor，不删
				* ReactorImpl，因此这里直接调用Destroy()即可
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

	/* 如果是第一次生成网络类型的线程 */
	if ( NULL == pNetworkThread )
	{
		pQueue = new Queue;
		if ( iType == CMEThreadIDManager::CLASSIFICATION_TYPE_NETWORK )
		{
			CMESingletonT<Queue>::Instance( pQueue );

			/**
			* Singleton::Instance( xxx )函数会重新生成一个对象实例，
			* 再由于我们只是需要一份实例，所以以下进行删除原有实例，
			* 后获取唯一实例的动作
			*/
			ME_DELETE( pQueue );							// 删除原有实例
			pQueue = CMESingletonT<Queue>::Instance();	// 获取唯一实例
		}
	}
	else
	{
		/* 如果已经有网络线程类型的线程存在，而且此次生成线程的请求又是网络线程类型 */
		if ( iType == CMEThreadIDManager::CLASSIFICATION_TYPE_NETWORK )
		{
			pQueue = CMESingletonT<Queue>::Instance();
		}
		/* 如果不是网络线程类型 */
		else
		{
			pQueue = new Queue;
		}
	}
}

ME_NAME_SPACE_END

