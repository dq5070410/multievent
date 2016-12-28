/**
* �����˳�ʱ��������ա������һ����
* ������Ϊ����ϵ����Զ�����
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
* �����˳�ʱ��������ա������һ����
* ������Ϊ����ϵ����Զ�����
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
	static CCleanupBase*			s_pHeader;	// ����ͷ
	static MutexType*				s_pMutex;	// ��

private:
	CCleanupBase*				m_pNext;	// ָ����һ�������ָ��
};

ME_NAME_SPACE_END

#endif	// __ME_CLEAN_UP_H_
















