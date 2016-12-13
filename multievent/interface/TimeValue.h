/**
* 时间相关
* 
*
* History
*	12/04/2012		Created
*/

#ifndef __TIME_VALUE_H_
#define __TIME_VALUE_H_

#include "Base.h"

NAME_SPACE_BEGIN

/**
* 这个类只提供一些简单的时间功能，如获取当前的秒，
* 毫秒之类的还是支持，但诸如时间格式化(什么年月日星期之类的)
* 都不支持
*
* 它的最大用处在于作为CHikTimer的标准输入函数
*/
class OS_EXPORT CHikTimeValue
{
public:
	CTimeValue();

	CTimeValue( const CHikTimeValue& htvTime );

	CTimeValue(
		LONGLONG llSecond,
		LONG lMicrosecond = 0 );

	CHikTimeValue( const ME_TIME_VALUE& htaTime );

	CHikTimeValue( const timeval& tvTime );

#ifdef WIN
	CTimeValue( const FILETIME& ftTime );
#endif	// WIN

public:
	/**
	* 从1970-01-01 00:00 (UTC)到你调用函数时
	* 一共“过去”的时间(秒和微秒)
	*/
	static CTimeValue TimeOfDay();

public:
	/**
	* 根据<htvTime>来设置TimeValue
	* 
	* 返回值：
	*		无
	*/
	void Set( const CHikTimeValue& htvTime );

	/**
	* 根据<llSecond>、<lMicrosecond>来设置TimeValue
	* 
	* 返回值：
	*		无
	*/
	void Set(
		LONGLONG llSecond,
		LONG lMicrosecond = 0 );

	/**
	* 根据<htvTime>来设置TimeValue
	* 
	* 返回值：
	*		无
	*/
	void Set( const ME_TIME_VALUE& htvTime );

	/**
	* 根据<tvTime>来设置TimeValue
	* 
	* 返回值：
	*		无
	*/
	void Set( const timeval& tvTime );

#ifdef WIN
	void Set( const FILETIME& ftTime );
#endif	// ME_WIN

	/**
	* 获取秒
	*/
	LONGLONG GetSecond() const;

	/**
	* 获取微秒
	*/
	LONG GetMicrosecond() const;

	/**
	* 获取毫秒
	*/
	LONGLONG GetMilliSecond() const;

public:
	CTimeValue& operator + ( const CTimeValue& htvTime );

private:
	void BounderCheck();

private:
	enum
	{
		/* 1秒等于1000毫秒 */
		ONE_SECOND_IN_MILLI_SECOND = 1000,

		/* 1秒等于1000000微秒 */
		ONE_SECOND_IN_MACRO_SECOND = 1000000,

		/* 1秒等于1000000个100纳秒 */
		ONE_SECOND_IN_100_NANO_SECOND = 10000000,

		/* 1秒等于1000000000纳秒 */
		ONE_SECOND_IN_NANO_SECOND = 1000000000,
	};

private:
	ME_TIME_VALUE					m_Time;	// 实际存储的时间结构
};

ME_NAME_SPACE_END

#endif	// __TIME_VALUE_H_




