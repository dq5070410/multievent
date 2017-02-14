#include "OSEvent.h"

ME_NAME_SPACE_OS_BEGIN

ME_INLINE ME_Result EventCreate(
	ME_EVENT& pEvent,
	const char* pszName /* = NULL */,
	BOOL bManualReset /* = FALSE */,
	BOOL bSignaled /* = FALSE */ )
{
#ifdef ME_WIN
	pEvent = ::CreateEventA(
		NULL,
		bManualReset,
		bSignaled,
		pszName );

	return (pEvent && ME_INVALID_HANDLE != pEvent) ? ME_OK : ME_ERROR_FAILURE;

#else
	return ME_ERROR_NOT_AVAILABLE;

#endif	// ME_WIN
}

ME_INLINE ME_Result EventDestroy( ME_EVENT& pEvent )
{
#ifdef ME_WIN
	::CloseHandle( pEvent );

	return ME_OK;

#else
	return ME_ERROR_NOT_AVAILABLE;

#endif	// ME_WIN
}

ME_INLINE ME_Result EventWait( ME_EVENT pEvent )
{
#ifdef ME_WIN
	ME_Result hResult = ::WaitForSingleObject(
		pEvent,
		INFINITE );

	if ( ME_SUCCEEDED(hResult) )
	{
		return ME_OK;
	}
	else
	{
		return ME_ERROR_FAILURE;
	}

#else
	return ME_ERROR_NOT_AVAILABLE;

#endif	// ME_WIN
}

ME_INLINE ME_Result EventTimeWait( 
	ME_EVENT pEvent,
	const CTimeValue& htvTime )
{
#ifdef ME_WIN
	ME_Result hResult = ::WaitForSingleObject(
		pEvent,
		htvTime.GetMilliSecond() );

	if ( ME_SUCCEEDED(hResult) || (WAIT_TIMEOUT == hResult) )
	{
		return ME_OK == hResult ? ME_OK : ME_ERROR_TIMEOUT;
	}
	else
	{
		return ME_ERROR_FAILURE;
	}

#else
	return ME_ERROR_NOT_AVAILABLE;

#endif	// ME_WIN
}

ME_INLINE ME_Result EventSignal( ME_EVENT pEvent )
{
#ifdef ME_WIN
	ME_Result hResult = !::SetEvent( pEvent );
	if ( ME_SUCCEEDED(hResult) )
	{
		return ME_OK;
	}
	else
	{
		return ME_ERROR_FAILURE;
	}

#else
	return ME_ERROR_NOT_AVAILABLE;

#endif	// ME_WIN
}

ME_INLINE ME_Result EventReset( ME_EVENT pEvent )
{
#ifdef ME_WIN
	ME_Result hResult = !::ResetEvent( pEvent );
	if ( ME_SUCCEEDED(hResult) )
	{
		return ME_OK;
	}
	else
	{
		return ME_ERROR_FAILURE;
	}

#else
	return ME_ERROR_NOT_AVAILABLE;

#endif	// ME_WIN
}

ME_NAME_SPACE_OS_END










