/**
* �¼�(��)��ʵ����
*
* ��win32�����£��������¼�(event)����linux��������
* ��������ģ��ʵ��
* 
*
* History
*	12/18/2016		Created
*/

#ifndef __LOCK_EVENT_H_
#define __LOCK_EVENT_H_

#include "Base.h"

#ifndef ME_WIN
#include "LockConditionVariable.h"
#endif	// WIN32

ME_NAME_SPACE_BEGIN

class CTimeValue;

/**
* �¼�(��)��ʵ����
*
* ��win32�����£��������¼�(event)����linux��������
* ��������ģ��ʵ��
*/
class ME_OS_EXPORT CLockEvent
{
public:
	explicit CLockEvent(
		BOOL bManualReset = FALSE,
		BOOL bSignaled = FALSE,
		const char* pszName = NULL );

	~CLockEvent();

public:
	/**
	* �����¼�����(�����Signal����Ч����ͬ)
	*
	* �������������
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������ʧ��
	*/
	ME_Result Lock();

	/**
	* ������ǰ�¼�(�����Reset����Ч����ͬ)
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������ʧ��
	*/
	ME_Result UnLock();

	/**
	* ���¼���Ϊ�����źš���״̬
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������ʧ��
	*/
	ME_Result Signal();

	/**
	* ���¼���Ϊ�����źš���״̬
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������ʧ��
	*/
	ME_Result Reset();

	/**
	* �ȴ�����¼�����Ϊ�����źš���״̬
	*
	* �������������
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������ʧ��
	*/
	ME_Result Wait();

	/**
	* �ȴ�����¼�����Ϊ�����źš���״̬
	*
	* �������������
	*
	* <pTimeout>���ȴ��೤ʱ��
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ���ME_ERROR_TIMEOUTΪ��ʱ������ʧ��
	*/
	ME_Result Wait( CTimeValue* pTimeout );

private:
	void operator = ( const CLockEvent& );
	CMELockEvent( const CLockEvent& );

private:
#ifdef WIN
	ME_EVENT						m_pEvent;
#else
	CLockConditionVariable		m_pEvent;
#endif	// WIN
};

ME_NAME_SPACE_END

#endif	// __LOCK_EVENT_H_


