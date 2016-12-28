/**
* 信号量相关操作的API
* 
*
* History
*	2/18/2016		Created
*/

#ifndef __OS_SEMAPHORE_H_
#define __OS_SEMAPHORE_H_

#include "Base.h"

NAME_SPACE_OS_BEGIN

const DWORD ME_SEMAPHORE_MAX_COUNT = 0x7fffffff;
const DWORD ME_SEMAPHORE_FILE_MODE = 438;	// 文件属性rw-r--r--

/** 
* 创建一个信号灯
*
* <pSemaphore>，out，成功之后为非NULL
* <dwInitCount>，初始被占用的资源数
* <dwMaxCount>，信号灯可用资源数量，最大不能超过0x7fffffff
* <pszName>，信号灯的名字
*
* 返回值:
*		ME_OK是成功; 非ME_OK皆为失败
*/
ME_Result OS_EXPORT SemaphoreCreate( 
	ME_SEMAPHONE& pSemaphore,
	DWORD dwInitCount,
	DWORD dwMaxCount = ME_SEMAPHORE_MAX_COUNT,
	const char* pszName = NULL );

/** 
* 删除一个信号灯
*
* <pSemaphore>，哪个信号灯
* <pszName>，信号灯的名字
*
* 返回值:
*		ME_OK是成功; 非ME_OK皆为失败
*/
ME_Result OS_EXPORT SemaphoreDestroy( 
	ME_SEMAPHONE& pSemaphore,
	const char* pszName = NULL );

/** 
* 获取一个信号灯
*
* 这个函数会阻塞
*
* 这个函数会降低当前信号灯可用的资源数
*
* <pSemaphore>，哪一个信号灯
*
* 返回值:
*		ME_OK是成功; 非ME_OK皆为失败
*/
ME_Result OS_EXPORT SemaphoreLock( ME_SEMAPHONE pSemaphore );

/** 
* 放弃一个信号灯
*
* 这个函数会增加当前信号灯可用的资源数
*
* <pSemaphore>，哪一个信号灯
*
* 返回值:
*		ME_OK是成功; 非ME_OK皆为失败
*/
ME_Result OS_EXPORT SemaphoreUnLock( ME_SEMAPHONE pSemaphore );

/** 
* 减少可用资源数
*
* <pSemaphore>，哪一个信号灯
* <dwCount>, 减少多少个可用资源数
*
* 返回值:
*		ME_OK是成功; 非ME_OK皆为失败
*/
ME_Result OS_EXPORT SemaphorePostN( 
	ME_SEMAPHONE pSemaphore,
	DWORD dwCount );

NAME_SPACE_OS_END

#endif	// __OS_SEMAPHORE_H_

