/**
* �ź�����ز�����API
* 
*
* History
*	2/18/2016		Created
*/

#ifndef __OS_SEMAPHORE_H_
#define __OS_SEMAPHORE_H_

#include "Base.h"

NAME_SPACE_OS_BEGIN

const DWORD ME_SEMAPHORE_MAX_COUNT = 0x7fffffff;
const DWORD ME_SEMAPHORE_FILE_MODE = 438;	// �ļ�����rw-r--r--

/** 
* ����һ���źŵ�
*
* <pSemaphore>��out���ɹ�֮��Ϊ��NULL
* <dwInitCount>����ʼ��ռ�õ���Դ��
* <dwMaxCount>���źŵƿ�����Դ����������ܳ���0x7fffffff
* <pszName>���źŵƵ�����
*
* ����ֵ:
*		ME_OK�ǳɹ�; ��ME_OK��Ϊʧ��
*/
ME_Result OS_EXPORT SemaphoreCreate( 
	ME_SEMAPHONE& pSemaphore,
	DWORD dwInitCount,
	DWORD dwMaxCount = ME_SEMAPHORE_MAX_COUNT,
	const char* pszName = NULL );

/** 
* ɾ��һ���źŵ�
*
* <pSemaphore>���ĸ��źŵ�
* <pszName>���źŵƵ�����
*
* ����ֵ:
*		ME_OK�ǳɹ�; ��ME_OK��Ϊʧ��
*/
ME_Result OS_EXPORT SemaphoreDestroy( 
	ME_SEMAPHONE& pSemaphore,
	const char* pszName = NULL );

/** 
* ��ȡһ���źŵ�
*
* �������������
*
* ��������ή�͵�ǰ�źŵƿ��õ���Դ��
*
* <pSemaphore>����һ���źŵ�
*
* ����ֵ:
*		ME_OK�ǳɹ�; ��ME_OK��Ϊʧ��
*/
ME_Result OS_EXPORT SemaphoreLock( ME_SEMAPHONE pSemaphore );

/** 
* ����һ���źŵ�
*
* ������������ӵ�ǰ�źŵƿ��õ���Դ��
*
* <pSemaphore>����һ���źŵ�
*
* ����ֵ:
*		ME_OK�ǳɹ�; ��ME_OK��Ϊʧ��
*/
ME_Result OS_EXPORT SemaphoreUnLock( ME_SEMAPHONE pSemaphore );

/** 
* ���ٿ�����Դ��
*
* <pSemaphore>����һ���źŵ�
* <dwCount>, ���ٶ��ٸ�������Դ��
*
* ����ֵ:
*		ME_OK�ǳɹ�; ��ME_OK��Ϊʧ��
*/
ME_Result OS_EXPORT SemaphorePostN( 
	ME_SEMAPHONE pSemaphore,
	DWORD dwCount );

NAME_SPACE_OS_END

#endif	// __OS_SEMAPHORE_H_

