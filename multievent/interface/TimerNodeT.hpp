
template <class TimerType, class TimerSinkType>
CTimerNodeT<TimerType, TimerSinkType>::CTimerNodeT()
	: m_pTimer( NULL )
	, m_pTimerSink( NULL )
	, m_iLoopTime( 0 )
{

}

template <class TimerType, class TimerSinkType>
CTimerNodeT<TimerType, TimerSinkType>::CTimerNodeT( SuperType* pSuper )
	: m_pTimer( pSuper )
	, m_pTimerSink( NULL )
	, m_iLoopTime( 0 )
{

}

template <class TimerType, class TimerSinkType>
CTimerNodeT<TimerType, TimerSinkType>::CTimerNodeT( 
	TimerType* pTimer, 
	TimerSinkType* pSink, 
	const CHikTimeValue& htvInterval, 
	const CHikTimeValue& htvDelay, 
	int iLoopTime /* = 0 */ )
	: m_pTimer( pTimer )
	, m_pTimerSink( pSink )
	, m_iLoopTime( iLoopTime )
{
	m_httInterval = htvInterval;
	m_httDelay = htvDelay;
}

template <class TimerType, class TimerSinkType>
CTimerNodeT<TimerType, TimerSinkType>::~CTimerNodeT()
{

}

template <class TimerType, class TimerSinkType>
HikResult CTimerNodeT<TimerType, TimerSinkType>::Set( 
	TimerType* pTimer, 
	TimerSinkType* pSink, 
	const CHikTimeTick& httInterval, 
	const CHikTimeTick& httDelay, 
	int iLoopTime /* = 0 */ )
{
	HIK_ASSERTE_RETURN( pTimer && pSink, HIK_ERROR_INVALID_ARG );

	m_pTimer = pTimer;
	m_pTimerSink = pSink;

	m_httInterval = httInterval;
	m_httDelay = httDelay;
	m_iLoopTime = iLoopTime;

	return HIK_OK;
}

template <class TimerType, class TimerSinkType>
void CTimerNodeT<TimerType, TimerSinkType>::SetTimeTick( const CHikTimeTick& httTimeTick )
{
	m_httDelay = httTimeTick;
}

template <class TimerType, class TimerSinkType>
void CTimerNodeT<TimerType, TimerSinkType>::SetTimeTick( 
	DWORD dwRound, 
	DWORD dwTickCount )
{
	m_httDelay.Set(
		dwRound,
		dwTickCount );
}

template <class TimerType, class TimerSinkType>
int CTimerNodeT<TimerType, TimerSinkType>::ReduceLoopTime()
{
	if ( (0 == m_iLoopTime) || (-1 == m_iLoopTime) )
	{
		return m_iLoopTime;
	}

	return (--m_iLoopTime);
}

template <class TimerType, class TimerSinkType>
const CHikTimeTick& CTimerNodeT<TimerType, TimerSinkType>::GetTimeTick() const
{
	return m_httDelay;
}

template <class TimerType, class TimerSinkType>
DWORD CTimerNodeT<TimerType, TimerSinkType>::GetTimeCount() const
{
	return m_httDelay.GetTickCount();
}

template <class TimerType, class TimerSinkType>
DWORD CTimerNodeT<TimerType, TimerSinkType>::GetIntervalTimeCount() const
{
	return m_httInterval.GetTickCount();
}

template <class TimerType, class TimerSinkType>
HikResult CTimerNodeT<TimerType, TimerSinkType>::Active()
{
	HIK_ASSERTE_RETURN( m_pTimerSink, HIK_ERROR_NULL_POINTER );

	m_pTimerSink->OnTimer( m_pTimer );

	return HIK_OK;
}

template <class TimerType, class TimerSinkType>
BOOL CTimerNodeT<TimerType, TimerSinkType>::operator == ( const NodeType& ntNode )
{
	return (m_pTimer == ntNode.m_pTimer);
}

template <class TimerType, class TimerSinkType>
BOOL CTimerNodeT<TimerType, TimerSinkType>::operator == ( const SuperType& stSuper )
{
	return (m_pTimer == &stSuper);
}

template <class TimerType, class TimerSinkType>
BOOL CTimerNodeT<TimerType, TimerSinkType>::operator > ( const NodeType& ntNode )
{
	return (m_httDelay > ntNode.m_httDelay);
}

template <class TimerType, class TimerSinkType>
BOOL CTimerNodeT<TimerType, TimerSinkType>::operator < ( const NodeType& ntNode )
{
	return (m_httDelay < ntNode.m_httDelay);
}

