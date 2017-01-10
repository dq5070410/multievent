/**
 *  事件多路分离器实现基础类定义
 *
 *
 *  History
 *  01/30/2016 create file
 */

#include "Reactor.h"
#include "ReactorBase.h"
#include "LockGuardT.h"
#include "WakenerSocketPair.h"
#include "WakenerEvent.h"
#include "ConnectionDefines.h"

ME_NAME_SPACE_BEGIN

CMEReactorBase::CMEReactorBase(
    IMEWakener* pWakener,
    INT iType )
    : m_iActiveIOItemNum( 0 )
    , m_pWakener( pWakener )
    , m_iNewIndex( 0 )
    , m_iStatus( CMEReactor::RUN_STATE_RUNNING )
    , m_iType( iType )
    , m_pEventQueue( NULL )
    , m_pTimerQueue( NULL )
{

}

CMEReactorBase::~CMEReactorBase()
{
}

ME_Result CMEReactorBase::Notify( INT iNotifyType )
{
    ME_INFO_TRACE_THIS( "iNotifyType: " << iNotifyType );
    ME_ASSERTE_RETURN( (NULL != m_pWakener), ME_ERROR_NULL_POINTER );

    //ME_Result hResult = ME_ERROR_FAILURE;

    /* 唤醒模式 */
    if ( CMEReactor::NOTIFY_TYPE_WAKEUP == iNotifyType )
    {
        return m_pWakener->Notify();
    }

    /* 停止模式 */
    /* 不管怎么样都会设置停止状态 */
    m_iStatus = CMEReactor::RUN_STATE_EXIT;

    return m_pWakener->Notify();    // 唤醒Reactor
}

ME_Result CMEReactorBase::Register(
    IMEIO* pIO,
    IMEIOSink* pIOSink,
    INT iEvent,
    INT& iIndex )
{
    ME_INFO_TRACE_THIS( "pIO: " << pIO
        << ", pIOSink: " << pIOSink
        << ", iEvent: " << iEvent
        << ", iIndex: " << iIndex );

    ME_ASSERTE_RETURN( (ME_INVALID_HANDLE != pIO->GetHandle()), ME_ERROR_INVALID_ARG );

    CMELockGuardT<CMELockThreadMutex> hGuard( &m_hThreadMutex );

    ME_Result hResult = ME_ERROR_FAILURE;

    if ( -1 != iIndex )
    {
        HikIOItemMap::iterator iterIOItem = m_ioItemMap.find( iIndex );
        ME_ASSERTE_RETURN( (m_ioItemMap.end() != iterIOItem), ME_ERROR_NOT_FOUND );

        /* 通过注册只允许修改Event类型，如果试图修改IO对象或者IO Sink对象，都报错 */
        ME_ASSERTE_RETURN( (iterIOItem->second.pIO == pIO), ME_ERROR_NOT_FOUND );
        ME_ASSERTE_RETURN( (iterIOItem->second.pIOSink == pIOSink), ME_ERROR_FOUND );

        hResult = AddWaitableObject(
            pIO,
            iEvent,
            iterIOItem->first );

        /* 上面成功了再修改IO Item，减少回退 */
        if ( ME_SUCCEEDED(hResult) )
        {
            iterIOItem->second.iEvent = iEvent;

            /* modify by fy, 2013/05/13, fix bug #BG00B0038 */
            m_pWakener->Notify();
            /* modify by fy end */
        }

        iIndex = iterIOItem->first;
    }
    /* iIndex指定为-1，则是新增 */
    else
    {
        if ( IsFull() )
        {
            return ME_ERROR_REACH_MAX;
        }

        /* 拿一个新的Index以后再调用AddWaitableObject */
        iIndex = m_iNewIndex++;
        hResult = AddWaitableObject(
            pIO,
            iEvent,
            iIndex );

        if ( ME_SUCCEEDED(hResult) )
        {
            /* 初始化io item，目前只有在这里才会设置IOSink */
            ME_IO_ITEM stIOItem;
            stIOItem.pIO = pIO;
            stIOItem.pIOSink = pIOSink;
            stIOItem.iEvent = iEvent;
            stIOItem.iActiveEvent = CMEReactor::EVENT_TYPE_NULL;
            /* 插入io item表 */
            m_ioItemMap.insert( std::make_pair(
                iIndex,     // fix：iEvent->iIndex
                stIOItem) );

            m_pWakener->Notify();
        }
        else
        {
            iEvent = -1;    //lint -e438 iEvent is Output Parameter
        }
    }
    
    return hResult;
}

ME_Result CMEReactorBase::UnRegister(
    IMEIO* pIO,
    INT iEvent,
    INT iIndex )
{
    ME_INFO_TRACE_THIS( "pIO: " << pIO
        << ", iEvent: " << iEvent
        << ", iIndex: " << iIndex );

    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    ME_Result hResult = ME_ERROR_NOT_FOUND;

    /* 根据提供的Index来找 */
    HikIOItemMap::iterator iterIOItem = m_ioItemMap.find( iIndex );
    if ( m_ioItemMap.end() == iterIOItem )
    {
        /* 目前，如果注销的时候找不到这个IO Item，那么就认为已经成功了 */
        ME_WARNING_TRACE_THIS( "Unregister but not found io item, index: " << iIndex );

        return ME_OK;
    }

    ME_ASSERTE_RETURN( (iterIOItem->second.pIO == pIO), ME_ERROR_FAILURE );

    /**
     *  因为目前多路分离机制等待的也是复合的事件类型，取消时需要进行一定的处理避
     *  免取消过多或者过少，此外还需要考虑select和WaitForMultipleObjects的特性，
     *  尽量兼容两者的RemoveWaitableObject接口，不过这里的实现很有可能会在后续版
     *  本升级中发生改变，因为这里需要进行兼容性的开发，得考虑是否有更好的解决方
     *  案
     */

    /* 对事件类型进行一定的预处理 */
    INT iResultEvent( 0 );
    INT iRemoveEvent( 0 );
    ME_SET_BITS( iResultEvent, (iterIOItem->second.iEvent & ~((DWORD)iEvent)) );

    /* 不管怎样，都需要告诉RemoveWaitableObject，用户取消了哪些类型 */
    ME_SET_BITS( iRemoveEvent, iEvent );

    /* 如果不关注可读以及接受客户端的类型，那么需要设置为这两种类型都取消 */
    /* 这样，派生类就知道这种复合类型已经完全不需要关注了 */
    if ( ME_BIT_DISABLED(iResultEvent, CMEReactor::EVENT_TYPE_IN) )
    {
        ME_SET_BITS( iRemoveEvent, CMEReactor::EVENT_TYPE_IN );
    }

    if ( ME_BIT_DISABLED(iResultEvent, CMEReactor::EVENT_TYPE_OUT) )
    {
        ME_SET_BITS( iRemoveEvent, CMEReactor::EVENT_TYPE_OUT );
    }

    if ( ME_BIT_DISABLED(iResultEvent, CMEReactor::EVENT_TYPE_ERR) )
    {
        ME_SET_BITS( iRemoveEvent, CMEReactor::EVENT_TYPE_ERR );
    }

    /**
     *  对派生类RemoveWaitableObject的要求是，检查某一复合类型时，当两种都要
     *  求取消时，才予以取消 
     */
    hResult = RemoveWaitableObject(
        pIO,
        iRemoveEvent,
        iterIOItem->first );

    /* 上面成功才改变IO Item */
    if ( ME_SUCCEEDED(hResult) )
    {
        /* 预判是否会完全取消 */
        if ( ME_BIT_CMP_MASK(iterIOItem->second.iEvent, iEvent, iterIOItem->second.iEvent) )
        {
            m_ioItemMap.erase( iterIOItem );
        }
        else
        {
            ME_CLR_BITS( iterIOItem->second.iEvent, (DWORD)iEvent );
        }
    }

    return hResult;
}

ME_Result CMEReactorBase::AttachEventQueue( CMEEventQueue* pEq )
{
    ME_INFO_TRACE_THIS( "pEq: " << pEq );
    ME_ASSERTE_RETURN( (NULL == m_pEventQueue), ME_ERROR_FOUND );

    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );
    ME_ASSERTE_RETURN( (NULL == m_pEventQueue), ME_ERROR_FOUND );
    m_pEventQueue = pEq;

    return ME_OK;
}

ME_Result CMEReactorBase::AttachTimerQueue( CMETimerQueue* pTq )
{
    ME_INFO_TRACE_THIS( "pTq: " << pTq );
    ME_ASSERTE_RETURN( (NULL == m_pTimerQueue), ME_ERROR_FOUND );

    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );
    ME_ASSERTE_RETURN( (NULL == m_pTimerQueue), ME_ERROR_FOUND );
    m_pTimerQueue = pTq;

    return ME_OK;
}

ME_Result CMEReactorBase::Dispatch( BOOL bExit /* = FALSE */ )
{
    ME_INFO_TRACE_THIS( "bExit: " << bExit );
    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    /** 
     *  IO事件回调，这里需要一个临时的空间来存放所有发生事件的IO对象，否则，在回
     *  掉过程中如果进行了注销，那么势必会造成IO Item Map的变化，所有的Iterator
     *  都会失效，然后就是崩溃
     *
     */

    std::vector<ME_IO_ITEM*> vecActiveIOItem;
    std::vector<ME_IO_ITEM*>::iterator iterActiveIOItem;

    /* 先把发生事件的IO对象都找出来，对Active计数也做检查，减少遍历数量 */
    HikIOItemMap::iterator iterIOItem = m_ioItemMap.begin();
    for ( ; iterIOItem != m_ioItemMap.end() && m_iActiveIOItemNum > 0; ++iterIOItem )
    {
        if ( iterIOItem->second.iActiveEvent )
        {
            vecActiveIOItem.push_back( &(iterIOItem->second) );
            /* Active计数减1 */
            --m_iActiveIOItemNum;
        }
    }

    /* 再对所有发生事件的IO对象进行回调 */
    for ( iterActiveIOItem = vecActiveIOItem.begin();
          iterActiveIOItem != vecActiveIOItem.end();
          ++iterActiveIOItem )
    {
        /** 
         *  在回调前必须重置，WaitForMultipleObjects和Select反馈事件类型的方式不
         *  同，前者不会遍历所有的io item自动完成反馈前重置（后者可以），这里就
         *  有必要完成这样的工作
         *  不能用ME_SET_BITS宏去设置0
         */
        INT iActiveEvent = (*iterActiveIOItem)->iActiveEvent;
        ME_CLR_BITS( (*iterActiveIOItem)->iActiveEvent, CMEReactor::EVENT_TYPE_ALL );

        if ( ME_BIT_ENABLED(iActiveEvent, CMEReactor::EVENT_TYPE_IN) )
        {
            (*iterActiveIOItem)->pIOSink->OnInput(
                (*iterActiveIOItem)->pIO->GetHandle(),
                iActiveEvent );
        }

        if ( ME_BIT_ENABLED(iActiveEvent, CMEReactor::EVENT_TYPE_OUT) )
        {
            (*iterActiveIOItem)->pIOSink->OnOutput(
                (*iterActiveIOItem)->pIO->GetHandle(),
                iActiveEvent );
        }

        if ( ME_BIT_ENABLED(iActiveEvent, CMEReactor::EVENT_TYPE_ERR) )
        {
            (*iterActiveIOItem)->pIOSink->OnClose(
                (*iterActiveIOItem)->pIO->GetHandle(),
                iActiveEvent );
        }
    }

    /* 通知事件队列处理事件 */
    if ( NULL != m_pEventQueue )
    {
        /* 和HikReactor.cpp的Run配合，如果退出，那么最后一次分派要搞定所有事件 */
        do 
        {
            BOOL bEventQueueExit = FALSE;
            m_pEventQueue->HandleEvent( bEventQueueExit );

            /* 如果这个返回TRUE，那么表示事件都回调完了，跳出循环 */
            if ( bEventQueueExit )
            {
                break;
            }

        } while ( bExit );
    }

    /*
     *  最后在处理时钟，因为到Dispatch的时候，有以下几种情况：
     *  事件队列在上一轮没有处理完所有事件，这轮没有系统IO的情况下，直接超时进入
     *  Dispatch，因为等待的超时时间是0，所以在进入Dispatch的时候，很可能没有任
     *  何时钟到期，先处理完一些事件以后再检查时钟队列，如果存在到期时钟，那么这
     *  一轮就能处理掉，否则，很可能要在下一轮才会处理到期时钟
     *  在等待第一个到期时钟的超时时间后超时进入Dispatch，这时通常没有自定义事件
     *  也没有系统IO事件，那么执行到这里应该不会花去多少时间，超时时钟还是能够及
     *  时处理掉的
     *  捕获到系统IO事件，有可能是有系统IO需要处理，也有可能是有新的自定事件或时
     *  钟，进入Dispatch后，时钟队列原有的时钟到期的可能性不是很大，而新添加的时
     *  钟立即到期也只是有一定概率，所以还是考虑先搞定自定义事件和系统IO事件再处
     *  理时钟队列，这个时候，如果有到期时钟，那么还是能够在这一轮处理完的
     */
    if ( NULL != m_pTimerQueue )
    {
        if ( !bExit )
        {
            m_pTimerQueue->Expire();
        }
        else
        {
            m_pTimerQueue->Destroy();
        }
    }

    return m_iStatus;   // 如果分离器状态发生变化，那么在这里返回让CMEReactor知道
}

/* 2013.9.4 added by 韦珂 */
int CMEReactorBase::GetType() const
{
	return m_iType;
}

UINT CMEReactorBase::GetWaitTime()
{
    ME_INFO_TRACE_THIS( "" );

    UINT iWaitTime = ME_INFINITE;
    do
    {
        /* 事件队列不空就设置等待时间为0 */
        if ( NULL != m_pEventQueue )
        {
            if ( FALSE == m_pEventQueue->IsEmpty() )
            {
                iWaitTime = 0;
                break;
            }
        }

        /* 事件队列空就看时钟队列最近到期的时钟 */
        if ( NULL != m_pTimerQueue )
        {
            iWaitTime = m_pTimerQueue->GetNextTimeout();
        }

    } while ( FALSE );

    return iWaitTime;
}

BOOL CMEReactorBase::IsFull()
{
    ME_INFO_TRACE_THIS( "" );

    if ( (CMEReactor::REACTOR_TYPE_SELECT == m_iType && m_ioItemMap.size() >= ME_SELECT_MAX_WAIT_NUM) ||
         (CMEReactor::REACTOR_TYPE_WFMO == m_iType && m_ioItemMap.size() >= ME_WFMO_MAX_WAIT_NUM) )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

ME_NAME_SPACE_END

