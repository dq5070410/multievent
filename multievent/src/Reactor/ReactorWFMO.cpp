/**
 *  基于WaitForMultipleObjects的分离器类定义
 *
 *  History
 *  02/01/2016 create file
 */

#include "ReactorWFMO.h"
#include "ConnectionDefines.h"
#include "Reactor.h"
#include "IO.h"
#include "LockGuardT.h"
#include "WakenerEvent.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

CMEReactorWFMO::CMEReactorWFMO( IMEWakener* pWakener )
    : CMEReactorBase(
        pWakener,
        CMEReactor::REACTOR_TYPE_WFMO )
{
    INT iIndex = 0;
    for ( ; (UINT)iIndex < ME_WFMO_MAX_WAIT_NUM; ++iIndex )
    {
        m_EventArray[iIndex] = NULL;
        m_Slot[iIndex] = 0;             // 总觉得这里填0不好，但是填-1又有风险
    }

    /**
     *  Wakener比较特殊，目前只有在Reactor构造的时候注册到Reactor，并且，目前没
     *  有将其注销，就目前的实现，不会引发问题，但终究是不太对称，后面有机会再改
     */
    iIndex = -1;
    Register(
        pWakener,
        pWakener,
        CMEReactor::EVENT_TYPE_ACTIVE,
        iIndex );
}

CMEReactorWFMO::~CMEReactorWFMO()
{

}

ME_Result CMEReactorWFMO::HandleEvent()
{
    ME_INFO_TRACE_THIS( "" );

    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    ME_Result hResult = ME_ERROR_FAILURE;
    INT iStartIndex = 0;
    INT iTotalNum = m_ioItemMap.size();
    UINT iTimeout = GetWaitTime();   // 第一次等待，用基类提供的接口获取等待时间

    /**
     *  通过多次调用WaitForMultipleObjects来捕获所有激活的事件对象索引
     *  此处的++iStartIndex能够让分离过程总是从激活事件索引的后一个位置开始新的
     *  一次捕获
     */
    for ( ; iStartIndex < iTotalNum; ++iStartIndex )
    {
        DWORD iActivePos = WaitForMultipleObjects(
            iTotalNum - iStartIndex,
            &m_EventArray[iStartIndex],
            FALSE,
            iTimeout );

        /** 
         *  等到东西了，但是只有索引最小的一个，那么需要设置等待时间和起始事件索
         *  引，检查后续的事件句柄
         */
        if ( WAIT_FAILED != iActivePos && WAIT_TIMEOUT != iActivePos )
        {
            iTimeout = 0;
            /* 必须用iActivePos - WAIT_OBJECT_0方式来计算索引偏移 */
            iStartIndex += ( iActivePos - WAIT_OBJECT_0 );

            /* 目前对触发事件没有什么特别的处理，就是设置下激活状态 */
            INT iIOItemIndex = m_Slot[iStartIndex];
            ME_SET_BITS( m_ioItemMap[iIOItemIndex].iActiveEvent, CMEReactor::EVENT_TYPE_ACTIVE );
            ++m_iActiveIOItemNum;
        }
        /* 等待超时 */
        else if ( WAIT_TIMEOUT == iActivePos )
        {
            hResult = ME_ERROR_TIMEOUT;
            break;
        }
        /* 发生错误 */
        else
        {
            hResult = ME_ERROR_FAILURE;
            break;
        }
    }

    /**
     *  根据捕获到的激活的事件对象数量来决定返回值正常
     *  只有没有捕获到激活的事件对象时，才返回超时或错误
     *  因为上面的循环会尝试捕获所有激活的Event，但是最后一次捕获的结果很可能是
     *  超时（也不能排除发生错误的可能），这个时候也不能够算是超时，总应该让分离
     *  器进入分派过程将捕获到的事件处理掉
     */
    if ( 0 < m_iActiveIOItemNum )
    {
        hResult = ME_OK;
    }

    return hResult;
}

ME_Result CMEReactorWFMO::AddWaitableObject(
    IMEIO* pIO,
    INT iEvent,
    INT iIndex )
{
    ME_INFO_TRACE_THIS( "pIO: " << pIO
        << ", iEvent: " << iEvent
        << ", iIndex: " << iIndex );

    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    /** 
     *  WaitForMultipleObjects目前只接受Windows事件句柄，只有激活和非激活两种状
     *  态，而没有具体的事件类型（虽然目前激活包含了读和写两种事件类型），暂时就
     *  不支持修改了，已经存在的事件句柄不重复添加或者修改，以后要有修改功能，那
     *  么是需要删除当前的，再添加一个新的
     */
    INT iWaitableObjectIndex = 0;
    INT iTotalNum = m_ioItemMap.size();

    for (; iWaitableObjectIndex < iTotalNum; ++iWaitableObjectIndex)
    {
        /* 只要句柄一样，那么就认为是同一个 */
        if ( m_EventArray[iWaitableObjectIndex] == pIO->GetHandle() )
        {
            break;
        }
    }

    /** 
     *  这里和基类注册接口实现关联性太强，只有在先调用当前函数，后添加io item表
     *  的情况下，这个判断才成立，需要特别注意
     *  没有匹配的io事件句柄，添加进来
     */
    if ( iWaitableObjectIndex == iTotalNum )
    {
        m_EventArray[iWaitableObjectIndex] = pIO->GetHandle();
        m_Slot[iWaitableObjectIndex] = iIndex;
    }

    return ME_OK;
}

ME_Result CMEReactorWFMO::RemoveWaitableObject(
    IMEIO* pIO,
    INT iEvent,
    INT iIndex )
{
    ME_INFO_TRACE_THIS( "pIO: " << pIO
        << ", iEvent: " << iEvent
        << ", iIndex: " << iIndex );

    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    /** 
     *  在注销时，会对取消的事件类型进行预处理，如果不再关注某个事件句柄，那么所
     *  有的事件类型都必须被取消，因为基类判断一个io item是否移除是根据是否取消
     *  所有的事件类型，那么如果这里要和基类同步，就不能仅仅关注读和写事件类型，
     *  必须判断所有事件类型都需要取消时，才移除事件句柄，否则，会有麻烦的同步问
     *  题
     */
    INT iWaitableObjectIndex = 0;
    INT iTotalNum = m_ioItemMap.size();

    for (; iWaitableObjectIndex < iTotalNum; ++iWaitableObjectIndex)
    {
        /* 只要句柄一样，那么就认为是同一个 */
        if ( m_EventArray[iWaitableObjectIndex] == pIO->GetHandle() )
        {
            break;
        }
    }

    /** 
     *  找到事件句柄，并且需要移除它，那么就需要将它后方所有的事件句柄都逐一前移
     *  以后如果实现可移除部分事件类型，那么就需要先移除当前的，然后再填一个新的
     *  到当前的位置
     */
    if ( iWaitableObjectIndex < iTotalNum &&
        ME_BIT_CMP_MASK(iEvent, CMEReactor::EVENT_TYPE_ALL, CMEReactor::EVENT_TYPE_ALL) )
    {
        /* 这里要小心下标越界 */
        for ( ; iWaitableObjectIndex < iTotalNum - 1; ++iWaitableObjectIndex )
        {
            m_EventArray[iWaitableObjectIndex] = m_EventArray[iWaitableObjectIndex + 1];
            m_Slot[iWaitableObjectIndex] = m_Slot[iWaitableObjectIndex + 1];
        }

        /* 重置末尾的内容，先这么写着，如果不需要再删除 */
        m_EventArray[iWaitableObjectIndex] = NULL;
        m_Slot[iWaitableObjectIndex] = 0;
    }

    return ME_OK;
}

ME_NAME_SPACE_END

#endif	// ME_WIN