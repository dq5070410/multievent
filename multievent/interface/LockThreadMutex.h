/**
* �����ڵ��߳���
* win����cs��linux����pthread_mutex_t
* 
* History
*	12/08/2012		Created
*/

#ifndef __LOCK_THREAD_MUTEX_H_
#define __LOCK_THREAD_MUTEX_H_

#include "Base.h"

ME_NAME_SPACE_BEGIN

/**
* �����ڵ��߳���
* win����cs��linux����pthread_mutex_t
*/
class ME_OS_EXPORT CMELockThreadMutex
{
public:
	CMELockThreadMutex();
	~CMELockThreadMutex();

public:
	/**
	* ���Ի�������
	*
	* �������������
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������ʧ��
	*/
	ME_Result Lock();

	/**
	* ���������(�������߳̿��Ի�ȡ)
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������ʧ��
	*/
	ME_Result UnLock();

private:
	ME_MUTEX_CS					m_ThreadMutex;

	/**
	* Ϊ�����Singleton::Instance( pObj )�������Ķ�
	* ���������Ҫ<pObj>���Ϳ��Ÿ�ֵ�뿽�����캯��
	*/
//private:
//	void operator = ( const CMELockThreadMutex& );
//	CMELockThreadMutex( const CMELockThreadMutex& );
};

ME_NAME_SPACE_END

#endif	// __LOCK_THREAD_MUTEX_H_

















