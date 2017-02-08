/**
* 线程相关API
* 
*
* History
*	12/05/2016		Created
*/

#ifndef __OS_THREAD_H_
#define __OS_THREAD_H_

#include "Base.h"

ME_NAME_SPACE_BEGIN
class CTimeValue;
ME_NAME_SPACE_END

ME_NAME_SPACE_OS_BEGIN

ME_Result ME_OS_EXPORT ThreadCreate(
	ME_THREAD_FUNC pFunc,
	LPVOID pArgs,
	UINT lFlags,
	ME_THREAD_ID& htiThreadID,
	ME_THREAD_HANDLE* hThreadHandle = 0,
	LONG lPriority = 0,
	LPVOID pStack = NULL,
	DWORD dwStacksize = 0 );

/**
* win32环境：立即结束指定线程
* linux环境：把线程置为cancel状态(不会立即结束线程而直到下一个cancel点)
*
* 在win32环境中调用这个方法，你必须确定你知道要干什么以及这个方法可能会引起
* 的一些不可预知的后果(比如内存泄漏，句柄泄漏)，如果你不确定，那么不要调用
* 这个方法
*
* 在linux环境中，在可能导致同样的问题的同时，还有一个需要注意的是：如果线程
* 函数处于循环或者之类的情况下，可能会导致线程迟迟无法退出，因为，cancel点
* 无法到达，为了避免这个问题，你需要在你的循环里加上ThreadTestDestroy，以确保
* 在线程能够顺利的到达cancel点
*/
ME_Result ME_OS_EXPORT ThreadDestroy(
	ME_THREAD_HANDLE hThreadHandle, 
	DWORD dwExitCode );

/**
* 设置一个cancel点
*
* win32环境下，这个函数不做任何处理
* linux环境下，会设置一cancellation point，这样当线程处于cancel状态时，
* 它就能够顺利的退出(当然，也还是要花上一点时间)
*/
ME_Result ME_OS_EXPORT ThreadTestDestroy();

ME_Result ME_OS_EXPORT ThreadJoin( 
	ME_THREAD_HANDLE hThreadHandle,
	CTimeValue* pTimeout = NULL,
	ME_THREAD_STAT* pStat = NULL );

ME_Result ME_OS_EXPORT ThreadSetPriority(
	ME_THREAD_HANDLE hThreadHandle,
	int iPriority );

/**
* 获取当前线程的ID
*/
ME_THREAD_ID ME_OS_EXPORT GetThreadID();

/**
* 获得当前线程的句柄
*
* 在linux环境下，ThreadHandle()与ThreadID()的结果是一样的
* 因为linux没有句柄一说
*
* 在win32环境下，则是不同
*/
ME_THREAD_HANDLE ME_OS_EXPORT GetThreadHandle();

ME_NAME_SPACE_OS_END

#endif	// __ME_OS_THREAD_H_



