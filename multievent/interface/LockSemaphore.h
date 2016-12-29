/**
* 信号量的实现类
*
* 目前只在linux环境下起作用，win32环境暂未实现
*
* History
*	2/18/2013		Created
*/
#ifndef __LOCK_SEMAPHORE_H_
#define __LOCK_SEMAPHORE_H_

#include "Base.h"
#include "OSSemaphore.h"

ME_NAME_SPACE_BEGIN

const DWORD ME_SEMAPHONE_NAME_MAX_LENGTH = 16;

/**
* 信号量的实现类
*/
class ME_OS_EXPORT CMELockSemaphore
{
public:
	explicit CMELockSemaphore(
		DWORD dwInitCount,
		DWORD dwMaxCount = OS::ME_SEMAPHORE_MAX_COUNT,
		const char* pszName = NULL );

	~CMELockSemaphore();

public:
	/** 
	* 获取信号灯
	*
	* 这个函数会阻塞
	*
	* 这个函数会降低当前信号灯可用的资源数
	*
	* 返回值:
	*		ME_OK是成功; 非ME_OK皆为失败
	*/
	ME_Result Lock();

	/** 
	* 放弃信号灯
	*
	* 这个函数会增加当前信号灯可用的资源数
	*
	* 返回值:
	*		ME_OK是成功; 非ME_OK皆为失败
	*/
	ME_Result UnLock();

	/** 
	* 减少可用资源数
	*
	* <dwCount>, 减少多少个可用资源数
	*
	* 返回值:
	*		ME_OK是成功; 非ME_OK皆为失败
	*/
	ME_Result PostN( DWORD dwCount );

private:
	void operator = ( const CMELockSemaphore& );
	CMELockSemaphore( const CMELockSemaphore& );

private:
	ME_SEMAPHONE					m_Semaphore;
	char							m_szName[ME_SEMAPHONE_NAME_MAX_LENGTH];
};

ME_NAME_SPACE_END

#endif	// __LOCK_SEMAPHORE_H_


