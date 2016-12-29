/**
* �ź�����ʵ����
*
* Ŀǰֻ��linux�����������ã�win32������δʵ��
*
* History
*	2/18/2013		Created
*/
#ifndef __LOCK_SEMAPHORE_H_
#define __LOCK_SEMAPHORE_H_

#include "Base.h"
#include "OSSemaphore.h"

ME_NAME_SPACE_BEGIN

const DWORD ME_SEMAPHONE_NAME_MAX_LENGTH = 16;

/**
* �ź�����ʵ����
*/
class ME_OS_EXPORT CMELockSemaphore
{
public:
	explicit CMELockSemaphore(
		DWORD dwInitCount,
		DWORD dwMaxCount = OS::ME_SEMAPHORE_MAX_COUNT,
		const char* pszName = NULL );

	~CMELockSemaphore();

public:
	/** 
	* ��ȡ�źŵ�
	*
	* �������������
	*
	* ��������ή�͵�ǰ�źŵƿ��õ���Դ��
	*
	* ����ֵ:
	*		ME_OK�ǳɹ�; ��ME_OK��Ϊʧ��
	*/
	ME_Result Lock();

	/** 
	* �����źŵ�
	*
	* ������������ӵ�ǰ�źŵƿ��õ���Դ��
	*
	* ����ֵ:
	*		ME_OK�ǳɹ�; ��ME_OK��Ϊʧ��
	*/
	ME_Result UnLock();

	/** 
	* ���ٿ�����Դ��
	*
	* <dwCount>, ���ٶ��ٸ�������Դ��
	*
	* ����ֵ:
	*		ME_OK�ǳɹ�; ��ME_OK��Ϊʧ��
	*/
	ME_Result PostN( DWORD dwCount );

private:
	void operator = ( const CMELockSemaphore& );
	CMELockSemaphore( const CMELockSemaphore& );

private:
	ME_SEMAPHONE					m_Semaphore;
	char							m_szName[ME_SEMAPHONE_NAME_MAX_LENGTH];
};

ME_NAME_SPACE_END

#endif	// __LOCK_SEMAPHORE_H_


