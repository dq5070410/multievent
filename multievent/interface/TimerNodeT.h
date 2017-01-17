/**
* 时钟队列节点的封装类
* 
*
* History
*	2/05/2016		Created
*/
#ifndef __TIME_NODE_T_H_
#define __TIME_NODE_T_H_

#include "Base.h"
#include "TimeValue.h"
#include "TimeTick.h"
#include "Timer.h"

NAME_SPACE_BEGIN

/**
* 时钟队列节点的封装类
*/
template <class TimerType, class TimerSinkType>
class CTimerNodeT
{
public:
	typedef CTimerNodeT<TimerType, TimerSinkType> NodeType;
	typedef TimerType SuperType;

public:
	CTimerNodeT();

	/**
	* 为了符合CMENodeListT的要求而添加
	*/
	CTimerNodeT( SuperType* pSuper );

	CTimerNodeT(
		TimerType* pTimer,
		TimerSinkType* pSink,
		const CTimeValue& htvInterval,
		const CTimeValue& htvDelay,
		int iLoopTime = 0 );

	~CTimerNodeT();

public:
	/**
	* 设置时钟节点
	*
	* <pTimer>，谁schedule了这个时钟
	* <pSink>，回调对象(当时钟发生时通知它)
	* <httInterval>，时钟的间隔
	* <httDelay>，延迟多长时间后开始
	* <iLoopTime>，循环多少次
	*
	* 返回值：
	*		ME_OK为成功；其他为失败
	*/
	ME_Result Set(
		TimerType* pTimer,
		TimerSinkType* pSink,
		const CTimeTick& httInterval,
		const CTimeTick& httDelay,
		int iLoopTime = 0 );

	/**
	* 设置time tick
	*
	* <httTimeTick>，根据什么来设置time tick
	*
	* 返回值：
	*		无
	*/
	void SetTimeTick( const CTimeTick& httTimeTick );

	/**
	* 设置time tick
	*
	* <dwRound>，轮次
	* <dwTickCount>，tick count
	*
	* 返回值：
	*		无
	*/
	void SetTimeTick(
		DWORD dwRound,
		DWORD dwTickCount );

	/**
	* 减少当前节点一轮次
	*
	* 返回值：
	*		当前节点的执行轮次
	*/
	int ReduceLoopTime();

	/**
	* 获取当前节点的time tick
	*
	* 返回值：
	*		CTimeTick
	*/
	const CTimeTick& GetTimeTick() const;

	/**
	* 获取当前节点的time count
	*
	* 返回值：
	*		DWORD范围的任意值
	*/
	DWORD GetTimeCount() const;

	/**
	* 获取当前节点的时钟间隔(tick count，也就是毫秒ms)
	*
	* 返回值：
	*		DWORD范围的任意值
	*/
	DWORD GetIntervalTimeCount() const;

	/**
	* Hit timer，让timer事件发生
	*/
	ME_Result Active();

public:
	BOOL operator == ( const NodeType& ntNode );
	
	BOOL operator == ( const SuperType& stSuper );

	BOOL operator > ( const NodeType& ntNode );

	BOOL operator < ( const NodeType& ntNode );

private:
	TimerType*						m_pTimer;			// 谁call的
	TimerSinkType*					m_pTimerSink;		// callback对象

	CTimeTick					    m_httDelay;			// 延后多长时间开始计算tick
	CTimeTick					    m_httInterval;		// tick的间隔
	int								m_iLoopTime;		// 循环次数
};

#include "TimerNodeT.hpp"

NAME_SPACE_END

#endif	// __TIME_NODE_T_H_










