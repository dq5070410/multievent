/**
* 用于退出时做对象回收、清理的一个类
* 更多是为了配合单例以而存在
=
*
* History
*	12/08/2016		Created
*/

#ifndef __ME_CLEAN_UP_BASE_H_
#define __ME_CLEAN_UP_BASE_H_

#include "Platform.h"
#include "UtilDefines.h"

ME_NAME_SPACE_BEGIN
class CLockThreadMutex;

/**
* 用于退出时做对象回收、清理的一个类
* 更多是为了配合单例以而存在
*/
class ME_OS_EXPORT CCleanupBase
{
protected:
	CCleanupBase();
	virtual ~CCleanupBase();

public:
	static void CleanupAll();

public:
	virtual void Cleanup();

protected:
	typedef CHikLockThreadMutex MutexType;

protected:
	static MutexType* GetCleanupMutex();

	static void ReleaseCleanupMutex();

private:
	void operator = ( const CCleanupBase& );
	CCleanupBase( const CCleanupBase& );

private:
	static CCleanupBase*			s_pHeader;	// 链表头
	static MutexType*				s_pMutex;	// 锁

private:
	CCleanupBase*				m_pNext;	// 指向下一个对象的指针
};

ME_NAME_SPACE_END

#endif	// __ME_CLEAN_UP_H_
















