#include "OSThread.h"
#include "TimeValue.h"

ME_NAME_SPACE_OS_BEGIN

ME_Result ThreadCreate( 
	ME_THREAD_FUNC pFunc, 
	LPVOID pArgs, 
	UINT nFlags, 
	ME_THREAD_ID& htiThreadID, 
	ME_THREAD_HANDLE* hThreadHandle /* = 0 */, 
	LONG lPriority /* = 0 */, 
	LPVOID pStack /* = NULL */, 
	DWORD dwStacksize /* = 0 */ )
{
#ifdef ME_WIN
	htiThreadID = 0;
	*hThreadHandle = (ME_THREAD_HANDLE)::_beginthreadex(
		NULL,
		dwStacksize,
		pFunc,
		pArgs,
		nFlags,
		&htiThreadID );

	if ( 0 == htiThreadID )
	{
		ME_ERROR_TRACE( "failed to create thread, error = " << ::GetLastError() );

		return ME_ERROR_FAILURE;
	}

	return ME_OK;

#elif defined( ME_LINUX )
	pthread_attr_t Attr;
	ME_Result hResult = ::pthread_attr_init( &Attr );
	if ( ME_FAILED(hResult) ) 
	{
		ME_ERROR_TRACE( "failed to create thread attr, error = " << hResult );

		return ME_ERROR_FAILURE;
	}

	do 
	{
		hResult = ::pthread_attr_setdetachstate(
			&Attr,
			PTHREAD_CREATE_JOINABLE );

		if ( ME_FAILED(hResult) ) 
		{
			ME_ERROR_TRACE("failed to set thread state, error = " << hResult );

			hResult = ME_ERROR_FAILURE;

			break;
		}

		hResult = ::pthread_create(
			&htiThreadID,
			&Attr,
			pFunc,
			pArgs );

		if ( ME_FAILED(hResult) ) 
		{
			ME_ERROR_TRACE("failed to set thread state, error = " << hResult );

			hResult = ME_ERROR_FAILURE;

			break;
		}

		*hThreadHandle = htiThreadID;
	} while ( 0 );
	
	::pthread_attr_destroy( &Attr );

	return hResult;

#endif	// ME_WIN
}

/* 2013.4.12 ��gcc�ܹ���ȷʶ�� */
/*ME_INLINE */ME_Result ThreadDestroy( 
	ME_THREAD_HANDLE hThreadHandle, 
	DWORD dwExitCode )
{
#ifdef ME_WIN
	BOOL bResult = ::TerminateThread( 
		hThreadHandle,
		dwExitCode );

	if ( FALSE == bResult )
	{
		return ME_ERROR_FAILURE;
	}

	::CloseHandle( hThreadHandle );

	return ME_OK;

#elif defined( ME_LINUX )

	ME_Result hResult = ::pthread_cancel( hThreadHandle );	// 
	if ( ME_FAILED(hResult) )
	{
		ME_ERROR_TRACE( "thread ( " << hThreadHandle <<
			" ) cancel failed" << 
			", error = " << hResult );

		return ME_ERROR_FAILURE;
	}

	return ThreadJoin( hThreadHandle );

#endif	// ME_WIN
}

ME_INLINE ME_Result ThreadTestDestroy()
{
#ifdef ME_WIN

#elif defined( ME_LINUX )
	::pthread_testcancel();	// ����һ��cancellation point

#endif	// ME_WIN

	return ME_OK;
}

ME_Result ThreadJoin( 
	ME_THREAD_HANDLE hThreadHandle,
	CTimeValue* pTimeout, /* = NULL, */
	ME_THREAD_STAT* pStat /* = NULL */ )
{
	ME_Result hResult = ME_ERROR_FAILURE;
#ifdef ME_WIN
	if ( NULL == pTimeout )
	{
		 hResult = ::WaitForSingleObject(
			 hThreadHandle,
			 INFINITE );
	}
	else
	{
		hResult = ::WaitForSingleObject(
			hThreadHandle,
			(DWORD)pTimeout->GetMilliSecond() );
	}

	if ( ME_SUCCEEDED(hResult) )
	{
		if ( pStat )
		{
			::GetExitCodeThread( 
				hThreadHandle,
				pStat );
		}
	}
	else
	{
		hResult = ERROR_INVALID_HANDLE == ::GetLastError() ? ME_OK : ME_ERROR_FAILURE;
	}

#elif defined( ME_LINUX )
	if( NULL == pTimeout )
	{
		hResult = ::pthread_join(
			hThreadHandle,
			pStat );
	}
	else
	{
		/** 
		* timespec�Ǵ�1970.1.1 GMT 00:00:00�����, ������Ҫ��CTimeValue
		* ����ȡ�õ�ǰ��������Сʱ����, �ٻ����ȥ
		*/

		struct timespec tsTimeout;
		CTimeValue tvAbs = CTimeValue::TimeOfDay() + *pTimeout;
		tsTimeout.tv_sec = tvAbs.GetSecond();				// ��
		tsTimeout.tv_nsec = tvAbs.GetMicrosecond() * 1000;	// ΢�뻻�������

		hResult = ::pthread_timedjoin_np(
			hThreadHandle,
			pStat,
			&tsTimeout );
	}

	/**
	* �������ֵ��EINVAL��˵���߳��Ѿ��˳�����ô����join
	* ҲӦ����ɹ�����Ϊjoin��Ŀ�������߳��˳�����Ȼ�߳�
	* �Ѿ��˳�(�����ǲ�����Ϊ���join���˳���)����ô��Ӧ��
	* ��ɹ�
	*/
	hResult = ((EINVAL == hResult) || (ME_OK == hResult)) ? ME_OK : ME_ERROR_FAILURE;

#endif	// ME_WIN

	return hResult;
}

ME_Result ThreadSetPriority(
	ME_THREAD_HANDLE hThreadHandle,
	int iPriority )
{
	ME_Result hResult = ME_ERROR_NOT_AVAILABLE;
#ifdef ME_WIN
	return (TRUE == ::SetThreadPriority( hThreadHandle, iPriority ) ? ME_OK : ME_ERROR_FAILURE);

#endif	// ME_WIN

	return hResult;
}

/* 2013.4.12 ��gcc�ܹ���ȷʶ�� */
/*ME_INLINE */ME_THREAD_ID GetThreadID()
{
#ifdef ME_WIN
	return ::GetCurrentThreadId();

#elif defined( ME_LINUX )
	return ::pthread_self();

#endif	// ME_WIN
}

ME_INLINE ME_THREAD_HANDLE GetThreadHandle()
{
#ifdef ME_WIN
	return ::GetCurrentThread();

#elif defined( ME_LINUX )
	return ::pthread_self();

#endif	// ME_WIN
}

ME_NAME_SPACE_OS_END

