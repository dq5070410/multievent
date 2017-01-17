/**
* tick count的封装类
*
* History
*	2/05/2016		Created
*/

#ifndef __TIME_TICK_H_
#define __TIME_TICK_H_

#include "Base.h"
#include "TimeValue.h"

NAME_SPACE_BEGIN

typedef struct tagTimeTick
{
	DWORD dwRound;		// 轮数，用于标识走过了多少轮(每轮为DWORD的最大值)
	DWORD dwTickCount;	// 当前的tickcount
} ME_TIME_TICK;


/**
* tick count的封装类
*/
class ME_OS_EXPORT CTimeTick
{
public:
	static const DWORD MAX_TICK_COUNT;

public:
	CTimeTick();

	CTimeTick( 
		DWORD dwRound,
		DWORD dwTimeCount );

	CTimeTick( const ME_TIME_TICK& httTick );

	~CTimeTick();

public:
	/**
	* 获取系统tickcount
	*
	* 返回值：
	*		DWORD范围的任意值
	*/
	static DWORD GetSysTickCount();

public:
	/**
	* 设置time tick
	*
	* 返回值：
	*		无
	*/
	void Set( const ME_TIME_TICK& httTimeTick );

	/**
	* 设置time tick
	*
	* <dwRound>，轮数
	* <dwTickCount>，tick count
	*
	* 返回值：
	*		无
	*/
	void Set( 
		DWORD dwRound, 
		DWORD dwTickCount );

	/**
	* 获取time tick
	*
	* 返回值：
	*		ME_TIME_TICK
	*/
	const ME_TIME_TICK& Get() const;

	DWORD GetRound() const;

	/**
	* 获取tick count
	* 
	* 返回值：
	*		DWORD范围的任意值
	*/
	DWORD GetTickCount() const;

	/**
	* 根据<dwTickCount>更新保存的ME_TIME_TICK
	*
	* <dwTickCount>，需要增加多少tickcount
	* <bAddRound>，轮数是否要增加
	*/
	void Update( 
		DWORD dwTickCount,
		BOOL bAddRound = FALSE );

	const CTimeTick& operator = ( const CMETimeValue& htvTime );

	const CTimeTick& operator = ( const ME_TIME_TICK& httTick );

	BOOL operator < ( const CTimeTick& httDest ) const;

	BOOL operator > ( const CTimeTick& httDest ) const;

private:
	ME_TIME_TICK					m_httTick;
};

NAME_SPACE_END

#endif	// __TIME_TICK_H_









