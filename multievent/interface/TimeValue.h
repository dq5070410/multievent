/**
* ʱ�����
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
* �����ֻ�ṩһЩ�򵥵�ʱ�书�ܣ����ȡ��ǰ���룬
* ����֮��Ļ���֧�֣�������ʱ���ʽ��(ʲô����������֮���)
* ����֧��
*
* ��������ô�������ΪCHikTimer�ı�׼���뺯��
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
	* ��1970-01-01 00:00 (UTC)������ú���ʱ
	* һ������ȥ����ʱ��(���΢��)
	*/
	static CTimeValue TimeOfDay();

public:
	/**
	* ����<htvTime>������TimeValue
	* 
	* ����ֵ��
	*		��
	*/
	void Set( const CHikTimeValue& htvTime );

	/**
	* ����<llSecond>��<lMicrosecond>������TimeValue
	* 
	* ����ֵ��
	*		��
	*/
	void Set(
		LONGLONG llSecond,
		LONG lMicrosecond = 0 );

	/**
	* ����<htvTime>������TimeValue
	* 
	* ����ֵ��
	*		��
	*/
	void Set( const ME_TIME_VALUE& htvTime );

	/**
	* ����<tvTime>������TimeValue
	* 
	* ����ֵ��
	*		��
	*/
	void Set( const timeval& tvTime );

#ifdef WIN
	void Set( const FILETIME& ftTime );
#endif	// ME_WIN

	/**
	* ��ȡ��
	*/
	LONGLONG GetSecond() const;

	/**
	* ��ȡ΢��
	*/
	LONG GetMicrosecond() const;

	/**
	* ��ȡ����
	*/
	LONGLONG GetMilliSecond() const;

public:
	CTimeValue& operator + ( const CTimeValue& htvTime );

private:
	void BounderCheck();

private:
	enum
	{
		/* 1�����1000���� */
		ONE_SECOND_IN_MILLI_SECOND = 1000,

		/* 1�����1000000΢�� */
		ONE_SECOND_IN_MACRO_SECOND = 1000000,

		/* 1�����1000000��100���� */
		ONE_SECOND_IN_100_NANO_SECOND = 10000000,

		/* 1�����1000000000���� */
		ONE_SECOND_IN_NANO_SECOND = 1000000000,
	};

private:
	ME_TIME_VALUE					m_Time;	// ʵ�ʴ洢��ʱ��ṹ
};

ME_NAME_SPACE_END

#endif	// __TIME_VALUE_H_




