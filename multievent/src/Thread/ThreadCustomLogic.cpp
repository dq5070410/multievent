/**
 *  用户自定义逻辑线程对象类定义
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
     *  必须分别考虑阻塞模式和非阻塞模式下这个函数的行为，阻塞模式下因为用户需要
     *  在外层循环调用Run以达到执行用户逻辑的目的，所以OnStart是会被调用多次的，
     *  这时Worker返回不认为是线程结束，只有在标志退出状态时，再调用Run，这时才
     *  会执行OnStop。非阻塞模式的行为则完全不一样，Run只需要调用一次，Worker也
     *  只会调用一次，同样的，OnStart也是一次，当OnStart退出时，就认为线程这时需
     *  要退出，那么就必须调用OnStop，而通常情况下，用户控制OnStart退出时，会在
     *  调用Join之前，这时线程的状态不会立即置为退出状态，所以仅判断退出状态才调
     *  用OnStop很可能会导致OnStop没有机会被调用，所以，这里必须增加判断条件，分
     *  别适用于阻塞模式和非阻塞模式
     */

    {
        /** 
         *  非阻塞模式下存在多线程调用的情况下，加锁，防止线程对象的状态以外的变
         *  成RUNNING，不管怎样，线程的状态只能按NULL->RUNNING->EXIT顺序改变，当
         *  线程对象被Join以后，决不能再变成RUNNING状态
         */
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

        if ( RUN_STATE_NONE == m_iStatus )
        {
            m_iStatus = RUN_STATE_RUNNING;
        }
    }

    /* 阻塞模式 */
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
    /* 非阻塞模式 */
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

