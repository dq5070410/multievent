#include "LockSemaphore.h"

ME_NAME_SPACE_BEGIN

CMELockSemaphore::CMELockSemaphore( 
	DWORD dwInitCount, 
	DWORD dwMaxCount /* = OS::ME_SEMAPHORE_MAX_COUNT */, 
	const char* pszName /* = NULL */ )
{
	ME_Result hResult = OS::SemaphoreCreate(
		m_Semaphore,
		dwInitCount,
		dwMaxCount,
		pszName );

	ME_ASSERTE( ME_SUCCEEDED(hResult) );

	::memset(
		m_szName,
		0,
		ME_SEMAPHONE_NAME_MAX_LENGTH );

	if ( pszName )
	{
		::strncpy(
			m_szName,
			pszName,
			ME_SEMAPHONE_NAME_MAX_LENGTH );
	}
}

CMELockSemaphore::~CMELockSemaphore()
{
	OS::SemaphoreDestroy( 
		m_Semaphore, 
		m_szName );
}

ME_Result CMELockSemaphore::Lock()
{
	return OS::SemaphoreLock( m_Semaphore );
}

ME_Result CMELockSemaphore::UnLock()
{
	return OS::SemaphoreUnLock( m_Semaphore );
}

ME_Result CMELockSemaphore::PostN( DWORD dwCount )
{
	return OS::SemaphorePostN(
		m_Semaphore,
		dwCount );
}

ME_NAME_SPACE_END



