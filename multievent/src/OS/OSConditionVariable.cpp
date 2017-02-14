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

/* 2013.4.12 ��gcc�ܹ���ȷʶ�� */
/*ME_INLINE */ME_Result ConditionVariableDestroy( ME_CONDITION_VARIABLE* pConditionVariable )
{
#ifdef ME_WIN
	return ME_ERROR_NOT_IMPLEMENTED;

#elif defined( ME_LINUX )
	ME_Result hResult = ::pthread_cond_destroy( pConditionVariable );

	return ME_SUCCEEDED(hResult) ? ME_OK : ME_ERROR_FAILURE;

#endif	// ME_WIN
}

/* 2013.4.12 ��gcc�ܹ���ȷʶ�� */
/*ME_INLINE */ME_Result ConditionVariableSignal( ME_CONDITION_VARIABLE* pConditionVariable )
{
#ifdef ME_WIN
	return ME_ERROR_NOT_IMPLEMENTED;

#elif defined( ME_LINUX )
	ME_Result hResult = ::pthread_cond_signal( pConditionVariable );

	return ME_SUCCEEDED(hResult) ? ME_OK : ME_ERROR_FAILURE;

#endif	// ME_WIN
}

/* 2013.4.12 ��gcc�ܹ���ȷʶ�� */
/*ME_INLINE */ME_Result ConditionVariableBroadcast( ME_CONDITION_VARIABLE* pConditionVariable )
{
#ifdef ME_WIN
	return ME_ERROR_NOT_IMPLEMENTED;

#elif defined( ME_LINUX )
	ME_Result hResult = ::pthread_cond_broadcast( pConditionVariable );

	return ME_SUCCEEDED(hResult) ? ME_OK : ME_ERROR_FAILURE;

#endif	// ME_WIN
}

/* 2013.4.12 ��gcc�ܹ���ȷʶ�� */
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
		* timespec�Ǵ�1970.1.1 GMT 00:00:00�����, ������Ҫ��CMETimeValue
		* ����ȡ�õ�ǰ��������Сʱ����, �ٻ����ȥ
		*/

		struct timespec tsTimeout;
		CTimeValue tvAbs = CTimeValue::TimeOfDay() + *pTimeout;
		tsTimeout.tv_sec = tvAbs.GetSecond();				// ��
		tsTimeout.tv_nsec = tvAbs.GetMicrosecond() * 1000;	// ΢�뻻�������

		hResult = ::pthread_cond_timedwait(
			pConditionVariable,
			pMutex,
			&tsTimeout );
	}

	return ME_SUCCEEDED(hResult) ? ME_OK : ME_ERROR_FAILURE;

#endif	// ME_WIN
}

ME_NAME_SPACE_OS_END



