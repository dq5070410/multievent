/**
* 时钟队列的封装模板类
* 
*
* History
*	2/05/2016		Created
*/
#ifndef __TIME_QUEUE_T_H_
#define __TIME_QUEUE_T_H_

#include "Base.h"
#include "TimerNodeT.h"
#include "NodeListT.h"
#include "LockGuardT.h"
#include "Timer.h"
#include "LockThreadMutex.h"
#include "SingletonT.h"

ME_NAME_SPACE_BEGIN

/**
* 时钟队列的封装模板类
*/
template <class TimerType, class TimerSinkType, class LockType>
class CMETimerQueueT
{
public:
	typedef CTimerNodeT<TimerType, TimerSinkType> NodeType;

public:
	enum
	{
		MINIMUM_TIMER_INTERVAL = 50, // 最小时钟间隔，当ME_TIME_LIMIT开启的时候会强制检查
	};

public:
	CMETimerQueueT();

	~CMETimerQueueT();

public:
	/**
	* 启动一个timer
	* 
	* 如果你反复的schedule同一个时钟，只有最后一次的schedule是起作用的
	*
	* <pTimer>，谁启动了这个时钟
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
		TimerType* pTimer,
		TimerSinkType* pSink,
		const CTimeValue& htvInterval,
		const CTimeValue& htvDelay,
		int iLoopTime = 0 );

	/**
	* 取消时钟
	*
	* <pTimer>，取消哪个时钟
	*
	* 返回值:
	*		ME_OK为成功；其他为失败
	*/
	ME_Result Cancel( TimerType* pTimer );

	/**
	* 获取最早发生的一个时钟的tick count(也就是毫秒)
	*
	* 返回值：
	*		UINT范围内的任意值；当时钟队列为空时返回-1
	*/
	UINT GetNextTimeout();

	/**
	* 检查哪个时钟到点了并激活它
	*
	* 返回值：
	*		本次被激活的时钟个数(UINT范围内的任意值)
	*/
	UINT Expire();

	/**
	* 使时钟队列停止工作
	*
	* 返回值：
	*		无
	*/
	void Destroy();

private:
	/**
	* 设置时钟节点
	*
	* <pNode>，需要设置的时钟节点
	* <pTimer>，谁启动了这个时钟
	* <pSink>，时钟回调对象，必须要设
	* <htvInterval>，间隔，多长时间来一次，发生多少次取决于<iLoopTime>
	* <htvDelay>，延迟，当这个时钟启动后多长时间发生
	* <iLoopTime>，一共发生多少次，注意，“延迟”那一次是不计算在内的，如果你想
	*				这个timer持续不断的发生，那么-1就是你需要填入的值
	*
	* 返回值：
	*		ME_OK为成功；其他为失败
	*/
	ME_Result SetupNode(
		NodeType* pNode,
		TimerType* pTimer,
		TimerSinkType* pSink,
		const CTimeValue& htvInterval,
		const CTimeValue& htvDelay,
		int iLoopTime = 0 );

	/**
	* 更新标准时钟
	*
	* 把当前时刻的系统tickcount存储起来
	*
	* 返回值：
	*		无
	*/
	void UpdateStandardTick();

	/**
	* 根据标准时钟更新节点
	*
	* <pNode>，哪个节点
	*
	* 返回值：
	*		无
	*/
	void UpdateNode( NodeType* pNode );

	/**
	* 根据标准时钟，以及<dwTickStep>来校准，并把结果
	* 存入<httTick>
	*
	* <httTick>，out
	* <dwTickStep>，需要调整多少tick count
	*/
	void AdjustTick(
		CTimeTick& httTick,
		DWORD dwTickStep );

public:
	/**
	* 重载赋值操作符
	*
	* 避免在赋值的过程中，由于某些变量默认的赋值操作符行为而导致的
	* 异常(如锁<m_Lock>，如果是 Critical Setion类型，会导致其成员
	* 变量DebugInfo会与下一个同样是赋值操作过的CS实例的DebugInfo
	* 内存地址是同一个，而最终导致double delete)
	*/
	void operator = ( const CMETimerQueueT<TimerType, TimerSinkType, LockType>& Dst );

private:
	/**
	* 用于排序的仿函数(使用小于算法)
	*/
	template <class TimerNodeType>
	class CTimerNodeLesser
	{
	public:
		BOOL operator () ( const TimerNodeType& pSrc, const TimerNodeType& pDst );
	};

	typedef CTimerNodeLesser<NodeType*> SorterType;

private:
	LockType						m_Lock;			// 锁
	BOOL							m_bExit;		// 时钟队列是否处于退出状态

	/**
	* 用于记录系统的tickcount
	*
	* 由于tickcount是一个DWORD值，DOWRD的最大值，按毫秒计算也就是差不多49.7天
	* 这里就存在一个边界，也就是在快到49.7天的时候schedule一个timer的话，就有
	* 可能在第二轮的49.7天里才会被执行，换句话来说，也就是被执行时的tickcount
	* 有可能会比它被schedule的时候小，所以，必须用<m_httStandard>来记录tickcount，
	* 以便于在发生上述情况时，能够通过<m_httStandard>的轮数(dwRound)来识别
	*/
	CTimeTick					m_httStandard;
	LockType						m_LockStandard;	// 用一个单独的锁保护<m_httStandard>

private:
	typedef CMENodeListT<NodeType, SorterType> ListType;
	ListType						m_nodeList;		// 节点链表
};

typedef CMETimerQueueT<CMETimer, ITimerSink, CMELockThreadMutex> CMETimerQueue;	// 时钟队列

typedef CMESingletonT<CMETimerQueue> CMETimerQueueSingleton;

#include "TimerQueueT.hpp"

ME_NAME_SPACE_END


#endif	// __TIME_QUEUE_T_H_





