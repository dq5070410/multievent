/**
* 定时器
* 
*
* History
*	2/1/2016		Created
*/

#ifndef __TIME_H_
#define __TIME_H_

#include "Base.h"

NAME_SPACE_BEGIN

class ITimerSink;
class CTimeValue;

/**
* 定时器
*/
class ME_OS_EXPORT CMETimer
{
public:
	CMETimer();

	~CMETimer();

public:
	/**
	* 启动一个timer
	* 
	* 如果你反复的schedule同一个时钟，只有最后一次的schedule是起作用的
	*
	* <pSink>，时钟回调对象，必须要设
	* <htvInterval>，间隔，多长时间来一次，发生多少次取决于<iLoopTime>
	* <htvDelay>，延迟，当这个时钟启动后多长时间发生
	* <iLoopTime>，一共发生多少次，注意，“延迟”那一次是不计算在内的，如果你想
	*				这个timer持续不断的发生，那么-1就是你需要填入的值
	*
	* 返回值:
	*		ME_OK为成功；其余为失败
	*/
	ME_Result Schedule(
		ITimerSink* pSink,
		const CTimeValue& htvInterval,
		const CTimeValue& htvDelay,
		int iLoopTime );

	/**
	* 启动一个timer
	*
	* 如果你反复的schedule同一个时钟，只有最后一次的schedule是起作用的
	*
	* 通过这个方法启动的timer执行且只会执行一次，比如，你通过这个方法
	* 启动了timer，设定<htvInterval>为500ms，那么它只会在500ms后发生
	* 一次，之后就没了
	*
	* <pSink>，时钟回调对象，必须要设
	* <htvInterval>，间隔，多长时间来一次
	*
	* 返回值:
	*		ME_OK为成功；其余为失败
	*/
	ME_Result Schedule(
		ITimerSink* pSink,
		const CTimeValue& htvInterval );

	/**
	* 启动一个timer
	*
	* 如果你反复的schedule同一个时钟，只有最后一次的schedule是起作用的
	*
	* 通过这个方法启动的timer会执行<iLoopTime>次，比如，你通过这个方法
	* 启动了timer，设定<htvInterval>为500ms，那么它只会在500ms后发生
	*
	* <pSink>，时钟回调对象，必须要设
	* <htvInterval>，间隔，多长时间来一次，发生多少次取决于<iLoopTime>
	* <iLoopTime>，一共发生多少次，注意，“延迟”那一次是不计算在内的，如果你想
	*				这个timer持续不断的发生，那么-1就是你需要填入的值
	*
	* 返回值:
	*		ME_OK为成功；其余为失败
	*/
	ME_Result Schedule(
		ITimerSink* pSink,
		const CTimeValue& htvInterval,
		int iLoopTime );

	/**
	* 取消当前的timer
	*
	* 返回值:
	*		无
	*/
	void Cancel();
};

/**
* 定时器回调对象
*
* 如果你要想在schedule了之后有所反应，就必须继承这个类
*/
class ITimerSink
{
public:
	virtual ~ITimerSink() {}

public:
	/**
	* 你schedule的时钟发生了
	*
	* <pTimerID>，哪个时钟发生了
	*
	* 返回值:
	*		无
	*/
	virtual void OnTimer( CMETimer* pTimerID ) = 0;
};


NAME_SPACE_END

#endif	// __ME_TIME_H_

















