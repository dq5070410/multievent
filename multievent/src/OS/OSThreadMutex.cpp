#include "OSThreadMutex.h"

NAME_SPACE_OS_BEGIN

ME_Result ThreadMutexCreate( ME_THREAD_MUTEX& htmLock )
{
#ifdef ME_WIN
	::InitializeCriticalSection( &htmLock );

#elif defined( ME_LINUX )
	pthread_mutexattr_t pmaMutexAttr;

	INT iResult = ::pthread_mutexattr_init( &pmaMutexAttr );
	if ( 0 != iResult )
	{
		::pthread_mutexattr_destroy( &pmaMutexAttr );
		ME_ERROR_TRACE( "thread mutex attr init error = " << iResult );

		return ME_ERROR_FAILURE;
	}

	/* 设置mutex类型为回旋锁 */
	iResult = ::pthread_mutexattr_settype(
		&pmaMutexAttr, 
		PTHREAD_MUTEX_RECURSIVE );

	ME_ASSERTE( 0 == iResult );

	iResult = ::pthread_mutex_init( 
		&htmLock, 
		&pmaMutexAttr );

	if ( 0 != iResult )
	{
		ThreadMutexDestroy( htmLock );

		ME_ERROR_TRACE( "thread mutex init error = " << iResult );

		return ME_ERROR_FAILURE;
	}

#endif	// ME_WIN

	return ME_OK;
}

ME_INLINE ME_Result ThreadMutexDestroy( ME_THREAD_MUTEX& htmLock )
{
#ifdef ME_WIN
	::DeleteCriticalSection( &htmLock );

#elif defined( ME_LINUX )
	int iResult = ::pthread_mutex_destroy( &htmLock );
	if ( 0 != iResult )
	{
		ME_ERROR_TRACE( 
			"failed to destroy mutex = " << &htmLock
			<< ", error = " << iResult );

		return ME_ERROR_FAILURE;
	}

#endif	// ME_WIN

	return ME_OK;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result ThreadMutexLock( ME_THREAD_MUTEX& htmLock )
{
#ifdef ME_WIN
	::EnterCriticalSection( &htmLock );

#elif defined( ME_LINUX )
	int iResult = ::pthread_mutex_lock( &htmLock );
	if ( 0 != iResult )
	{
		ME_ERROR_TRACE( 
			"failed to lock mutex = " << &htmLock
			<< ", error = " << iResult );

		return ME_ERROR_FAILURE;
	}

#endif	// ME_WIN

	return ME_OK;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result ThreadMutexUnLock( ME_THREAD_MUTEX& htmLock )
{
#ifdef ME_WIN
	::LeaveCriticalSection( &htmLock );

#elif defined( ME_LINUX )
	int iResult = ::pthread_mutex_unlock( &htmLock );
	if ( 0 != iResult )
	{
		ME_ERROR_TRACE( 
			"failed to unlock mutex = " << &htmLock
			<< ", error = " << iResult );

		return ME_ERROR_FAILURE;
	}

#endif	// ME_WIN

	return ME_OK;
}

NAME_SPACE_OS_END


