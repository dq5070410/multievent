/**
* CS
*
* History
*	12/05/2016		Created
*/

#ifndef __OS_THREAD_MUTEX_H_
#define __OS_THREAD_MUTEX_H_

#include "Base.h"

NAME_SPACE_OS_BEGIN

/**
* 创建一个thread mutex(win32下是cs，linux下是mutex)
*
* <htmLock>，out，成功之后为非NULL
*
* 返回值：
*		ME_OK为成功；其他失败
*/
ME_Result ME_OS_EXPORT ThreadMutexCreate( ME_THREAD_MUTEX& htmLock );

/**
* 删除一个thread mutex(win32下是cs，linux下是mutex)
*
* <htmLock>，哪一个thread mutex
*
* 返回值：
*		ME_OK为成功；其他失败
*/
ME_Result ME_OS_EXPORT ThreadMutexDestroy( ME_THREAD_MUTEX& htmLock );

/**
* 尝试获得这把锁
*
* 这个方法会阻塞
*
* <htmLock>，哪一个thread mutex
*
* 返回值：
*		ME_OK为成功；其他失败
*/
ME_Result ME_OS_EXPORT ThreadMutexLock( ME_THREAD_MUTEX& htmLock );

/**
* 放弃这把锁(让其他线程可以获取)
*
* <htmLock>，哪一个thread mutex
*
* 返回值：
*		ME_OK为成功；其他失败
*/
ME_Result ME_OS_EXPORT ThreadMutexUnLock( ME_THREAD_MUTEX& htmLock );

NAME_SPACE_OS_END

#endif	// __OS_THREAD_MUTEX_H_
