/**
* ʱ�Ӷ��еķ�װģ����
* 
*
* History
*	2/05/2016		Created
*/
#ifndef __TIME_QUEUE_T_H_
#define __TIME_QUEUE_T_H_

#include "Base.h"
#include "TimerNodeT.h"
#include "NodeListT.h"
#include "LockGuardT.h"
#include "Timer.h"
#include "LockThreadMutex.h"
#include "SingletonT.h"

ME_NAME_SPACE_BEGIN

/**
* ʱ�Ӷ��еķ�װģ����
*/
template <class TimerType, class TimerSinkType, class LockType>
class CMETimerQueueT
{
public:
	typedef CTimerNodeT<TimerType, TimerSinkType> NodeType;

public:
	enum
	{
		MINIMUM_TIMER_INTERVAL = 50, // ��Сʱ�Ӽ������ME_TIME_LIMIT������ʱ���ǿ�Ƽ��
	};

public:
	CMETimerQueueT();

	~CMETimerQueueT();

public:
	/**
	* ����һ��timer
	* 
	* ����㷴����scheduleͬһ��ʱ�ӣ�ֻ�����һ�ε�schedule�������õ�
	*
	* <pTimer>��˭���������ʱ��
	* <pSink>��ʱ�ӻص����󣬱���Ҫ��
	* <htvInterval>��������೤ʱ����һ�Σ��������ٴ�ȡ����<iLoopTime>
	* <htvDelay>���ӳ٣������ʱ��������೤ʱ�䷢��
	* <iLoopTime>��һ���������ٴΣ�ע�⣬���ӳ١���һ���ǲ��������ڵģ��������
	*				���timer�������ϵķ�������ô-1��������Ҫ�����ֵ
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�������Ϊʧ��
	*/
	ME_Result Schedule( 
		TimerType* pTimer,
		TimerSinkType* pSink,
		const CTimeValue& htvInterval,
		const CTimeValue& htvDelay,
		int iLoopTime = 0 );

	/**
	* ȡ��ʱ��
	*
	* <pTimer>��ȡ���ĸ�ʱ��
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�������Ϊʧ��
	*/
	ME_Result Cancel( TimerType* pTimer );

	/**
	* ��ȡ���緢����һ��ʱ�ӵ�tick count(Ҳ���Ǻ���)
	*
	* ����ֵ��
	*		UINT��Χ�ڵ�����ֵ����ʱ�Ӷ���Ϊ��ʱ����-1
	*/
	UINT GetNextTimeout();

	/**
	* ����ĸ�ʱ�ӵ����˲�������
	*
	* ����ֵ��
	*		���α������ʱ�Ӹ���(UINT��Χ�ڵ�����ֵ)
	*/
	UINT Expire();

	/**
	* ʹʱ�Ӷ���ֹͣ����
	*
	* ����ֵ��
	*		��
	*/
	void Destroy();

private:
	/**
	* ����ʱ�ӽڵ�
	*
	* <pNode>����Ҫ���õ�ʱ�ӽڵ�
	* <pTimer>��˭���������ʱ��
	* <pSink>��ʱ�ӻص����󣬱���Ҫ��
	* <htvInterval>��������೤ʱ����һ�Σ��������ٴ�ȡ����<iLoopTime>
	* <htvDelay>���ӳ٣������ʱ��������೤ʱ�䷢��
	* <iLoopTime>��һ���������ٴΣ�ע�⣬���ӳ١���һ���ǲ��������ڵģ��������
	*				���timer�������ϵķ�������ô-1��������Ҫ�����ֵ
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������Ϊʧ��
	*/
	ME_Result SetupNode(
		NodeType* pNode,
		TimerType* pTimer,
		TimerSinkType* pSink,
		const CTimeValue& htvInterval,
		const CTimeValue& htvDelay,
		int iLoopTime = 0 );

	/**
	* ���±�׼ʱ��
	*
	* �ѵ�ǰʱ�̵�ϵͳtickcount�洢����
	*
	* ����ֵ��
	*		��
	*/
	void UpdateStandardTick();

	/**
	* ���ݱ�׼ʱ�Ӹ��½ڵ�
	*
	* <pNode>���ĸ��ڵ�
	*
	* ����ֵ��
	*		��
	*/
	void UpdateNode( NodeType* pNode );

	/**
	* ���ݱ�׼ʱ�ӣ��Լ�<dwTickStep>��У׼�����ѽ��
	* ����<httTick>
	*
	* <httTick>��out
	* <dwTickStep>����Ҫ��������tick count
	*/
	void AdjustTick(
		CTimeTick& httTick,
		DWORD dwTickStep );

public:
	/**
	* ���ظ�ֵ������
	*
	* �����ڸ�ֵ�Ĺ����У�����ĳЩ����Ĭ�ϵĸ�ֵ��������Ϊ�����µ�
	* �쳣(����<m_Lock>������� Critical Setion���ͣ��ᵼ�����Ա
	* ����DebugInfo������һ��ͬ���Ǹ�ֵ��������CSʵ����DebugInfo
	* �ڴ��ַ��ͬһ���������յ���double delete)
	*/
	void operator = ( const CMETimerQueueT<TimerType, TimerSinkType, LockType>& Dst );

private:
	/**
	* ��������ķº���(ʹ��С���㷨)
	*/
	template <class TimerNodeType>
	class CTimerNodeLesser
	{
	public:
		BOOL operator () ( const TimerNodeType& pSrc, const TimerNodeType& pDst );
	};

	typedef CTimerNodeLesser<NodeType*> SorterType;

private:
	LockType						m_Lock;			// ��
	BOOL							m_bExit;		// ʱ�Ӷ����Ƿ����˳�״̬

	/**
	* ���ڼ�¼ϵͳ��tickcount
	*
	* ����tickcount��һ��DWORDֵ��DOWRD�����ֵ�����������Ҳ���ǲ��49.7��
	* ����ʹ���һ���߽磬Ҳ�����ڿ쵽49.7���ʱ��scheduleһ��timer�Ļ�������
	* �����ڵڶ��ֵ�49.7����Żᱻִ�У����仰��˵��Ҳ���Ǳ�ִ��ʱ��tickcount
	* �п��ܻ������schedule��ʱ��С�����ԣ�������<m_httStandard>����¼tickcount��
	* �Ա����ڷ����������ʱ���ܹ�ͨ��<m_httStandard>������(dwRound)��ʶ��
	*/
	CTimeTick					m_httStandard;
	LockType						m_LockStandard;	// ��һ��������������<m_httStandard>

private:
	typedef CMENodeListT<NodeType, SorterType> ListType;
	ListType						m_nodeList;		// �ڵ�����
};

typedef CMETimerQueueT<CMETimer, ITimerSink, CMELockThreadMutex> CMETimerQueue;	// ʱ�Ӷ���

typedef CMESingletonT<CMETimerQueue> CMETimerQueueSingleton;

#include "TimerQueueT.hpp"

ME_NAME_SPACE_END


#endif	// __TIME_QUEUE_T_H_





