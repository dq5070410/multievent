/**
* ʱ�Ӷ��нڵ�ķ�װ��
* 
*
* History
*	2/05/2016		Created
*/
#ifndef __TIME_NODE_T_H_
#define __TIME_NODE_T_H_

#include "Base.h"
#include "TimeValue.h"
#include "TimeTick.h"
#include "Timer.h"

NAME_SPACE_BEGIN

/**
* ʱ�Ӷ��нڵ�ķ�װ��
*/
template <class TimerType, class TimerSinkType>
class CTimerNodeT
{
public:
	typedef CTimerNodeT<TimerType, TimerSinkType> NodeType;
	typedef TimerType SuperType;

public:
	CTimerNodeT();

	/**
	* Ϊ�˷���CMENodeListT��Ҫ������
	*/
	CTimerNodeT( SuperType* pSuper );

	CTimerNodeT(
		TimerType* pTimer,
		TimerSinkType* pSink,
		const CTimeValue& htvInterval,
		const CTimeValue& htvDelay,
		int iLoopTime = 0 );

	~CTimerNodeT();

public:
	/**
	* ����ʱ�ӽڵ�
	*
	* <pTimer>��˭schedule�����ʱ��
	* <pSink>���ص�����(��ʱ�ӷ���ʱ֪ͨ��)
	* <httInterval>��ʱ�ӵļ��
	* <httDelay>���ӳٶ೤ʱ���ʼ
	* <iLoopTime>��ѭ�����ٴ�
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������Ϊʧ��
	*/
	ME_Result Set(
		TimerType* pTimer,
		TimerSinkType* pSink,
		const CTimeTick& httInterval,
		const CTimeTick& httDelay,
		int iLoopTime = 0 );

	/**
	* ����time tick
	*
	* <httTimeTick>������ʲô������time tick
	*
	* ����ֵ��
	*		��
	*/
	void SetTimeTick( const CTimeTick& httTimeTick );

	/**
	* ����time tick
	*
	* <dwRound>���ִ�
	* <dwTickCount>��tick count
	*
	* ����ֵ��
	*		��
	*/
	void SetTimeTick(
		DWORD dwRound,
		DWORD dwTickCount );

	/**
	* ���ٵ�ǰ�ڵ�һ�ִ�
	*
	* ����ֵ��
	*		��ǰ�ڵ��ִ���ִ�
	*/
	int ReduceLoopTime();

	/**
	* ��ȡ��ǰ�ڵ��time tick
	*
	* ����ֵ��
	*		CTimeTick
	*/
	const CTimeTick& GetTimeTick() const;

	/**
	* ��ȡ��ǰ�ڵ��time count
	*
	* ����ֵ��
	*		DWORD��Χ������ֵ
	*/
	DWORD GetTimeCount() const;

	/**
	* ��ȡ��ǰ�ڵ��ʱ�Ӽ��(tick count��Ҳ���Ǻ���ms)
	*
	* ����ֵ��
	*		DWORD��Χ������ֵ
	*/
	DWORD GetIntervalTimeCount() const;

	/**
	* Hit timer����timer�¼�����
	*/
	ME_Result Active();

public:
	BOOL operator == ( const NodeType& ntNode );
	
	BOOL operator == ( const SuperType& stSuper );

	BOOL operator > ( const NodeType& ntNode );

	BOOL operator < ( const NodeType& ntNode );

private:
	TimerType*						m_pTimer;			// ˭call��
	TimerSinkType*					m_pTimerSink;		// callback����

	CTimeTick					    m_httDelay;			// �Ӻ�೤ʱ�俪ʼ����tick
	CTimeTick					    m_httInterval;		// tick�ļ��
	int								m_iLoopTime;		// ѭ������
};

#include "TimerNodeT.hpp"

NAME_SPACE_END

#endif	// __TIME_NODE_T_H_










