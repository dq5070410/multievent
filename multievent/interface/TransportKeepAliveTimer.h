/**
* ��ʱ��������Ƿ�����
* 
*
* History
*	3/12/2016		Created
*/

#ifndef __TRANSPORT_KEEPALIVE_TIMER_H_
#define __TRANSPORT_KEEPALIVE_TIMER_H_

#include "Base.h"
#include "ConnectionDefines.h"
#include "Timer.h"
#include "TimeValue.h"
#include "SingletonT.h"

ME_NAME_SPACE_BEGIN

class IMETransportTimerOperator;
class CMEThread;

/**
 * ��ʱ���࣬��ʱ����ע���˵�TransportOperatorKeepAlive��TimerCheck����
 */
class CMETransportKeepAliveTimer 
	: public CMETimer
    , public IMETimerSink
{
public:
	/**
	 * ���췽��������һ���̣߳����ҽ�������Ϊ��ʱ����ӵ��½����߳���
	 * ��ע�ᵽ�����̵߳�ԭ������������߳�Ҫ������¼��Ƚ϶࣬��ʱ�������صĲ�׼ȷ
	 */
    CMETransportKeepAliveTimer( );

	virtual ~CMETransportKeepAliveTimer();

public:
	/**
	 * ע��һ��KeepAliveOperator����Timer��
	 */
	ME_Result Register( 
		IMETransportTimerOperator* pOperator, 
		INT iInterval );

	/**
	 * ��Timer��ȡ��һ��KeepAliveOperator
	 */
	ME_Result UnRegister( IMETransportTimerOperator* pOperator );

	ME_Result Update( 
		IMETransportTimerOperator* pOperator, 
		INT iInterval );

    /* �̳���IMETimerSink */
public:
    virtual VOID OnTimer( CMETimer* pTimerID );

private:
    CHAR* SelfInfo();

	VOID GenInterval();

private:
	typedef std::map<IMETransportTimerOperator*, INT> OperatorMapType;

private:
	/* ��Ҫ���������� */
	OperatorMapType					m_mapTimerOperator;

	/* ��ʱ���������������ʱ�� */
	INT								m_iHBInterval;

	ULONGLONG						m_iCheckNum;

private:
	/* ��Ԫ������Ԫ�� */
	friend class TestTOKA;

};

typedef CMESingletonT<CMETransportKeepAliveTimer> CMETransportKeepAliveTimerSingleton;

ME_NAME_SPACE_END

#endif	// __TRANSPORT_KEEPALIVE_TIMER_H_
