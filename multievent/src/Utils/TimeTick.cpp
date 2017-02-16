#include "TimeTick.h"

ME_NAME_SPACE_BEGIN

const DWORD CTimeTick::MAX_TICK_COUNT = (DWORD)(-1);

CTimeTick::CTimeTick()
{
	Set(
		0,
		GetSysTickCount() );
}

CTimeTick::CTimeTick( 
	DWORD dwRound, 
	DWORD dwTimeCount )
{
	Set(
		dwRound,
		dwTimeCount );
}

CTimeTick::CTimeTick( const ME_TIME_TICK& httTick )
{
	Set( httTick );
}

CTimeTick::~CTimeTick()
{

}

DWORD CTimeTick::GetSysTickCount()
{
#ifdef ME_WIN
	return ::GetTickCount();

#elif defined( ME_LINUX )
	struct timeval timeVal;	
	::gettimeofday(
		&timeVal, 
		NULL);

	DWORD dwTickCount = ((timeVal.tv_sec * 1000) + (timeVal.tv_usec / 1000)); 

	return dwTickCount;

#endif	// ME_WIN
}

void CTimeTick::Set( const ME_TIME_TICK& httTimeTick )
{
	Set(
		httTimeTick.dwRound,
		httTimeTick.dwTickCount);
}

void CTimeTick::Set( 
	DWORD dwRound, 
	DWORD dwTickCount )
{
	m_httTick.dwRound = dwRound;
	m_httTick.dwTickCount = dwTickCount;
}

const ME_TIME_TICK& CTimeTick::Get() const
{
	return m_httTick;
}

DWORD CTimeTick::GetRound() const
{
	return m_httTick.dwRound;
}

DWORD CTimeTick::GetTickCount() const
{
	return m_httTick.dwTickCount;
}

void CTimeTick::Update(
	DWORD dwTickCount, 
	BOOL bAddRound /* = FALSE */ )
{
	if ( TRUE == bAddRound )
	{
		m_httTick.dwRound++;
	}

	m_httTick.dwTickCount = dwTickCount;
}

const CTimeTick& CTimeTick::operator = ( const CTimeValue& htvTime )
{
	Set(
		0,
		(DWORD)htvTime.GetMilliSecond() );

	return *this;
}

const CTimeTick& CTimeTick::operator = ( const ME_TIME_TICK& httTick )
{
	Set( httTick );

	return *this;
}

BOOL CTimeTick::operator < ( const CTimeTick& httDest ) const
{
	/* 如果循环轮次一样 */
	if ( m_httTick.dwRound == httDest.m_httTick.dwRound ) 
	{
		/* 直接比大小 */
		return (m_httTick.dwTickCount < httDest.m_httTick.dwTickCount);
	}
	else
	{
		/* 否则直接比轮次 */
		return (m_httTick.dwRound < httDest.m_httTick.dwRound);
	}
}

BOOL CTimeTick::operator > ( const CTimeTick& httDest ) const
{
	/* 如果循环轮次一样 */
	if ( m_httTick.dwRound == httDest.m_httTick.dwRound ) 
	{
		/* 直接比大小 */
		return (m_httTick.dwTickCount > httDest.m_httTick.dwTickCount);
	}
	else
	{
		/* 否则直接比轮次 */
		return (m_httTick.dwRound > httDest.m_httTick.dwRound);
	}
}

ME_NAME_SPACE_END


