/**
* 定时检测连接是否正常
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
 * 定时器类，定时调用注册了的TransportOperatorKeepAlive的TimerCheck方法
 */
class CMETransportKeepAliveTimer 
	: public CMETimer
    , public IMETimerSink
{
public:
	/**
	 * 构造方法，创建一个线程，并且将本身作为定时器添加到新建的线程里
	 * 不注册到网络线程的原因是如果网络线程要处理的事件比较多，则定时器会严重的不准确
	 */
    CMETransportKeepAliveTimer( );

	virtual ~CMETransportKeepAliveTimer();

public:
	/**
	 * 注册一个KeepAliveOperator到该Timer里
	 */
	ME_Result Register( 
		IMETransportTimerOperator* pOperator, 
		INT iInterval );

	/**
	 * 从Timer里取消一个KeepAliveOperator
	 */
	ME_Result UnRegister( IMETransportTimerOperator* pOperator );

	ME_Result Update( 
		IMETransportTimerOperator* pOperator, 
		INT iInterval );

    /* 继承自IMETimerSink */
public:
    virtual VOID OnTimer( CMETimer* pTimerID );

private:
    CHAR* SelfInfo();

	VOID GenInterval();

private:
	typedef std::map<IMETransportTimerOperator*, INT> OperatorMapType;

private:
	/* 需要被检查的连接 */
	OperatorMapType					m_mapTimerOperator;

	/* 定时器间隔及延误启动时间 */
	INT								m_iHBInterval;

	ULONGLONG						m_iCheckNum;

private:
	/* 单元测试友元类 */
	friend class TestTOKA;

};

typedef CMESingletonT<CMETransportKeepAliveTimer> CMETransportKeepAliveTimerSingleton;

ME_NAME_SPACE_END

#endif	// __TRANSPORT_KEEPALIVE_TIMER_H_
