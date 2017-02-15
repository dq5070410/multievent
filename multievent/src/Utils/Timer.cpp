#include "Timer.h"
#include "TimeValue.h"
#include "Thread.h"
#include "ThreadManager.h"

ME_NAME_SPACE_BEGIN


CMETimer::CMETimer()
{

}

CMETimer::~CMETimer()
{

}

ME_Result CMETimer::Schedule( 
	ITimerSink* pSink, 
	const CTimeValue& htvInterval, 
	const CTimeValue& htvDelay,
	int iLoopTime )
{
	CMEThread* pThread = (CMEThread*)CMEThreadManager::Instance()->GetCurrentThread();
	if ( NULL == pThread )
	{
		return ME_ERROR_NOT_AVAILABLE;
	}

	return pThread->ScheduleTimer(
		this,
		pSink,
		htvInterval,
		htvDelay,
		iLoopTime );
}

ME_Result CMETimer::Schedule( 
	ITimerSink* pSink, 
	const CTimeValue& htvInterval )
{
	return Schedule(
		pSink,
		htvInterval,
		CTimeValue(),
		1 );
}

ME_Result CMETimer::Schedule( 
	ITimerSink* pSink, 
	const CTimeValue& htvInterval, 
	int iLoopTime )
{
	return Schedule(
		pSink,
		htvInterval,
		CTimeValue(),
		iLoopTime );
}

void CMETimer::Cancel()
{
	CMEThread* pThread = (CMEThread*)CMEThreadManager::Instance()->GetCurrentThread();
	if ( NULL == pThread )
	{
		return;
	}

	pThread->CancelTimer( this );
}


ME_NAME_SPACE_END












