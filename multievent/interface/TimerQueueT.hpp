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

	/* ������ظ�schedule�ģ���Ҫ�������֮ǰ��shcedule����cancel */
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

	/* �����һ���ڵ������С�ڵ�ǰ���� */
	if ( dwNodeRound < m_httStandard.GetRound() )
	{
		/* ˵�����node�Ѿ����ڣ���Ҫ����ִ�� */
		return 0;
	}
	
	/* ��ǰ�ִ� */
	if ( dwNodeRound == m_httStandard.GetRound() )
	{
		return (dwNodeTickCount > m_httStandard.GetTickCount() ? dwNodeTickCount - m_httStandard.GetTickCount() : 0);
	}
	else	// ��һ��
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
		* ������������Ϊ��֪���ϲ�ִ��OnTimer���ö೤ʱ��
		* Ϊ�˾�ȷ�ͱ��������������ѭ�����ÿһ�ζ�update��
		*/
		UpdateStandardTick();

		if ( m_nodeList.GetFirstItem()->GetTimeTick() > m_httStandard )
		{
			/** 
			* nodelist������ģ������һ��node��tick���ڵ�ǰtick��
			* ˵��list�ﻹû���κ�һ��node timeout
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
	* ��<dwDelayTickCount>����ϵͳ��ǰtick count�͵õ�����ڵ���ʲôtick count
	* ʱactive
	*/
	AdjustTick( 
		httDelayFixed,
		dwDelayTickCount );

	CTimeTick httIntervalFixed;
	DWORD dwIntervalTickCount = (DWORD)htvInterval.GetMilliSecond();
	httIntervalFixed.Set(
		0,	// Ƶ��û��������һ˵
		MINIMUM_TIMER_INTERVAL > dwIntervalTickCount ? 
#if ME_TIME_LIMIT
		MINIMUM_TIMER_INTERVAL : dwIntervalTickCount	// С����СƵ��ֵ�ģ�ͨͨǿ�Ƹ�Ϊ��СƵ��ֵ
#else
		dwIntervalTickCount : dwIntervalTickCount		// ��������СƵ��ֵ��Ҳ���ǣ����Ƶ�ʿ����赽1ms
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
	* ϵͳ��ǰ��tickcount��������¶����<m_httStandard>�Ĵ� -- ��Ϊ������schedule��ִ��
	* ���ԣ�һ��ϵͳ��tickcount��<m_httStandard>��С����ô�϶��Ƿ�����49.7������������
	* ��ͱ�������<m_httStandard>��������
	*/
	m_httStandard.Update(
		CTimeTick::GetSysTickCount(),
		CTimeTick::GetSysTickCount() < m_httStandard.GetTickCount() ? TRUE : FALSE );

	/* ���д�����ܻ���һЩ΢С�����(���Ҳ�������뼶��)������������ʲô������Ӱ�� */
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
		/* �ڵ�ǰ��49.7����ִ�У�Ҳ��������һ��ִ�� */
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
	/* ������TimerNodeType���͵�<����� */
	return *pSrc < *pDst;
}













