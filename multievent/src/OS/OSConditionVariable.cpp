#include "OSConditionVariable.h"
#include "TimeValue.h"

ME_NAME_SPACE_OS_BEGIN

ME_Result ConditionVariableCreate( 
	ME_CONDITION_VARIABLE* pConditionVariable,
	const char* pszName )
{
#ifdef ME_WIN
	return ERROR_NOT_IMPLEMENTED;

#elif defined( ME_LINUX )
	ME_Result hResult = ::pthread_cond_init(
		pConditionVariable,
		NULL );

	return ME_SUCCEEDED(hResult) ? ME_OK : ME_ERROR_FAILURE;

#endif	// ME_WIN
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result ConditionVariableDestroy( ME_CONDITION_VARIABLE* pConditionVariable )
{
#ifdef ME_WIN
	return ME_ERROR_NOT_IMPLEMENTED;

#elif defined( ME_LINUX )
	ME_Result hResult = ::pthread_cond_destroy( pConditionVariable );

	return ME_SUCCEEDED(hResult) ? ME_OK : ME_ERROR_FAILURE;

#endif	// ME_WIN
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result ConditionVariableSignal( ME_CONDITION_VARIABLE* pConditionVariable )
{
#ifdef ME_WIN
	return ME_ERROR_NOT_IMPLEMENTED;

#elif defined( ME_LINUX )
	ME_Result hResult = ::pthread_cond_signal( pConditionVariable );

	return ME_SUCCEEDED(hResult) ? ME_OK : ME_ERROR_FAILURE;

#endif	// ME_WIN
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result ConditionVariableBroadcast( ME_CONDITION_VARIABLE* pConditionVariable )
{
#ifdef ME_WIN
	return ME_ERROR_NOT_IMPLEMENTED;

#elif defined( ME_LINUX )
	ME_Result hResult = ::pthread_cond_broadcast( pConditionVariable );

	return ME_SUCCEEDED(hResult) ? ME_OK : ME_ERROR_FAILURE;

#endif	// ME_WIN
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result ConditionVariableWait( 
	ME_CONDITION_VARIABLE* pConditionVariable, 
	ME_THREAD_MUTEX* pMutex,
	CTimeValue* pTimeout /*= NULL */ )
{
#ifdef ME_WIN
	return ME_ERROR_NOT_IMPLEMENTED;

#elif defined( ME_LINUX )
	ME_Result hResult = ME_ERROR_FAILURE;
	if ( NULL== pTimeout )
	{
		hResult = ::pthread_cond_wait(
			pConditionVariable,
			pMutex );
	}
	else
	{
		/** 
		* timespec是从1970.1.1 GMT 00:00:00算起的, 所以需要用CMETimeValue
		* 首先取得当前的年月日小时分秒, 再换算过去
		*/

		struct timespec tsTimeout;
		CTimeValue tvAbs = CTimeValue::TimeOfDay() + *pTimeout;
		tsTimeout.tv_sec = tvAbs.GetSecond();				// 秒
		tsTimeout.tv_nsec = tvAbs.GetMicrosecond() * 1000;	// 微秒换算成纳秒

		hResult = ::pthread_cond_timedwait(
			pConditionVariable,
			pMutex,
			&tsTimeout );
	}

	return ME_SUCCEEDED(hResult) ? ME_OK : ME_ERROR_FAILURE;

#endif	// ME_WIN
}

ME_NAME_SPACE_OS_END



