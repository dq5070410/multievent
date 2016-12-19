/**
* �¼�(��)��ʵ����
*
* ��win32�����£��������¼�(event)����linux��������
* ��������ģ��ʵ��
* 
*
* History
*	2/18/2013		Created
*/

#ifndef __LOCK_EVENT_H_
#define __LOCK_EVENT_H_

#include "Base.h"

#ifndef WIN
#include "HikLockConditionVariable.h"
#endif	// WIN32

NAME_SPACE_BEGIN

class CTimeValue;

/**
* �¼�(��)��ʵ����
*
* ��win32�����£��������¼�(event)����linux��������
* ��������ģ��ʵ��
*/
class OS_EXPORT CLockEvent
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
	CHikLockEvent( const CLockEvent& );

private:
#ifdef WIN
	ME_EVENT						m_pEvent;
#else
	CLockConditionVariable		m_pEvent;
#endif	// WIN
};

NAME_SPACE_END

#endif	// __LOCK_EVENT_H_

