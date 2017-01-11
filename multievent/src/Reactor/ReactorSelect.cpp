/**
 *  基于Select的分离器类定义
 *
 *
 *  History
 *  02/01/2016 create file
 */


#include "ReactorSelect.h"
#include "ConnectionDefines.h"
#include "Reactor.h"
#include "LockGuardT.h"
#include "WakenerSocketPair.h"


ME_NAME_SPACE_BEGIN


CMEReactorSelect::CMEReactorSelect( IMEWakener* pWakener )
    : CMEReactorBase(
        pWakener,
        CMEReactor::REACTOR_TYPE_SELECT )
#if defined( ME_LINUX )
    , m_iMasterNFDS( 0 )
#endif
{
#if defined( ENABLE_FD_SET_COPY )       // 整体拷贝模式
    FD_ZERO( &m_MasterFDSetIn );
    FD_ZERO( &m_MasterFDSetOut );
    FD_ZERO( &m_MasterFDSetErr );
#endif

    /**
     *  Wakener比较特殊，目前只有在Reactor构造的时候注册到Reactor，并且，目前没
     *  有将其注销，就目前的实现，不会引发问题，但终究是不太对称，后面有机会再改
     */
    INT iIndex = -1;
    Register(
        pWakener,
        pWakener,
        CMEReactor::EVENT_TYPE_READ,
        iIndex );
}

CMEReactorSelect::~CMEReactorSelect()
{

}

ME_Result CMEReactorSelect::HandleEvent()
{
    ME_INFO_TRACE_THIS( "" );

    ME_Result hResult = ME_ERROR_FAILURE;

    /* select过程的传入参数声明 */

    /* Working组的fd_set */
    fd_set stWorkingFDSetIn;
    fd_set stWorkingFDSetOut;
    fd_set stWorkingFDSetErr;

    hResult = InitWorkingFDSet(
        stWorkingFDSetIn,
        stWorkingFDSetOut,
        stWorkingFDSetErr );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    /* 计算等待超时时间 */
    UINT iTimeout = GetWaitTime();   // 用基类提供的接口获取等待时间
    timeval tvTimeout;

    if ( (UINT)ME_INFINITE != iTimeout )
    {
        tvTimeout.tv_sec = iTimeout / 1000;
        tvTimeout.tv_usec = iTimeout % 1000 * 1000;
    }

    /* 进行select */

#if defined( ME_WIN )
    //lint -e{645} 对tvTimeout的警告是误判
    /* Windows下第一个参数固定填0 */
    hResult = ::select(
        0,
        &stWorkingFDSetIn,
        &stWorkingFDSetOut,
        &stWorkingFDSetErr,
        ME_INFINITE != iTimeout ? &tvTimeout : NULL );

    ME_ASSERTE_RETURN( (SOCKET_ERROR != hResult), ME_ERROR_NETWORK_SOCKET_ERROR );

#elif defined( ME_LINUX )
    /* Linux下第一个参数要用到NFDS */
	int iWorkingNFDS = m_iMasterNFDS;
    hResult = ::select(
        iWorkingNFDS + 1,
        &stWorkingFDSetIn,
        &stWorkingFDSetOut,
        &stWorkingFDSetErr,
        (UINT)ME_INFINITE != iTimeout ? &tvTimeout : NULL );

    ME_ASSERTE_RETURN( (-1 != hResult), ME_ERROR_NETWORK_SOCKET_ERROR );

#endif

    FeedbackWorkingFDSet(
        stWorkingFDSetIn,
        stWorkingFDSetOut,
        stWorkingFDSetErr,
        hResult );

    return ME_OK;
}

ME_Result CMEReactorSelect::AddWaitableObject(
    IMEIO* pIO,
    INT iEvent,
    INT iIndex )
{
    ME_INFO_TRACE_THIS( "pIO: " << pIO
        << ", iEvent: " << iEvent
        << ", iIndex: " << iEvent );

#if defined( ENABLE_FD_SET_COPY )
    if ( ME_BIT_ENABLED(iEvent, CMEReactor::EVENT_TYPE_IN) )
    {
        FD_SET( (ME_SOCKET)pIO->GetHandle(), &m_MasterFDSetIn );
    }

    if ( ME_BIT_ENABLED(iEvent, CMEReactor::EVENT_TYPE_OUT) )
    {
        FD_SET( (ME_SOCKET)pIO->GetHandle(), &m_MasterFDSetOut );
    }

    if ( ME_BIT_ENABLED(iEvent, CMEReactor::EVENT_TYPE_ERR) )
    {
        FD_SET( (ME_SOCKET)pIO->GetHandle(), &m_MasterFDSetErr );
    }

    #if defined( ME_LINUX )
    /* Linux下需要更新nfds */
	if ( m_iMasterNFDS < pIO->GetHandle() )
	{
	    m_iMasterNFDS = pIO->GetHandle();
	}
    //if ( m_iMasterNFDS < iterIOItem->second.pIO->GetHandle() )
    //{
    //    m_iMasterNFDS = iterIOItem->second.pIO->GetHandle();
    //}
    #endif
#endif

    return ME_OK;
}

ME_Result CMEReactorSelect::RemoveWaitableObject(
    IMEIO* pIO,
    INT iEvent,
    INT iIndex )
{
    ME_INFO_TRACE_THIS( "pIO: " << pIO
        << ", iEvent: " << iEvent
        << ", iIndex: " << iEvent );

#if defined( ENABLE_FD_SET_COPY )
    /* 只有可读事件和接受到连接事件类型都要求取消时，才清除fd_set中对应的fd */
    if ( ME_BIT_CMP_MASK(iEvent, CMEReactor::EVENT_TYPE_IN, CMEReactor::EVENT_TYPE_IN) )
    {
        FD_CLR( (ME_SOCKET)pIO->GetHandle(), &m_MasterFDSetIn );
    }

    if ( ME_BIT_CMP_MASK(iEvent, CMEReactor::EVENT_TYPE_OUT, CMEReactor::EVENT_TYPE_OUT) )
    {
        FD_CLR( (ME_SOCKET)pIO->GetHandle(), &m_MasterFDSetOut );
    }

    if ( ME_BIT_CMP_MASK(iEvent, CMEReactor::EVENT_TYPE_ERR, CMEReactor::EVENT_TYPE_ERR) )
    {
        FD_CLR( (ME_SOCKET)pIO->GetHandle(), &m_MasterFDSetErr );
    }

    #if defined( ME_LINUX )
    /* Linux下需要判断是否更新nfds */
    //if ( m_iMasterNFDS == iterIOItem->second.pIO->GetHandle() &&
    //     CMEReactor::EVENT_TYPE_ALL == iEvent )
    //{
    //    m_iMasterNFDS = 0;  // 先置0
    //    HikIOItemMap::iterator iterIOItem = m_ioItemMap.begin();
    //    for ( ; iterIOItem != m_ioItemMap.end(); ++iterIOItem )
    //    {
    //        if ( m_iMasterNFDS < iterIOItem->second.pIO->GetHandle() )
    //        {
    //            m_iMasterNFDS = iterIOItem->second.pIO->GetHandle();
    //        }
    //    }
    //}

	if ( m_iMasterNFDS == pIO->GetHandle() &&
		CMEReactor::EVENT_TYPE_ALL == iEvent )
	{
		m_iMasterNFDS = 0;  // 先置0
		HikIOItemMap::iterator iterIOItem = m_ioItemMap.begin();
		for ( ; iterIOItem != m_ioItemMap.end(); ++iterIOItem )
		{
			if ( m_iMasterNFDS < iterIOItem->second.pIO->GetHandle() )
			{
				m_iMasterNFDS = iterIOItem->second.pIO->GetHandle();
			}
		}
	}
    #endif
#endif

    return ME_OK;
}

ME_Result CMEReactorSelect::InitWorkingFDSet(
    fd_set& stWorkingFDSetIn,
    fd_set& stWorkingFDSetOut,
    fd_set& stWorkingFDSetErr )
{
    FD_ZERO( &stWorkingFDSetIn );
    FD_ZERO( &stWorkingFDSetOut );
    FD_ZERO( &stWorkingFDSetErr );

    /* select的item数量，至少为1，且不能够超过ME_SELECT_MAX_WAIT_NUM */
    UINT iItemsNum = m_ioItemMap.size();
    ME_ASSERTE_RETURN( (0 < iItemsNum && ME_SELECT_MAX_WAIT_NUM >= iItemsNum), ME_ERROR_FAILURE );

#if defined( ME_LINUX )
    /* Linux下需要它作为select的第一个参数 */
    INT iWorkingNFDS = 0;
#endif

    /* 设置Working组的fd_set */
    {
        /* Lock m_hThreadMutex */
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

#if defined( ENABLE_FD_SET_COPY )       // 整体拷贝模式
	#ifdef ME_WIN
        ::memcpy_s(
            &stWorkingFDSetIn,
            sizeof(fd_set),
            &m_MasterFDSetIn,
            sizeof(fd_set) );

        ::memcpy_s(
            &stWorkingFDSetOut,
            sizeof(fd_set),
            &m_MasterFDSetOut,
            sizeof(fd_set) );

        ::memcpy_s(
            &stWorkingFDSetErr,
            sizeof(fd_set),
            &m_MasterFDSetErr,
            sizeof(fd_set) );

	#else
		::memcpy(
			&stWorkingFDSetIn,
			&m_MasterFDSetIn,
			sizeof(fd_set) );

		::memcpy(
			&stWorkingFDSetOut,
			&m_MasterFDSetOut,
			sizeof(fd_set) );

		::memcpy(
			&stWorkingFDSetErr,
			&m_MasterFDSetErr,
			sizeof(fd_set) );

	#endif	// ME_WIN

#if defined( ME_LINUX )
        iWorkingNFDS = m_iMasterNFDS;   // Linux下需要从Master组拿到nfds
#endif

#else   // 逐一设置模式
        HikIOItemMap::iterator iterIOItem = m_ioItemMap.begin();
        for ( ; iterIOItem != m_ioItemMap.end(); ++iterIOItem )
        {
            if ( ME_BIT_ENABLED(iterIOItem->second.iEvent, CMEReactor::EVENT_TYPE_IN) )
            {
                FD_SET( (ME_SOCKET)iterIOItem->second.pIO->GetHandle(), &stWorkingFDSetIn );
            }

            if ( ME_BIT_ENABLED(iterIOItem->second.iEvent, CMEReactor::EVENT_TYPE_OUT) )
            {
                FD_SET( (ME_SOCKET)iterIOItem->second.pIO->GetHandle(), &stWorkingFDSetOut );
            }

            if ( ME_BIT_ENABLED(iterIOItem->second.iEvent, CMEReactor::EVENT_TYPE_ERR) )
            {
                FD_SET( (ME_SOCKET)iterIOItem->second.pIO->GetHandle(), &stWorkingFDSetErr );
            }

#if defined( ME_LINUX )
            /* Linux下需要更新nfds */
            if ( iWorkingNFDS < iterIOItem->second.pIO->GetHandle() )
            {
                iWorkingNFDS = iterIOItem->second.pIO->GetHandle();
            }
#endif
        }
#endif
    }   // Unlock m_hThreadMutex

    return ME_OK;
}

void CMEReactorSelect::FeedbackWorkingFDSet(
    fd_set& stWorkingFDSetIn,
    fd_set& stWorkingFDSetOut,
    fd_set& stWorkingFDSetErr,
    INT iActiveNum  )
{
    /* Lock m_hThreadMutex */
    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    m_iActiveIOItemNum = 0;     // 先将活跃的IO对象计数清零

    /* 增加m_iActiveIOItemNum < hResult判断以减少不必要的遍历 */
    HikIOItemMap::iterator iterIOItem = m_ioItemMap.begin();
    for ( ; iterIOItem != m_ioItemMap.end() && m_iActiveIOItemNum < iActiveNum; ++iterIOItem )
    {
        /* 先置0 */
        iterIOItem->second.iActiveEvent = CMEReactor::EVENT_TYPE_NULL;

        /* 检查并设置发生的事件 */
        if ( FD_ISSET(iterIOItem->second.pIO->GetHandle(), &stWorkingFDSetIn) )
        {
            iterIOItem->second.iActiveEvent = CMEReactor::EVENT_TYPE_IN;
        }

        if ( FD_ISSET(iterIOItem->second.pIO->GetHandle(), &stWorkingFDSetOut) )
        {
            iterIOItem->second.iActiveEvent = CMEReactor::EVENT_TYPE_OUT;
        }

        if ( FD_ISSET(iterIOItem->second.pIO->GetHandle(), &stWorkingFDSetErr) )
        {
            iterIOItem->second.iActiveEvent = CMEReactor::EVENT_TYPE_ERR;
        }

        /* 更新活跃的IO对象计数 */
        if ( iterIOItem->second.iActiveEvent )
        {
            ++m_iActiveIOItemNum;
        }
    }
}

ME_NAME_SPACE_END

