/**
* 互斥量
* 
*
* History
*	12/08/2016		Created
*/

#ifndef __LOCK_MUTEX_H_
#define __LOCK_MUTEX_H_

#include "Base.h"

ME_NAME_SPACE_BEGIN

class CTimeValue;

/**
* 互斥量
*/
class ME_OS_EXPORT CMELockMutex
{
public:
	explicit CMELockMutex( LPSTR pszName = NULL );
	~CMELockMutex();

public:
	/**
	* 尝试获得这个互斥量
	*
	* 这个方法会阻塞
	*
	* 返回值：
	*		ME_OK为成功；其他失败
	*/
	ME_Result Lock();

	/**
	* 尝试获得这个互斥量
	*
	* <htvTimeout>，最多尝试多长时间
	*
	* 这个方法会阻塞
	*
	* 返回值：
	*		ME_OK为成功；ME_ERROR_TIMEOUT为超时；其他失败
	*/
	ME_Result Lock( const CTimeValue& htvTimeout );

	/**
	* 放弃这个互斥量(让其他线程可以获取)
	*
	* 返回值：
	*		ME_OK为成功；其他失败
	*/
	ME_Result UnLock();

private:
	void operator = ( const CMELockMutex& );
	CMELockMutex( const CMELockMutex& );

private:
	ME_MUTEX_CS					m_Mutex;
};

ME_NAME_SPACE_END

#endif	// __LOCK_MUTEX_H_










