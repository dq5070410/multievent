#include "LockMutex.h"
#include "OS.h"

ME_NAME_SPACE_BEGIN

CMELockMutex::CMELockMutex( LPSTR pszName /* = NULL */ )
{
	OS::MutexCreate(
		ME_MUTEX_CS::TYPE_PROCESS,
		m_Mutex,
		pszName );
}

CMELockMutex::~CMELockMutex()
{
	OS::MutexDestroy( m_Mutex );
}

ME_Result CMELockMutex::Lock()
{
	return OS::MutexLock( m_Mutex );
}

ME_Result CMELockMutex::Lock( const CTimeValue& htvTimeout )
{
	return OS::MutexLock( 
		m_Mutex, 
		htvTimeout );
}

ME_Result CMELockMutex::UnLock()
{
	return OS::MutexUnLock( m_Mutex );
}

ME_NAME_SPACE_END












