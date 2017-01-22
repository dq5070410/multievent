/**
* 事件(锁)的实现类
*
* 在win32环境下，它就是事件(event)，在linux环境下用
* 条件变量模拟实现
* 
*
* History
*	12/18/2016		Created
*/

#ifndef __LOCK_EVENT_H_
#define __LOCK_EVENT_H_

#include "Base.h"

#ifndef ME_WIN
#include "LockConditionVariable.h"
#endif	// WIN32

ME_NAME_SPACE_BEGIN

class CTimeValue;

/**
* 事件(锁)的实现类
*
* 在win32环境下，它就是事件(event)，在linux环境下用
* 条件变量模拟实现
*/
class ME_OS_EXPORT CLockEvent
{
public:
	explicit CLockEvent(
		BOOL bManualReset = FALSE,
		BOOL bSignaled = FALSE,
		const char* pszName = NULL );

	~CLockEvent();

public:
	/**
	* 锁定事件本身(与调用Signal函数效果相同)
	*
	* 这个方法会阻塞
	*
	* 返回值：
	*		ME_OK为成功；其他失败
	*/
	ME_Result Lock();

	/**
	* 解锁当前事件(与调用Reset函数效果相同)
	*
	* 返回值：
	*		ME_OK为成功；其他失败
	*/
	ME_Result UnLock();

	/**
	* 把事件置为“有信号”的状态
	*
	* 返回值：
	*		ME_OK为成功；其他失败
	*/
	ME_Result Signal();

	/**
	* 把事件置为“无信号”的状态
	*
	* 返回值：
	*		ME_OK为成功；其他失败
	*/
	ME_Result Reset();

	/**
	* 等待这个事件被置为“有信号”的状态
	*
	* 这个函数会阻塞
	*
	* 返回值：
	*		ME_OK为成功；其他失败
	*/
	ME_Result Wait();

	/**
	* 等待这个事件被置为“有信号”的状态
	*
	* 这个函数会阻塞
	*
	* <pTimeout>，等待多长时间
	*
	* 返回值：
	*		ME_OK为成功；ME_ERROR_TIMEOUT为超时；其他失败
	*/
	ME_Result Wait( CTimeValue* pTimeout );

private:
	void operator = ( const CLockEvent& );
	CMELockEvent( const CLockEvent& );

private:
#ifdef WIN
	ME_EVENT						m_pEvent;
#else
	CLockConditionVariable		m_pEvent;
#endif	// WIN
};

ME_NAME_SPACE_END

#endif	// __LOCK_EVENT_H_


