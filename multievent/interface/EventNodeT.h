/**
* 事件队列节点的封装类
* 
*
* History
*	2/05/2012		Created
*/
#ifndef __EVENT_NODE_T_H_
#define __EVENT_NODE_T_H_

#include "Base.h"
#include "LockEvent.h"

NAME_SPACE_BEGIN

/**
* 事件队列节点的封装类
*/
template <class EventType>
class CEventNodeT
{
public:
	typedef CEventNodeT<EventType> NodeType;
	typedef EventType SuperType;

public:
	enum MODE
	{
		MODE_POST = 100,	// Post工作模式，非阻塞
		MODE_SEND,			// Send工作模式，阻塞
	};

	/**
	* 事件的优先级
	*
	* 目前暂不使用，预留
	*/
	enum PRIORITY
	{
		PRIORITY_LOW = 200,		// 低优先
		PRIORITY_NORMAL,		// 普通
		PRIORITY_HIGH,			// 高优先
	};

	class CHikSyncEvent;

public:
	CEventNodeT();

	/**
	* 为了符合CHikNodeListT的要求而添加
	*/
	CEventNodeT( SuperType* pSuper );

	/**
	* 同时，为了避免默认参数而导致的编译器无法识别到底是
	* 调用哪个构造函数，只能把原来的默认参数去掉
	*/
	/*CHikEventNodeT(
		EventType* pEvent,
		MODE nMode = MODE_POST,
		CHikSyncEvent* pSync = NULL,
		PRIORITY nPriority = PRIORITY_NORMAL );*/

	CEventNodeT(
		EventType* pEvent,
		MODE nMode,
		CHikSyncEvent* pSync,
		PRIORITY nPriority = PRIORITY_NORMAL );

public:
	/**
	* 设置Node节点保存的event
	*
	* <pEvent>，需要执行的事件
	* <nMode>，工作方式(POST还是SEND)
	*
	* 返回值：
	*		ME_OK为成功；其他为失败
	*/
	ME_Result Set(
		EventType* pEvent,
		MODE nMode );

	/**
	* 设置同步事件
	*
	* <pSync>，同步事件
	*
	* 返回值：
	*		ME_OK为成功；其他为失败
	*/
	ME_Result SetSyncEvent( CSyncEvent* pSync );

	/**
	* 设置事件优先级
	*
	* 预留项，目前暂不使用
	*
	* <nPriority>，优先级
	*
	* 返回值：
	*		ME_OK为成功；其他为失败
	*/
	void SetPriority( PRIORITY nPriority );

	/**
	* 获取事件优先级
	*
	* 预留项，目前暂不使用
	*
	* 返回值：
	*		200 ~ 202之间
	*/
	UINT GetPriority() const;

	/**
	* 让事件发生
	*/
	ME_Result Active();

public:
	BOOL operator == ( const EventType* pEvent );

public:
	class CSyncEvent
	{
	public:
		void Signal( ME_Result hResult = ME_OK );

		/**
		* 把GetResult() const; 改为GetResult();
		* 不然就会一直提示“error C2663: 'ME::CLockEvent::Wait' : 2 overloads have no legal conversion for 'this' pointer”
		*/
		ME_Result GetResult();

	private:
		CLockEvent			    	m_SyncEvent;		// 同步事件
		ME_Result					m_hResult;			// 事件执行结果(这不是“同步事件”执行的结果，而是“真正的”自定义事件执行结果)
	};

private:
	EventType*						m_pEvent;			// 存放需要执行的自定义事件
	MODE							m_nEventMode;		// 标识执行事件的方法Send or Post

	CSyncEvent*					m_pSync;			// 在事件执行方法为Send时，用这个来做同步阻塞操作

	PRIORITY						m_nPriority;		// 优先级，保留，目前无用
};

#include "EventNodeT.hpp"

NAME_SPACE_END

#endif	// __EVENT_NODE_T_H_




