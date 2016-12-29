/**
* 进程内的线程锁
* win下是cs，linux下是pthread_mutex_t
* 
* History
*	12/08/2012		Created
*/

#ifndef __LOCK_THREAD_MUTEX_H_
#define __LOCK_THREAD_MUTEX_H_

#include "Base.h"

ME_NAME_SPACE_BEGIN

/**
* 进程内的线程锁
* win下是cs，linux下是pthread_mutex_t
*/
class ME_OS_EXPORT CMELockThreadMutex
{
public:
	CMELockThreadMutex();
	~CMELockThreadMutex();

public:
	/**
	* 尝试获得这把锁
	*
	* 这个方法会阻塞
	*
	* 返回值：
	*		ME_OK为成功；其他失败
	*/
	ME_Result Lock();

	/**
	* 放弃这把锁(让其他线程可以获取)
	*
	* 返回值：
	*		ME_OK为成功；其他失败
	*/
	ME_Result UnLock();

private:
	ME_MUTEX_CS					m_ThreadMutex;

	/**
	* 为了配合Singleton::Instance( pObj )函数而改动
	* 这个函数需要<pObj>类型开放赋值与拷贝构造函数
	*/
//private:
//	void operator = ( const CMELockThreadMutex& );
//	CMELockThreadMutex( const CMELockThreadMutex& );
};

ME_NAME_SPACE_END

#endif	// __LOCK_THREAD_MUTEX_H_

















