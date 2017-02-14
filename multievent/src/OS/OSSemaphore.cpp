#include "OSSemaphore.h"

ME_NAME_SPACE_OS_BEGIN

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result SemaphoreCreate( 
	ME_SEMAPHONE& pSemaphore,
	DWORD dwInitCount,
	DWORD dwMaxCount /* = ME_SEMAPHORE_MAX_COUNT */,
	const char* pszName /* = NULL */ )
{
#ifdef ME_WIN
	pSemaphore = ::CreateSemaphoreA(
		NULL,
		dwInitCount,
		dwMaxCount,
		pszName );

	return (pSemaphore && ME_INVALID_HANDLE != pSemaphore) ? ME_OK : ME_ERROR_FAILURE;

#elif defined( ME_LINUX )
	/** 
	* 命名信号量需要用sem_open, 匿名的用sem_init
	* 同时对应的关闭操作也不一样, 分别对应sem_close和sem_destroy
	*/
	if ( NULL == pszName )
	{
		ME_Result hResult = ::sem_init( 
			pSemaphore, 
			FALSE, 
			dwInitCount );

		if ( ME_FAILED(hResult) ) 
		{
			return ME_ERROR_FAILURE;
		}
	}
	else
	{
		pSemaphore = ::sem_open(
			pszName,
			O_CREAT,
			ME_SEMAPHORE_FILE_MODE,
			dwInitCount );

		if ( SEM_FAILED == pSemaphore )
		{
			return ME_ERROR_FAILURE;
		}
	}

	return ME_OK;
#endif	// ME_WIN
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result SemaphoreDestroy( 
	ME_SEMAPHONE& pSemaphore,
	const char* pszName /* = NULL */ )
{
#ifdef ME_WIN
	::CloseHandle( pSemaphore );

#elif defined( ME_LINUX )
	ME_Result hResult = ME_ERROR_FAILURE;
	if ( NULL == pszName )
	{
		hResult = ::sem_destroy( pSemaphore );

		if ( ME_FAILED(hResult) ) 
		{
			return ME_ERROR_FAILURE;
		}
	}
	else
	{
		hResult = ::sem_unlink( pszName );
		if ( ME_FAILED(hResult) )
		{
			return ME_ERROR_FAILURE;
		}

		hResult = ::sem_close( pSemaphore );
		if ( ME_FAILED(hResult) )
		{
			return ME_ERROR_FAILURE;
		}
	}

#endif	// ME_WIN

	return ME_OK;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result SemaphoreLock( ME_SEMAPHONE pSemaphore )
{
	ME_Result hResult = ME_ERROR_FAILURE;

#ifdef ME_WIN
	hResult = ::WaitForSingleObject(
		pSemaphore,
		INFINITE );

#elif defined( ME_LINUX )
	hResult = ::sem_wait( pSemaphore );

#endif	// ME_WIN

	return ME_SUCCEEDED(hResult) ? ME_OK : ME_ERROR_FAILURE;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result SemaphoreUnLock( ME_SEMAPHONE pSemaphore )
{
	return SemaphorePostN(
		pSemaphore,
		1 );
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result SemaphorePostN( 
	ME_SEMAPHONE pSemaphore,
	DWORD dwCount )
{
	ME_Result hResult = ME_ERROR_FAILURE;

#ifdef ME_WIN
	hResult = ::ReleaseSemaphore(
		pSemaphore,
		dwCount,
		NULL );

	if ( !ME_FAILED(hResult) )
	{
		return ME_ERROR_FAILURE;
	}
	else
	{
		return ME_OK;
	}

#elif defined( ME_LINUX )
	for ( int i = 0; i < dwCount; ++i )
	{
		hResult = ::sem_post( pSemaphore );
		if ( ME_FAILED(hResult) )
		{
			return ME_ERROR_FAILURE;
		}
	}

	return hResult;

#endif	// ME_WIN
}

ME_NAME_SPACE_OS_END


