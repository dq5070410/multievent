/**
* ����ȷ�����߳�ɾ����ȫ����
*
* ͨ���ڶ��󱻴����������߳��Լ������߳�֮��һ��3��
* �¼�����, ȷ�������߳�����Ҫcallback���ö�����¼�
* �����, ����һ�λص����������߳���֮��, ��ɾ������
*
*
* History
*	3/1/2016		Created
*/

#ifndef __MT_SAFE_DESTROY_H_
#define __MT_SAFE_DESTROY_H_

#include "Base.h"
#include "CustomizeEvent.h"
#include "LockEvent.h"

ME_NAME_SPACE_BEGIN

/**
* ����ȷ�����߳�ɾ����ȫ����
*
* ͨ���ڶ��󱻴����������߳��Լ������߳�֮��һ��3��
* �¼�����, ȷ�������߳�����Ҫcallback���ö�����¼�
* �����, ����һ�λص����������߳���֮��, ��ɾ������
*/
class CMEMTSafeDestroy : public ICustomizeEvent
{
public:
	/* ����ɾ��״̬ */
	typedef DWORD DESTROY_STATUS;
	enum
	{
		DESTROY_STATUS_NONE,

		/* �ڵ�ǰ�̳߳���ɾ�� */
		DESTROY_STATUS_CALL_IN_CURRENT_THREAD,

		/* �������̳߳���ɾ�� */
		DESTROY_STATUS_CALL_IN_NETWORK_THREAD,

		/* �ص���ǰ�̳߳���������ɾ�� */
		DESTROY_STATUS_BACK_TO_CURRENT_THREAD,
	};

public:
	CMEMTSafeDestroy();

	virtual ~CMEMTSafeDestroy();

	/* �̳���IMECustomizeEvent */
public:
	/**
	* ���¼�����ʱ������ص���֪ͨ��
	*
	* �¼���ͬ���첽֮�֣���ͬ��������£����������Ҫ֪��
	* �¼�ִ�н������ôһ����������Ը�ԭ��Ӧ����callback
	* �ĺ��������˷���ֵ
	*/
	virtual ME_Result OnEvent();

	/**
	* ��OnEvent() callback֮��OnDestroy()�����Żᱻ
	* ���ã�������������¼������������Ĭ����Ϊ������
	* delete this�������������Ҫ�󣬿��������������
	*
	* �ӹ����������������������һ���ص��������ں�������
	* ��On�����ǣ����ǵ���OnEvent���̣��ͼ�����
	*
	* �¼���ͬ���첽֮�֣���ͬ��������£����������Ҫ֪��
	* �¼�ִ�н������ôһ����������Ը�ԭ��Ӧ����callback
	* �ĺ��������˷���ֵ
	*/
	virtual ME_Result OnDestroy();

protected:

	/*
	* �ڱ������̳߳���ɾ������
	* ���ݵ�ǰ״̬��ͬ, ����Ҳ��ͬ
	* ��״̬����DESTROY_STATUS_CALL_IN_CURRENT_THREADʱ,
	* ����ֻ���������߳�Ͷ��һ���¼�, ȷ���������߳���,
	* �й��ڸö�����¼�������ִ��ɾ��֮ǰ, �����;
	* ��״̬����DESTROY_STATUS_BACK_TO_CURRENT_THREADʱ,
	* ִ��ɾ������
	*
	* ����ֵ:
	*		��
	*/
	virtual void Destroy_u();

	/*
	* �������̳߳���ɾ������
	* �������������������ɾ������, ֻ������󱻴������߳�
	* Ͷ��һ���¼�, ֪ͨ����, �������������̵߳��¼��Ѿ�
	* �Ѿ������, ����ɾ��������
	*
	* ����ֵ:
	*		��
	*/
	virtual void Destroy_n();

protected:
	DESTROY_STATUS					m_dwStatus;		// ��ǰ״̬
	CLockEvent					    m_Event;		// ����ȷ�������߳��������֮����delete
};

ME_NAME_SPACE_END

#endif	// __MT_SAFE_DESTROY_H_


