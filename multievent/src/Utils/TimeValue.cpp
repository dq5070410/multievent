#include "TimeValue.h"

ME_NAME_SPACE_BEGIN

CTimeValue CTimeValue::TimeOfDay()
{
#ifdef ME_WIN
	FILETIME ftTime;
	::GetSystemTimeAsFileTime( &ftTime );

	return CTimeValue( ftTime );

#elif defined( ME_LINUX )
	timeval tvTime;
	::gettimeofday( 
		&tvTime, 
		NULL );

	return CTimeValue( tvTime );

#else
	ME_ERROR_TRACE( "the platform you working on is not supported" );

	return CTimeValue( 0 );
#endif	// ME_WIN
}

CTimeValue::CTimeValue()
{
	Set( 0 );
}

CTimeValue::CTimeValue( const CTimeValue& htvTime )
{
	Set( htvTime );
}

CTimeValue::CTimeValue( 
	LONGLONG llSecond, 
	LONG lMicrosecond /* = 0 */ )
{
	Set( 
		llSecond,
		lMicrosecond );
}

CTimeValue::CTimeValue( const ME_TIME_VALUE& htvTime )
{
	Set( htvTime );
}

CTimeValue::CTimeValue( const timeval& tvTime )
{
	Set( tvTime );
}

#ifdef ME_WIN
CTimeValue::CTimeValue( const FILETIME& ftTime )
{
	Set( ftTime );
}
#endif	// ME_WIN

void CTimeValue::Set( const CTimeValue& htvTime )
{
	ME_ASSERTE_RETURN_VOID( &htvTime != this );

	*this = htvTime;
}

void CTimeValue::Set( 
	LONGLONG llSecond, 
	LONG lMicrosecond /* = 0 */ )
{
	m_Time.llSecond = llSecond;
	m_Time.lMicrosecond = lMicrosecond;

	BounderCheck();
}

void CTimeValue::Set( const ME_TIME_VALUE& htaTime )
{
	m_Time.llSecond = htaTime.llSecond;
	m_Time.lMicrosecond = htaTime.lMicrosecond;

	BounderCheck();
}

void CTimeValue::Set( const timeval& tvTime )
{
	m_Time.llSecond = tvTime.tv_sec;
	m_Time.lMicrosecond = tvTime.tv_usec;

	BounderCheck();
}

#ifdef ME_WIN

/* UTC时间1601年1月1日00:00 到1970年1月1日00:00 总共过去了多少个100纳秒*/
const DWORDLONG MS_FILETIME_SPEC = 0x19db1ded53e8000;

void CTimeValue::Set( const FILETIME& ftTime )
{
	/**
	* 文件时间是一个64位的值，以100纳秒为间隔，计算从
	* UTC时间00:00 1601年1月1日到现在，总共“过去”了
	* 多少个100纳秒
	*/
	ULARGE_INTEGER ui100ns;
	ui100ns.LowPart = ftTime.dwLowDateTime;
	ui100ns.HighPart = ftTime.dwHighDateTime;

	/**
	* 大家都是从1970年开始算，微软偏要与众不同，于是才
	* 有了下面这一段
	*/
	ui100ns.QuadPart -= MS_FILETIME_SPEC;
	m_Time.llSecond = (ui100ns.QuadPart / ONE_SECOND_IN_100_NANO_SECOND);

	/* 1000纳秒 = 1微秒，所以N个100ns再除一下就得到微秒 */
	m_Time.lMicrosecond = (ui100ns.QuadPart % ONE_SECOND_IN_100_NANO_SECOND) / (ONE_SECOND_IN_100_NANO_SECOND / ONE_SECOND_IN_MACRO_SECOND);
}
#endif	// ME_WIN

LONGLONG CTimeValue::GetSecond() const
{
	return m_Time.llSecond;
}

LONG CTimeValue::GetMicrosecond() const
{
	return m_Time.lMicrosecond;
}

LONGLONG CTimeValue::GetMilliSecond() const
{
	return (m_Time.llSecond * 1000 + m_Time.lMicrosecond / 1000);
}

CTimeValue& CTimeValue::operator + ( const CTimeValue& htvTime )
{
	Set(
		GetSecond() + htvTime.GetSecond(),
		GetMicrosecond() + htvTime.GetMicrosecond() );

	BounderCheck();

	return (*this);
}

void CTimeValue::BounderCheck()
{
	/* 如果<m_Time.lMicrosecond>大于边界值, 即超过1秒 */
	if ( ONE_SECOND_IN_MACRO_SECOND <= m_Time.lMicrosecond ) 
	{
		do 
		{
			m_Time.llSecond++;
			m_Time.lMicrosecond -= ONE_SECOND_IN_MACRO_SECOND;
		}
		while( ONE_SECOND_IN_MACRO_SECOND <= m_Time.lMicrosecond );
	}
	/* 如果<m_Time.lMicrosecond>小于负的边界值, 即小于1微秒 */
	else if ( -ONE_SECOND_IN_MACRO_SECOND >= m_Time.lMicrosecond ) 
	{
		do 
		{
			m_Time.llSecond--;
			m_Time.lMicrosecond += ONE_SECOND_IN_MACRO_SECOND;
		}
		while( -ONE_SECOND_IN_MACRO_SECOND >= m_Time.lMicrosecond );
	}

	/**
	* 为了防止上边的语句执行之后，还有些“异常”
	* 下面再排查一下
	*/

	/* 如果1秒有余而微秒不足 */
	if ( (1 <= m_Time.llSecond) && (0 > m_Time.lMicrosecond) ) 
	{
		m_Time.llSecond--;
		m_Time.lMicrosecond += ONE_SECOND_IN_MACRO_SECOND;
	}
	else if ( (0 > m_Time.llSecond) && (0 < m_Time.lMicrosecond) ) 
	{
		m_Time.llSecond++;
		m_Time.lMicrosecond -= ONE_SECOND_IN_MACRO_SECOND;
	}
}

ME_NAME_SPACE_END

