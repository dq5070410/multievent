#include "LockThreadMutex.h"
#include "OS.h"

ME_NAME_SPACE_BEGIN

CMELockThreadMutex::CMELockThreadMutex()
{
	OS::ThreadMutexCreate( m_ThreadMutex.htmLock );
}

CMELockThreadMutex::~CMELockThreadMutex()
{
	OS::ThreadMutexDestroy( m_ThreadMutex.htmLock );
}

ME_Result CMELockThreadMutex::Lock()
{
	return OS::ThreadMutexLock( m_ThreadMutex.htmLock );
}

ME_Result CMELockThreadMutex::UnLock()
{
	return OS::ThreadMutexUnLock( m_ThreadMutex.htmLock );
}

ME_NAME_SPACE_END












