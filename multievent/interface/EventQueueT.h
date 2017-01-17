/**
* �¼����еķ�װģ����
* 
*
* History
*	12/05/2016		Created
*/

#ifndef __EVENT_QUEUE_T_H_
#define __EVENT_QUEUE_T_H_

#include "Base.h"
#include "EventNodeT.h"
#include "NodeListT.h"
#include "LockGuardT.h"
#include "LockThreadMutex.h"
#include "CustomizeEvent.h"

#include "SingletonT.h"

ME_NAME_SPACE_BEGIN

/**
* �¼����еķ�װģ����
*/
template <class EventType, class LockType>
class CEventQueueT
{
public:
	typedef CEventNodeT<EventType> NodeType;
	typedef typename NodeType::CSyncEvent SyncEvent;

public:
	CEventQueueT();

	~CEventQueueT();

public:
	/**
	* Ͷ��һ���¼�
	* 
	* <pEvent>����ҪͶ�ݵ��¼�
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������Ϊʧ��
	*/
	ME_Result PostEvent( EventType* pEvent );

	/**
	* Ͷ��һ���¼�
	* 
	* <pEvent>����ҪͶ�ݵ��¼�
	* <nMode>���¼������ķ�ʽ
	* <pSyncEvent>��ͬ���¼�����nModeΪSENDʱ�����ֵ���벻ΪNULL
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������Ϊʧ��
	*/
	ME_Result PostEvent(
		EventType* pEvent,
		typename NodeType::MODE nMode,
		SyncEvent* pSyncEvent = NULL );

	/**
	* �ǲ�����Ҫ��ô��������û���ü�ϸ�룬�ȷ���
	*/
	ME_Result HandleEvent( BOOL& bExit );

	/**
	* �¼������ǲ���empty��
	*/
	BOOL IsEmpty();

	/**
	* ʹ�¼�����ֹͣ����
	*/
	void Destroy();

public:
	/**
	* ���ظ�ֵ������
	*
	* �����ڸ�ֵ�Ĺ����У�����ĳЩ����Ĭ�ϵĸ�ֵ��������Ϊ�����µ�
	* �쳣(����<m_Lock>������� Critical Setion���ͣ��ᵼ�����Ա
	* ����DebugInfo������һ��ͬ���Ǹ�ֵ��������CSʵ����DebugInfo
	* �ڴ��ַ��ͬһ���������յ���double delete)
	*/
	void operator = ( const CEventQueueT<EventType, LockType>& Dst );

private:
	typedef CNodeListT<NodeType> ListType;

private:
	ListType						m_nodeList;	// �¼��洢�ṹ
	LockType						m_Lock;		// ��
	BOOL							m_bExit;	// �¼������Ƿ����˳�״̬
};

typedef CEventQueueT<IMECustomizeEvent, CLockThreadMutex> CEventQueue;	// �¼�����

typedef CSingletonT<CEventQueue> CEventQueueSingleton;

#include "EventQueueT.hpp"

ME_NAME_SPACE_END

#endif	// __ME_EVENT_QUEUE_T_H_