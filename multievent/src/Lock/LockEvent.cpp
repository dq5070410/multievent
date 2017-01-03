#include "LockEvent.h"
#include "OS.h"
#include "TimeValue.h"

ME_NAME_SPACE_BEGIN

CMELockEvent::CMELockEvent( 
	BOOL bManualReset /* = FALSE */, 
	BOOL bSignaled /* = FALSE */, 
	const char* pszName /* = NULL */ )
#ifndef ME_WIN
	: m_pEvent( pszName, TRUE )
#endif	// ME_LINUX
{
#ifdef ME_WIN
	OS::EventCreate(
		m_pEvent,
		pszName,
		bManualReset,
		bSignaled );

#endif	// ME_WIN
}

CMELockEvent::~CMELockEvent()
{
#ifdef ME_WIN
	OS::EventDestroy( m_pEvent );

#else

#endif	// ME_WIN
}

ME_Result CMELockEvent::Lock()
{
	return Wait();
}

ME_Result CMELockEvent::UnLock()
{
	return Signal();
}

ME_Result CMELockEvent::Reset()
{
#ifdef ME_WIN
	return OS::EventReset( m_pEvent );

#else
	return ME_OK;

#endif	// ME_WIN
}

ME_Result CMELockEvent::Signal()
{
#ifdef ME_WIN
	return OS::EventSignal( m_pEvent );

#else
	m_pEvent.Lock();
	ME_Result hResult = m_pEvent.Signal();
	m_pEvent.UnLock();

	return hResult;

#endif	// ME_WIN
}

ME_Result CMELockEvent::Wait()
{
#ifdef ME_WIN
	return OS::EventWait( m_pEvent );

#else
	m_pEvent.Lock();
	ME_Result hResult = m_pEvent.Wait();
	m_pEvent.UnLock();

	return hResult;

#endif	// ME_WIN
}

ME_Result CMELockEvent::Wait( CMETimeValue* pTimeout )
{
#ifdef ME_WIN
	return OS::EventTimeWait(
		m_pEvent,
		*pTimeout );

#else
	m_pEvent.Lock();
	ME_Result hResult = m_pEvent.Wait( pTimeout );
	m_pEvent.UnLock();

	return hResult;

#endif	// ME_WIN
}

ME_NAME_SPACE_END


