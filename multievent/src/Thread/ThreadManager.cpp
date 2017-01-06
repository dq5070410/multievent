/**
 *  线程对象管理器实现类定义
 *
 *
 *  History
 *  02/18/2016 create file
 *
 *  03/09/2016 fix deadlock
 *             在线程对象管理器运行期间，存在多种操作访问线程表，其中注册和注销
 *             两种操作会改变线程表的内容（当然初始化和反初始化也会，但这两个操
 *             作都是在非运行期间），而JoinAll以及GetThread需要对线程表进行遍历
 *             来停止或查找线程对象，故在上述的过程中都采用了线程锁来进行同步，
 *             问题在于JoinAll过程（主线程）中，存在线程对象在回调自定义事件时，
 *             需要调用GetThread（线程对象所在线程）这时就会造成JoinAll先获得锁
 *             但等待线程对象停止，线程对象尝试GetThread被锁住而无法停止，导致死
 *             锁
 *             解决方法只有减少锁的范围，对于用户来说，初始化和反初始化都是不可
 *             直接访问的，并且是在组件初始化和反初始化期间由主线程调用，只有运
 *             行期间的创建和销毁过程会调用注册和注销访问线程表，而JoinAll操作属
 *             于组件退出期间的行为，这时也应不能够有外部的注册和注销行为，这时
 *             就能够避免在JoinAll期间使用锁来保护线程表，那么应该就能够避免上述
 *             死锁的问题
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
        /* 创建线程对象 */
        hResult = CreateThread(
            bBlock,
            iType,
            CMEThreadIDManager::CLASSIFICATION_TYPE_EXTERNAL,
            pThreadSink,
            pCMEThread );

        ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

        /* 用户接口需要注册线程对象 */
        hResult = Register( pCMEThread );
        if ( ME_FAILED(hResult) )
        {
            break;
        }

        /**
         * 非阻塞的条件下，用户接口还需要启动线程对象；阻塞模式则不能在这里启动，
         * 带有Reactor的线程对象会被阻塞
         */
        if ( !bBlock )
        {
            hResult = pCMEThread->Run();
            if ( ME_FAILED(hResult) )
            {
                break;
            }
        }

        /* 成功的情况下才输出线程对象实例 */
        pThread = pCMEThread;

    } while ( FALSE );

    /* 创建、注册和启动过程如果发生错误，那么需要注销并销毁线程对象 */
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
    /* 用户接口不允许销毁非外部线程 */
    ME_ASSERTE_RETURN( m_ThreadIDManager.IsExternalThread(pCMEThread->GetThreadID()), ME_ERROR_NOT_AVAILABLE );
    /* 没有停止的线程对象不允许销毁 */
    ME_ASSERTE_RETURN( (CMEThread::RUN_STATE_EXIT == pCMEThread->m_iStatus), ME_ERROR_NOT_AVAILABLE );

    ME_Result hResult = ME_ERROR_FAILURE;
    do
    {
        /* 这里无视hResult的想法是尽量完成线程对象的销毁，todo 是否合适再考虑 */
        /* 注销线程对象 */
        hResult = Unregister( pCMEThread );
        /* 销毁线程对象 */
        hResult = DestroyThread( pCMEThread );

    } while ( FALSE );

    return hResult;
}

ME_Result CMEThreadManager::Join( IMEThread* pThread )
{
    ME_INFO_TRACE_THIS( "" );
    CMEThread* pCMEThread = dynamic_cast<CMEThread*>( pThread );
    ME_ASSERTE_RETURN( (NULL != pCMEThread), ME_ERROR_INVALID_ARG );
    /* 用户模式不允许停止非外部线程 */
    ME_ASSERTE_RETURN( m_ThreadIDManager.IsExternalThread(pThread->GetThreadID()), ME_ERROR_INVALID_ARG );

    ME_Result hResult = Join(
        pCMEThread,
        ME_INFINITE );

    return hResult;
}

IMEThread* CMEThreadManager::GetCurrentThread()
{
    ME_INFO_TRACE_THIS( "" );

    /* 用当前的系统ID去找线程对象 */
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
 *  以下都是线程对象管理器面向组件开发的接口
 ******************************************************************************/

ME_Result CMEThreadManager::Initialize(
    IMEThreadSink* pThreadSink /* = NULL */,
    BOOL bBlockMode /* = FALSE */ )
{
    ME_INFO_TRACE_THIS( "bBlockMode: " << bBlockMode );

    ME_Result hResult = ME_ERROR_FAILURE;

    CMEThread* pThread = NULL;

    /* 保护m_bRunning，并且在初始化期间不允许有其他的注册或注销行为 */
    CMELockGuardT<CMELockThreadMutex> Gurad( &m_hThreadMutex );

    /* 开放注册注销 */
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

        /* 非阻塞模式则在创建好以后就启动线程，阻塞模式则让用户自己去启动 */
        if ( !bBlockMode )
        {
            hResult = pThread->Run();
            if ( ME_FAILED(hResult) )
            {
                break;
            }
        }

    } while ( FALSE );

    /* 注册或是启动失败都会到这里回收创建出来的线程对象 */
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

    /* 当前仅允许创建一个用户线程对象 */
    ME_ASSERTE_RETURN( (1 == iUserThreadNum), ME_ERROR_INVALID_ARG );

    ME_Result hResult = ME_ERROR_FAILURE;

    /* 如果在Initialize的时候创建了一个用户线程对象，那么这里就要减去 */
    iUserThreadNum -= m_mapThreads.size();

    /* 创建用户线程 */
    while ( 0 < iUserThreadNum )
    {
        CMEThread* pThread = NULL;

        /* 创建用户线程 */
        hResult = CreateThread(
            FALSE,
            ME_THREAD_EVENT_QUEUE_TIMER_QUEUE_REACTOR,
            CMEThreadIDManager::CLASSIFICATION_TYPE_USER,
            NULL,
            pThread );

        ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

        /* 注册用户线程 */
        hResult = Register( pThread );
        if ( ME_FAILED(hResult) )
        {
            DestroyThread( pThread );

            return hResult;
        }

        /* 启动用户线程 */
        hResult = pThread->Run();
        if ( ME_FAILED(hResult) )
        {
            Unregister( pThread );
            DestroyThread( pThread );

            return hResult;
        }

        --iUserThreadNum;
    }

    /* 初始化网络线程 */
    while ( 0 < iNetworkThreadNum )
    {
        CMEThread* pThread = NULL;

        /* 创建网络线程 */
        hResult = CreateThread(
            FALSE,
            ME_THREAD_EVENT_QUEUE_TIMER_QUEUE_REACTOR,
            CMEThreadIDManager::CLASSIFICATION_TYPE_NETWORK,
            NULL,
            pThread );

        ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

        /* 注册网络线程 */
        hResult = Register( pThread );
        if ( ME_FAILED(hResult) )
        {
            DestroyThread( pThread );

            return hResult;
        }

        /* 启动网络线程 */
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

    /* 不管怎么样，都再JoinAll一遍，确保线程对象都已经被停止 */
    JoinAll();

    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    ME_Result hLastError = ME_OK;
    ME_Result hResult = ME_ERROR_FAILURE;

    /* 线程对象都已经停止了，所以逐一销毁并清空线程对象表即可 */
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

    /* 先申请线程对象ID */
    INT iThreadID = m_ThreadIDManager.GenerateThreadID( iClassificationType );
    ME_ASSERTE_RETURN( (0 != iThreadID), ME_ERROR_REACH_MAX );

    /* 默认就用基于WaitForMultipleObjects的分离器 */
    INT iReactorType = CMEReactor::REACTOR_TYPE_WFMO;

#if (defined ME_LINUX)
    /* Linux下没有WaitForMultipleObjects，所以要换成基于select的Reactor */
    iReactorType = CMEReactor::REACTOR_TYPE_SELECT;
#endif

    /* 网络线程在当前版本也需要用基于select的Reactor */
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
    /* 线程对象能够被Destroy，有两个条件，一个是不能处于运行状态，另一个是线程句柄为空 */
    ME_ASSERTE_RETURN( (CMEThread::RUN_STATE_RUNNING != pThread->m_iStatus), ME_ERROR_NOT_AVAILABLE );
    ME_ASSERTE_RETURN( (NULL == pThread->GetThreadHandle()), ME_ERROR_NOT_AVAILABLE );

    /* 一旦判断通过，那么无论如何都不允许线程再启动 */
    pThread->Join(
        ME_INFINITE,
        FALSE );

    return CMEThreadFactorySingleton::Instance()->Destroy( pThread );
}

ME_Result CMEThreadManager::Register( CMEThread* pThread )
{
    ME_INFO_TRACE_THIS( "" );
    ME_ASSERTE_RETURN( (NULL != pThread), ME_ERROR_INVALID_ARG );

    /* 运行期间对线程对象表进行操作必须用锁来保护 */
    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    /* 禁止注册则直接返回，这样JoinAll过程就不需要锁来保护线程对象表了 */
    ME_ASSERTE_RETURN( m_bEnableRegister, ME_ERROR_NOT_INITIALIZED );

    /* 如果找到则不重复注册 */
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

    /* 运行期间对线程对象表进行操作必须用锁来保护 */
    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    /* 禁止注销则直接返回，这样JoinAll过程就不需要锁来保护线程对象表了 */
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
         *  将锁的范围限制到最小，当JoinAll的时候，先尝试将状态置为非运行状态
         *  只要执行完当前代码块，那么当前不会有，之后也不会有线程调用注册或者注
         *  销过程，JoinAll过程也就不必担心有线程改变线程对象表了（组件的初始化
         *  和反初始化都只在主线程调用的情况下），也就没必要用锁来保护线程对象表
         *  了
         */
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

        /* 禁止注册和注销操作，注册和注销都会直接返回错误而不会操作线程对象表 */
        m_bEnableRegister = FALSE;
    }

    /* 首先把外部线程都停掉 */
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

        /* 这里要先停网络线程再停用户线程 */
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
     *  因为GetThread在运行期间是会被调用的，这时也存在注册和注销的过程，所以需
     *  要对线程对象表进行保护，防止在遍历查找过程中因为线程对象表改变导致迭代器
     *  失效
     */
    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    /** 
     *  先简单实现，总是选择找到的目标分类的首个线程对象，以后有负载均衡机制时，
     *  再重新写这部分算法
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

