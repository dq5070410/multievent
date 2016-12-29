/**
* �Զ�������
* 
*
* History
*	12/08/2016		Created
*/

#ifndef __GUARD_T_H_
#define __GUARD_T_H_

#include "HikBase.h"

ME_NAME_SPACE_BEGIN

/**
* �Զ�������
*
* ��ȷ������Ķ�����Lock()�Լ�UnLock()����
*/
template <class LockType>
class CMELockGuardT
{
public:
	CMELockGuardT( LockType* pLock );

	~CMELockGuardT();

private:
	ME_Result Lock();
	ME_Result UnLock();

private:
	LockType*						m_pLock;
};

#include "LockGuardT.hpp"

ME_NAME_SPACE_END

#endif	// __GUARD_T_H_








