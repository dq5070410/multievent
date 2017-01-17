/**
* 事件队列的封装模板类
* 
*
* History
*	12/05/2016		Created
*/

#ifndef __EVENT_QUEUE_T_H_
#define __EVENT_QUEUE_T_H_

#include "Base.h"
#include "EventNodeT.h"
#include "NodeListT.h"
#include "LockGuardT.h"
#include "LockThreadMutex.h"
#include "CustomizeEvent.h"

#include "SingletonT.h"

ME_NAME_SPACE_BEGIN

/**
* 事件队列的封装模板类
*/
template <class EventType, class LockType>
class CEventQueueT
{
public:
	typedef CEventNodeT<EventType> NodeType;
	typedef typename NodeType::CSyncEvent SyncEvent;

public:
	CEventQueueT();

	~CEventQueueT();

public:
	/**
	* 投递一个事件
	* 
	* <pEvent>，需要投递的事件
	*
	* 返回值：
	*		ME_OK为成功；其他为失败
	*/
	ME_Result PostEvent( EventType* pEvent );

	/**
	* 投递一个事件
	* 
	* <pEvent>，需要投递的事件
	* <nMode>，事件工作的方式
	* <pSyncEvent>，同步事件，当nMode为SEND时，这个值必须不为NULL
	*
	* 返回值：
	*		ME_OK为成功；其他为失败
	*/
	ME_Result PostEvent(
		EventType* pEvent,
		typename NodeType::MODE nMode,
		SyncEvent* pSyncEvent = NULL );

	/**
	* 是不是需要这么个参数还没来得及细想，先放着
	*/
	ME_Result HandleEvent( BOOL& bExit );

	/**
	* 事件队列是不是empty的
	*/
	BOOL IsEmpty();

	/**
	* 使事件队列停止工作
	*/
	void Destroy();

public:
	/**
	* 重载赋值操作符
	*
	* 避免在赋值的过程中，由于某些变量默认的赋值操作符行为而导致的
	* 异常(如锁<m_Lock>，如果是 Critical Setion类型，会导致其成员
	* 变量DebugInfo会与下一个同样是赋值操作过的CS实例的DebugInfo
	* 内存地址是同一个，而最终导致double delete)
	*/
	void operator = ( const CEventQueueT<EventType, LockType>& Dst );

private:
	typedef CNodeListT<NodeType> ListType;

private:
	ListType						m_nodeList;	// 事件存储结构
	LockType						m_Lock;		// 锁
	BOOL							m_bExit;	// 事件队列是否处于退出状态
};

typedef CEventQueueT<IMECustomizeEvent, CLockThreadMutex> CEventQueue;	// 事件队列

typedef CSingletonT<CEventQueue> CEventQueueSingleton;

#include "EventQueueT.hpp"

ME_NAME_SPACE_END

#endif	// __ME_EVENT_QUEUE_T_H_