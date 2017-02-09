/**
* ����������ʵ����
*
* Ŀǰֻ��linux�����������ã�win32������δʵ��
* 
*
* History
*	2/18/2016		Created
*/

#ifndef __LOCK_CONDITION_VARIABLE_H_
#define __LOCK_CONDITION_VARIABLE_H_

#include "Base.h"
#include "AtomicOperationT.h"

ME_NAME_SPACE_BEGIN

class CTimeValue;

/**
* ����������ʵ����
*
* Ŀǰֻ��linux�����������ã�win32������δʵ��
*/
class ME_OS_EXPORT CMELockConditionVariable
{
public:
	explicit CMELockConditionVariable( 
		const char* pszName = NULL,
		BOOL bActAsEvent = FALSE );

	~CMELockConditionVariable();

public:
	/**
	* ����������������
	*
	* �������������
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������ʧ��
	*/
	ME_Result Lock();

	/**
	* ���������������
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������ʧ��
	*/
	ME_Result UnLock();

	/**
	* ����һ���ź�
	*
	* ��������ᷢ��һ���źŲ��һ���һ���̲߳���������ź�
	* ���磬���������߳��ڵȴ���������������źţ����������
	* ��������һ����������������źţ������ѡ���һ������ϵͳ����
	*
	*/
	ME_Result Signal();

	/**
	* �㲥�ź�
	*
	* ����������Թ㲥����ʽ�������еȴ���������������߳�
	*/
	ME_Result Broadcast();

	/**
	* �ȴ���ǰ�������������ź�
	*
	* �������������
	*
	* <pTimerout>���ȴ��೤ʱ��
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ���ME_ERROR_TIMEOUTΪ��ʱ������ʧ��
	*/
	ME_Result Wait( CTimeValue* pTimeout = NULL );

private:
	void operator = ( const CMELockConditionVariable& );
	CMELockConditionVariable( const CMELockConditionVariable& );

private:
	ME_CONDITION_VARIABLE				m_ConditionVariable;
	ME_THREAD_MUTEX					m_Mutex;

	/**
	* ����������signal֮����wait����wait�������ģ���Ϊ�źŲ���洢������˵
	* signal����һ��״̬(״̬��û�б�����֮ǰӦ��һֱ����)����Ϊ��ʹ��������
	* �ܹ���win32��event�������� -- CMELockEvent����linux��������ʹ����������
	* ��ʵ�֣���ˣ���Ҫ��һ����������¼����������û��signal��broadcast����
	* �Ա��ڽ���ϱ���˵��wait��signal֮ǰ������Ӧ������
	*/
	CMEAtomicOperationT<CMELockMutex>	m_Signal;

	/**
	* ���ڱ�ʶ�Ƿ���Ҫ����������eventһ������
	*
	* ���<m_bActAsEvent>����FALSE����ô�����������ͱ�ɫ���� -- <m_Signal>���������κ�����
	* �������TRUE��<m_Signal>�����ã�������������win32��event�����Ĺ���
	*/
	BOOL								m_bActAsEvent;
};

ME_NAME_SPACE_END

#endif	// __LOCK_CONDITION_VARIABLE_H_


