/**
* MultiEvent的C接口
*
* 在任何时候，在使用MultiEvent框架里的任何东西之前
* 你都应该首先调用以下接口
* 
*
* History
*	12/26/2016		Created
*/

#ifndef __MULTI_EVENT_H_
#define __MULTI_EVENT_H_

#include "Base.h"
#include "ThreadInterface.h"

ME_NAME_SPACE_BEGIN

extern "C" 
{
	/**
	* 初始化MultiEvent
	*
	* <pUserThreadSink>，用户线程回调对象
	* <bUserThreadWorkInBlockMode>，用户线程是否需要工作在阻塞模式
	*
	* 返回值：
	*		ME_OK为成功；其他为失败
	*/
	ME_Result ME_OS_EXPORT Initialize(
		IMEThreadSink* pUserThreadSink = NULL,
		BOOL bUserThreadWorkInBlockMode = FALSE );

	/**
	* 卸载MultiEvent
	*
	* 返回值：
	*		ME_OK为成功；其他为失败
	*/
	ME_Result ME_OS_EXPORT UnInitialize();

	/**
	* 运行用户线程(一次)
	*
	* 当且仅当用户线程工作在阻塞模式下，这个函数才有用
	*
	* 返回值：
	*		无
	*/
	void ME_OS_EXPORT Run();
}

ME_NAME_SPACE_END

#endif	// __MULTI_EVENT_H_



