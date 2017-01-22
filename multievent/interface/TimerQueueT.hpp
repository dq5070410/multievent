#ifdef ME_LINUX
ME_USING_ME
#endif	// ME_LINUX

template <class TimerType, class TimerSinkType, class LockType>
CMETimerQueueT<TimerType, TimerSinkType, LockType>::CMETimerQueueT()
	: m_bExit( FALSE )
{

}

template <class TimerType, class TimerSinkType, class LockType>
CMETimerQueueT<TimerType, TimerSinkType, LockType>::~CMETimerQueueT()
{
	if ( FALSE == m_nodeList.IsEmpty() )
	{
		UINT nSize = m_nodeList.Size();
		for ( ; nSize > 0; --nSize )
		{
			NodeType* pNode = m_nodeList.EraseFirstItem();
			ME_DELETE( pNode );
		}
	}
}

template <class TimerType, class TimerSinkType, class LockType>
ME_Result CMETimerQueueT<TimerType, TimerSinkType, LockType>::Schedule( 
	TimerType* pTimer, 
	TimerSinkType* pSink, 
	const CTimeValue& htvInterval, 
	const CTimeValue& htvDelay, 
	int iLoopTime /* = 0 */ )
{
	ME_ASSERTE_RETURN( pTimer && pSink, ME_ERROR_NULL_POINTER );

	CMELockGuardT<LockType> Guard( &m_Lock );

	if ( TRUE == m_bExit )
	{
		return ME_ERROR_NOT_AVAILABLE;
	}

	NodeType* pNode = new NodeType;
	ME_Result hResult = SetupNode( 
		pNode,
		pTimer,
		pSink,
		htvInterval,
		htvDelay,
		iLoopTime );

	if ( ME_FAILED(hResult) )
	{
		ME_DELETE( pNode );

		ME_ERROR_TRACE_THIS( "failed to setup node" );

		return hResult;
	}

	/* 如果有重复schedule的，需要把这个人之前的shcedule首先cancel */
	Cancel( pTimer );

	hResult = m_nodeList.Add( pNode );
	/*SortFunctorType sft;
	m_nodeList.Sort( sft );*/

	m_nodeList.Sort();

	return hResult;
}

template <class TimerType, class TimerSinkType, class LockType>
ME_Result CMETimerQueueT<TimerType, TimerSinkType, LockType>::Cancel( TimerType* pTimer )
{
	CMELockGuardT<LockType> Guard( &m_Lock );

	if ( TRUE == m_bExit )
	{
		return ME_ERROR_NOT_AVAILABLE;
	}

	NodeType* pResult = m_nodeList.Find( pTimer );
	if ( NULL == pResult )
	{
		return ME_OK;
	}

	m_nodeList.Remove( pResult );
	ME_DELETE( pResult );

	return ME_OK;
}

template <class TimerType, class TimerSinkType, class LockType>
UINT CMETimerQueueT<TimerType, TimerSinkType, LockType>::GetNextTimeout()
{
	CMELockGuardT<LockType> Guard( &m_Lock );

	if ( (TRUE == m_nodeList.IsEmpty()) ||
		 (TRUE == m_bExit) )
	{
		return ME_INFINITE;
	}

	DWORD dwNodeRound = m_nodeList.GetFirstItem()->GetTimeTick().GetRound();
	DWORD dwNodeTickCount = m_nodeList.GetFirstItem()->GetTimeTick().GetTickCount();

	UpdateStandardTick();

	/* 如果第一个节点的轮数小于当前轮数 */
	if ( dwNodeRound < m_httStandard.GetRound() )
	{
		/* 说明这个node已经到期，需要立即执行 */
		return 0;
	}
	
	/* 当前轮次 */
	if ( dwNodeRound == m_httStandard.GetRound() )
	{
		return (dwNodeTickCount > m_httStandard.GetTickCount() ? dwNodeTickCount - m_httStandard.GetTickCount() : 0);
	}
	else	// 下一轮
	{
		return (dwNodeTickCount + (CTimeTick::MAX_TICK_COUNT - m_httStandard.GetTickCount()));
	}
}

template <class TimerType, class TimerSinkType, class LockType>
UINT CMETimerQueueT<TimerType, TimerSinkType, LockType>::Expire()
{
	CMELockGuardT<LockType> Guard( &m_Lock );

	if ( (TRUE == m_nodeList.IsEmpty()) ||
		 (TRUE == m_bExit) )
	{
		return ME_INFINITE;
	}

	NodeType* pNode = NULL;
	UINT nListSize = m_nodeList.Size();
	UINT iActiveNode = 0;
	for ( ; iActiveNode < nListSize; ++iActiveNode )
	{
		/** 
		* 放在这里是因为不知道上层执行OnTimer会用多长时间
		* 为了精确和保险起见，还是在循环里边每一次都update吧
		*/
		UpdateStandardTick();

		if ( m_nodeList.GetFirstItem()->GetTimeTick() > m_httStandard )
		{
			/** 
			* nodelist是排序的，如果第一个node的tick大于当前tick，
			* 说明list里还没有任何一个node timeout
			*/
			break;
		}

		pNode = m_nodeList.EraseFirstItem();
		if ( 0 == pNode->ReduceLoopTime() )
		{
			pNode->Active();
			ME_DELETE( pNode );
		}
		else
		{
			UpdateNode( pNode );
			m_nodeList.Add( pNode );
			m_nodeList.Sort();

			pNode->Active();
		}
	}

	return iActiveNode;
}

template <class TimerType, class TimerSinkType, class LockType>
void CMETimerQueueT<TimerType, TimerSinkType, LockType>::Destroy()
{
	CMELockGuardT<LockType> Guard( &m_Lock );

	m_bExit = TRUE;
}

template <class TimerType, class TimerSinkType, class LockType>
ME_Result CMETimerQueueT<TimerType, TimerSinkType, LockType>::SetupNode( 
	NodeType* pNode, 
	TimerType* pTimer, 
	TimerSinkType* pSink, 
	const CTimeValue& htvInterval, 
	const CTimeValue& htvDelay, 
	int iLoopTime /* = 0 */ )
{
	ME_ASSERTE_RETURN( pNode, ME_ERROR_NULL_POINTER );

	ME_ASSERTE_RETURN(
		(CTimeTick::MAX_TICK_COUNT > (DWORD)htvInterval.GetMilliSecond()) &&
		(CTimeTick::MAX_TICK_COUNT > (DWORD)htvDelay.GetMilliSecond()),
		ME_ERROR_INVALID_ARG );

	UpdateStandardTick();

	CTimeTick httDelayFixed;
	DWORD dwDelayTickCount = (DWORD)htvDelay.GetMilliSecond();

	/**
	* 把<dwDelayTickCount>加上系统当前tick count就得到这个节点在什么tick count
	* 时active
	*/
	AdjustTick( 
		httDelayFixed,
		dwDelayTickCount );

	CTimeTick httIntervalFixed;
	DWORD dwIntervalTickCount = (DWORD)htvInterval.GetMilliSecond();
	httIntervalFixed.Set(
		0,	// 频率没有轮数这一说
		MINIMUM_TIMER_INTERVAL > dwIntervalTickCount ? 
#if ME_TIME_LIMIT
		MINIMUM_TIMER_INTERVAL : dwIntervalTickCount	// 小于最小频率值的，通通强制改为最小频率值
#else
		dwIntervalTickCount : dwIntervalTickCount		// 不限制最小频率值，也就是，最短频率可以设到1ms
#endif	// ME_TIME_LIMIT
			);

	return pNode->Set(
		pTimer,
		pSink,
		httIntervalFixed,
		httDelayFixed,
		iLoopTime) ;
}

template <class TimerType, class TimerSinkType, class LockType>
void CMETimerQueueT<TimerType, TimerSinkType, LockType>::UpdateStandardTick()
{
	CMELockGuardT<LockType> Guard( &m_LockStandard );

	/**
	* 系统当前的tickcount正常情况下都会比<m_httStandard>的大 -- 因为总是先schedule后执行
	* 所以，一旦系统的tickcount比<m_httStandard>的小，那么肯定是发生了49.7天的这种情况，
	* 这就必须增加<m_httStandard>的轮数了
	*/
	m_httStandard.Update(
		CTimeTick::GetSysTickCount(),
		CTimeTick::GetSysTickCount() < m_httStandard.GetTickCount() ? TRUE : FALSE );

	/* 这个写法可能会有一些微小的误差(最多也就是纳秒级的)，不过不会有什么不良的影响 */
}

template <class TimerType, class TimerSinkType, class LockType>
void CMETimerQueueT<TimerType, TimerSinkType, LockType>::UpdateNode( NodeType* pNode )
{
	UpdateStandardTick();

	CTimeTick httUpdate;

	AdjustTick(
		httUpdate,
		pNode->GetIntervalTimeCount() );

	pNode->SetTimeTick( httUpdate );
}

template <class TimerType, class TimerSinkType, class LockType>
void CMETimerQueueT<TimerType, TimerSinkType, LockType>::AdjustTick( 
	CTimeTick& httTick, 
	DWORD dwTickStep )
{
	if ( (CTimeTick::MAX_TICK_COUNT -  m_httStandard.GetTickCount()) > dwTickStep )
	{
		/* 在当前的49.7天内执行，也就是在这一轮执行 */
		httTick.Set(
			m_httStandard.GetRound(),
			dwTickStep + m_httStandard.GetTickCount() );
	}
	else
	{
		httTick.Set(
			m_httStandard.GetRound() + 1,
			dwTickStep - (CTimeTick::MAX_TICK_COUNT -  m_httStandard.GetTickCount()) );
	}
}

template <class TimerType, class TimerSinkType, class LockType>
void CMETimerQueueT<TimerType, TimerSinkType, LockType>::operator = ( const CMETimerQueueT<TimerType, TimerSinkType, LockType>& Dst )
{
	m_bExit = Dst.m_bExit;
	m_httStandard = Dst.m_httStandard;
}

template <class TimerType, class TimerSinkType, class LockType>
template <class TimerNodeType>
BOOL CMETimerQueueT<TimerType, TimerSinkType, LockType>::CTimerNodeLesser<TimerNodeType>::operator () (
	const TimerNodeType& pSrc, 
	const TimerNodeType& pDst )
{
	/* 依赖于TimerNodeType类型的<运算符 */
	return *pSrc < *pDst;
}













