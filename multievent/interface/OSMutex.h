/**
* mutex
*
* History
*	12/05/2016		Created
*/

#ifndef __OS_MUTEX_H_
#define __OS_MUTEX_H_

#include "Base.h"

ME_NAME_SPACE_BEGIN
class CTimeValue;
ME_NAME_SPACE_END

ME_NAME_SPACE_OS_BEGIN

/**
* 创建一个互斥量
*
* 这个方法会阻塞
*
* <iType>，什么类型的锁(CS还是mutex)
* <hmcLock>，out，成功之后为非NULL
* <pszName>，锁的名字
*
* 返回值：
*		ME_OK为成功；其他失败
*/
ME_Result ME_OS_EXPORT MutexCreate(
	INT iType,
	MUTEX_CS& hmcLock,
	LPSTR pszName = NULL );

/**
* 删除一个互斥量
*
* <hmcLock>，哪一个锁
* 这个方法会阻塞
*
* 返回值：
*		ME_OK为成功；R_ERROR_TIMEOUT为超时；其他失败
*/
ME_Result ME_OS_EXPORT MutexDestroy( MUTEX_CS& hmcLock );

/**
* 尝试获得一个互斥量
*
* <hmcLock>，哪一个锁
* 这个方法会阻塞
*
* 返回值：
*		ME_OK为成功；R_ERROR_TIMEOUT为超时；其他失败
*/
ME_Result ME_OS_EXPORT MutexLock( MUTEX_CS& hmcLock );

/**
* 尝试获得一个互斥量
*
* <hmcLock>，哪一个锁
* 这个方法会阻塞
*
* <htvTimeout>，最多尝试多长时间
*
* 返回值：
*		ME_OK为成功；R_ERROR_TIMEOUT为超时；其他失败
*/
ME_Result ME_OS_EXPORT MutexLock( 
	MUTEX_CS& hmcLock,
	const CTimeValue& htvTimeout );

/**
* 放弃一个互斥量(让其他线程可以获取)
*
* <hmcLock>，哪一个锁
*
* 返回值：
*		ME_OK为成功；其他失败
*/
ME_Result ME_OS_EXPORT MutexUnLock( MUTEX_CS& hmcLock );

ME_NAME_SPACE_OS_END

#endif	// __OS_MUTEX_H_
