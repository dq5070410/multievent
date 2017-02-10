/**
* 用于确保多线程删除安全的类
*
* 通过在对象被创建出来的线程以及网络线程之间一共3次
* 事件来回, 确保网络线程中需要callback到该对象的事件
* 被清空, 且再一次回到被创建的线程中之后, 再删除对象
*
*
* History
*	3/1/2016		Created
*/

#ifndef __MT_SAFE_DESTROY_H_
#define __MT_SAFE_DESTROY_H_

#include "Base.h"
#include "CustomizeEvent.h"
#include "LockEvent.h"

ME_NAME_SPACE_BEGIN

/**
* 用于确保多线程删除安全的类
*
* 通过在对象被创建出来的线程以及网络线程之间一共3次
* 事件来回, 确保网络线程中需要callback到该对象的事件
* 被清空, 且再一次回到被创建的线程中之后, 再删除对象
*/
class CMEMTSafeDestroy : public ICustomizeEvent
{
public:
	/* 对象删除状态 */
	typedef DWORD DESTROY_STATUS;
	enum
	{
		DESTROY_STATUS_NONE,

		/* 在当前线程尝试删除 */
		DESTROY_STATUS_CALL_IN_CURRENT_THREAD,

		/* 在网络线程尝试删除 */
		DESTROY_STATUS_CALL_IN_NETWORK_THREAD,

		/* 回到当前线程尝试真正的删除 */
		DESTROY_STATUS_BACK_TO_CURRENT_THREAD,
	};

public:
	CMEMTSafeDestroy();

	virtual ~CMEMTSafeDestroy();

	/* 继承自IMECustomizeEvent */
public:
	/**
	* 当事件发生时，这个回调会通知你
	*
	* 事件有同步异步之分，在同步的情况下，或许存在需要知道
	* 事件执行结果的这么一种情况，所以给原本应当是callback
	* 的函数加上了返回值
	*/
	virtual ME_Result OnEvent();

	/**
	* 在OnEvent() callback之后，OnDestroy()紧接着会被
	* 调用，用来销毁这个事件。这个函数有默认行为，就是
	* delete this，如果你有特殊要求，可以重载这个函数
	*
	* 从功能上来讲，这个函数不是一个回调，不用在函数名上
	* 加On，但是，考虑到与OnEvent对仗，就加上了
	*
	* 事件有同步异步之分，在同步的情况下，或许存在需要知道
	* 事件执行结果的这么一种情况，所以给原本应当是callback
	* 的函数加上了返回值
	*/
	virtual ME_Result OnDestroy();

protected:

	/*
	* 在被创建线程尝试删除对象
	* 根据当前状态不同, 处理也不同
	* 当状态处于DESTROY_STATUS_CALL_IN_CURRENT_THREAD时,
	* 函数只是向网络线程投递一个事件, 确保在网络线程里,
	* 有关于该对象的事件在真正执行删除之前, 被清空;
	* 当状态处于DESTROY_STATUS_BACK_TO_CURRENT_THREAD时,
	* 执行删除操作
	*
	* 返回值:
	*		无
	*/
	virtual void Destroy_u();

	/*
	* 在网络线程尝试删除对象
	* 这个函数并不会真正的删除对象, 只是向对象被创建的线程
	* 投递一个事件, 通知对象, 与对象相关网络线程的事件已经
	* 已经清空了, 可以删除对象了
	*
	* 返回值:
	*		无
	*/
	virtual void Destroy_n();

protected:
	DESTROY_STATUS					m_dwStatus;		// 当前状态
	CLockEvent					    m_Event;		// 用于确保网络线程清理完毕之后，再delete
};

ME_NAME_SPACE_END

#endif	// __MT_SAFE_DESTROY_H_


