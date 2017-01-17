/**
* CS
*
* History
*	12/05/2016		Created
*/

#ifndef __OS_THREAD_MUTEX_H_
#define __OS_THREAD_MUTEX_H_

#include "Base.h"

NAME_SPACE_OS_BEGIN

/**
* ����һ��thread mutex(win32����cs��linux����mutex)
*
* <htmLock>��out���ɹ�֮��Ϊ��NULL
*
* ����ֵ��
*		ME_OKΪ�ɹ�������ʧ��
*/
ME_Result ME_OS_EXPORT ThreadMutexCreate( ME_THREAD_MUTEX& htmLock );

/**
* ɾ��һ��thread mutex(win32����cs��linux����mutex)
*
* <htmLock>����һ��thread mutex
*
* ����ֵ��
*		ME_OKΪ�ɹ�������ʧ��
*/
ME_Result ME_OS_EXPORT ThreadMutexDestroy( ME_THREAD_MUTEX& htmLock );

/**
* ���Ի�������
*
* �������������
*
* <htmLock>����һ��thread mutex
*
* ����ֵ��
*		ME_OKΪ�ɹ�������ʧ��
*/
ME_Result ME_OS_EXPORT ThreadMutexLock( ME_THREAD_MUTEX& htmLock );

/**
* ���������(�������߳̿��Ի�ȡ)
*
* <htmLock>����һ��thread mutex
*
* ����ֵ��
*		ME_OKΪ�ɹ�������ʧ��
*/
ME_Result ME_OS_EXPORT ThreadMutexUnLock( ME_THREAD_MUTEX& htmLock );

NAME_SPACE_OS_END

#endif	// __OS_THREAD_MUTEX_H_
