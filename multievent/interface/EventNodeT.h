/**
* �¼����нڵ�ķ�װ��
* 
*
* History
*	2/05/2012		Created
*/
#ifndef __EVENT_NODE_T_H_
#define __EVENT_NODE_T_H_

#include "Base.h"
#include "LockEvent.h"

NAME_SPACE_BEGIN

/**
* �¼����нڵ�ķ�װ��
*/
template <class EventType>
class CEventNodeT
{
public:
	typedef CEventNodeT<EventType> NodeType;
	typedef EventType SuperType;

public:
	enum MODE
	{
		MODE_POST = 100,	// Post����ģʽ��������
		MODE_SEND,			// Send����ģʽ������
	};

	/**
	* �¼������ȼ�
	*
	* Ŀǰ�ݲ�ʹ�ã�Ԥ��
	*/
	enum PRIORITY
	{
		PRIORITY_LOW = 200,		// ������
		PRIORITY_NORMAL,		// ��ͨ
		PRIORITY_HIGH,			// ������
	};

	class CHikSyncEvent;

public:
	CEventNodeT();

	/**
	* Ϊ�˷���CHikNodeListT��Ҫ������
	*/
	CEventNodeT( SuperType* pSuper );

	/**
	* ͬʱ��Ϊ�˱���Ĭ�ϲ��������µı������޷�ʶ�𵽵���
	* �����ĸ����캯����ֻ�ܰ�ԭ����Ĭ�ϲ���ȥ��
	*/
	/*CHikEventNodeT(
		EventType* pEvent,
		MODE nMode = MODE_POST,
		CHikSyncEvent* pSync = NULL,
		PRIORITY nPriority = PRIORITY_NORMAL );*/

	CEventNodeT(
		EventType* pEvent,
		MODE nMode,
		CHikSyncEvent* pSync,
		PRIORITY nPriority = PRIORITY_NORMAL );

public:
	/**
	* ����Node�ڵ㱣���event
	*
	* <pEvent>����Ҫִ�е��¼�
	* <nMode>��������ʽ(POST����SEND)
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������Ϊʧ��
	*/
	ME_Result Set(
		EventType* pEvent,
		MODE nMode );

	/**
	* ����ͬ���¼�
	*
	* <pSync>��ͬ���¼�
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������Ϊʧ��
	*/
	ME_Result SetSyncEvent( CSyncEvent* pSync );

	/**
	* �����¼����ȼ�
	*
	* Ԥ���Ŀǰ�ݲ�ʹ��
	*
	* <nPriority>�����ȼ�
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������Ϊʧ��
	*/
	void SetPriority( PRIORITY nPriority );

	/**
	* ��ȡ�¼����ȼ�
	*
	* Ԥ���Ŀǰ�ݲ�ʹ��
	*
	* ����ֵ��
	*		200 ~ 202֮��
	*/
	UINT GetPriority() const;

	/**
	* ���¼�����
	*/
	ME_Result Active();

public:
	BOOL operator == ( const EventType* pEvent );

public:
	class CSyncEvent
	{
	public:
		void Signal( ME_Result hResult = ME_OK );

		/**
		* ��GetResult() const; ��ΪGetResult();
		* ��Ȼ�ͻ�һֱ��ʾ��error C2663: 'ME::CLockEvent::Wait' : 2 overloads have no legal conversion for 'this' pointer��
		*/
		ME_Result GetResult();

	private:
		CLockEvent			    	m_SyncEvent;		// ͬ���¼�
		ME_Result					m_hResult;			// �¼�ִ�н��(�ⲻ�ǡ�ͬ���¼���ִ�еĽ�������ǡ������ġ��Զ����¼�ִ�н��)
	};

private:
	EventType*						m_pEvent;			// �����Ҫִ�е��Զ����¼�
	MODE							m_nEventMode;		// ��ʶִ���¼��ķ���Send or Post

	CSyncEvent*					m_pSync;			// ���¼�ִ�з���ΪSendʱ�����������ͬ����������

	PRIORITY						m_nPriority;		// ���ȼ���������Ŀǰ����
};

#include "EventNodeT.hpp"

NAME_SPACE_END

#endif	// __EVENT_NODE_T_H_




