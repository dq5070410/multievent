/**
* Event相关操作的API
* 
*
* History
*	2/18/2016		Created
*/

#ifndef __OS_EVENT_H_
#define __OS_EVENT_H_

#include "Base.h"
#include "TimeValue.h"

NAME_SPACE_OS_BEGIN

/**
* 创建一个事件
*
* <pEvent>，out，成功应该为非NULL
* <pszName>，事件名字
* <bManualReset>，是否需要手动重置状态
* <bSignaled>，是否有信号
*
* 返回值：
*		ME_OK为成功；其他失败
*/
ME_Result ME_OS_EXPORT EventCreate(
	ME_EVENT& pEvent,
	const char* pszName = NULL,
	BOOL bManualReset = FALSE,
	BOOL bSignaled = FALSE );

/**
* 销毁一个事件
*
* <pEvent>，哪个事件需要销毁
*
* 返回值：
*		ME_OK为成功；其他失败
*/
ME_Result ME_OS_EXPORT EventDestroy( ME_EVENT& pEvent );

/**
* 销毁一个事件
*
* <pEvent>，哪个事件需要销毁
*
* 返回值：
*		ME_OK为成功；其他失败
*/
ME_Result ME_OS_EXPORT EventWait( ME_EVENT pEvent );

/**
* 等待一个事件变为“有信号”的状态
*
* 这个函数会阻塞
*
* <pEvent>，哪个事件
* <htvTime>，等待超时时间
*
* 返回值：
*		ME_OK为成功；其他失败
*/
ME_Result ME_OS_EXPORT EventTimeWait( 
	ME_EVENT pEvent,
	const CTimeValue& htvTime );

/**
* 把一个事件置为“有信号”的状态
*
* <pEvent>，哪个事件
*
* 返回值：
*		ME_OK为成功；其他失败
*/
ME_Result ME_OS_EXPORT EventSignal( ME_EVENT pEvent );

/**
* 把一个事件置为“无信号”的状态
*
* <pEvent>，哪个事件
*
* 返回值：
*		ME_OK为成功；其他失败
*/
ME_Result ME_OS_EXPORT EventReset( ME_EVENT pEvent );

NAME_SPACE_OS_END

#endif	// __OS_EVENT_H_
