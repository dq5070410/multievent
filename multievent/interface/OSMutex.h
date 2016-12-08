/**
* mutex
*
* History
*	12/05/2012		Created
*/

#ifndef __OS_MUTEX_H_
#define __OS_MUTEX_H_

#include "Base.h"

NAME_SPACE_BEGIN
class CTimeValue;
NAME_SPACE_END

HNAME_SPACE_OS_BEGIN

/**
* ����һ��������
*
* �������������
*
* <iType>��ʲô���͵���(CS����mutex)
* <hmcLock>��out���ɹ�֮��Ϊ��NULL
* <pszName>����������
*
* ����ֵ��
*		R_OKΪ�ɹ�������ʧ��
*/
Result OS_EXPORT MutexCreate(
	INT iType,
	MUTEX_CS& hmcLock,
	LPSTR pszName = NULL );

/**
* ɾ��һ��������
*
* <hmcLock>����һ����
* �������������
*
* ����ֵ��
*		R_OKΪ�ɹ���R_ERROR_TIMEOUTΪ��ʱ������ʧ��
*/
Result OS_EXPORT MutexDestroy( MUTEX_CS& hmcLock );

/**
* ���Ի��һ��������
*
* <hmcLock>����һ����
* �������������
*
* ����ֵ��
*		R_OKΪ�ɹ���R_ERROR_TIMEOUTΪ��ʱ������ʧ��
*/
Result OS_EXPORT MutexLock( MUTEX_CS& hmcLock );

/**
* ���Ի��һ��������
*
* <hmcLock>����һ����
* �������������
*
* <htvTimeout>����ೢ�Զ೤ʱ��
*
* ����ֵ��
*		R_OKΪ�ɹ���R_ERROR_TIMEOUTΪ��ʱ������ʧ��
*/
Result OS_EXPORT MutexLock( 
	MUTEX_CS& hmcLock,
	const CTimeValue& htvTimeout );

/**
* ����һ��������(�������߳̿��Ի�ȡ)
*
* <hmcLock>����һ����
*
* ����ֵ��
*		R_OKΪ�ɹ�������ʧ��
*/
Result OS_EXPORT MutexUnLock( MUTEX_CS& hmcLock );

NAME_SPACE_OS_END

#endif	// __OS_MUTEX_H_
