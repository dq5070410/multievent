/**
* ������
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
* ������
*/
class ME_OS_EXPORT CMELockMutex
{
public:
	explicit CMELockMutex( LPSTR pszName = NULL );
	~CMELockMutex();

public:
	/**
	* ���Ի�����������
	*
	* �������������
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������ʧ��
	*/
	ME_Result Lock();

	/**
	* ���Ի�����������
	*
	* <htvTimeout>����ೢ�Զ೤ʱ��
	*
	* �������������
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ���ME_ERROR_TIMEOUTΪ��ʱ������ʧ��
	*/
	ME_Result Lock( const CTimeValue& htvTimeout );

	/**
	* �������������(�������߳̿��Ի�ȡ)
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������ʧ��
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










