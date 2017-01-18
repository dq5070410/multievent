/**
* 用作TCP自动检测该连接是否还是可用处理类
* 
* HikTransportOperatorKeepAlive.h
*
* Hikvision System Technology Co., Ltd.
* All rights reserved
* 
* Author
*	王璟			(wangjingyf@hikvision.com)
*
* History
*	3/7/2013		Created
*/

#ifndef __ME_TRANSPORT_OPERATOR_KEEPALIVE_H_
#define __ME_TRANSPORT_OPERATOR_KEEPALIVE_H_

#include "HikBase.h"
#include "HikTransportOperatorBase.h"
#include "HikConnectionDefines.h"
#include "HikTimeValue.h"
#include "HikTimer.h"
#include "HikEventTransportT.h"
#include "HikPDU.h"

ME_NAME_SPACE_BEGIN

class IMETransport;
class CMETransportKeepAliveTimer;

class IMETransportTimerOperator
{
public:
	virtual VOID TimerCheck() = 0;

};

/**
* 检查连接是否可用类  
*/
class CMETransportOperatorKeepAlive 
    : public CMETransportOperatorBase	
	, public IMETransportTimerOperator
{
public:
	CMETransportOperatorKeepAlive( IMETransport* pTransport );

	virtual ~CMETransportOperatorKeepAlive();

    /* 继承自CMETransportOperatorBase */
public:
	ME_Result PreSendData( CMEMessageBlock*& pmbBlock );

    /**
    * 该条连接是否在发数据（以及该次的发送结果）以判断是否为空闲状态
	*/
	ME_Result PostSendData( CMEMessageBlock*& pmbBlock );

    /**
    * 判断收到的数据是否是心跳请求包，如果是则通过设置返回值控制不被回调给用户层
	* 无论收到什么样的包都更新“活跃时间”
	*/
	ME_Result PreOnReceive( CMEMessageBlock*& pmbBlock );

	DWORD GetType();

	/**
	* 可设定OPTION_TYPE_SET_KEEPALIVE_CHECKNUM和OPTION_TYPE_SET_KEEPALIVE_INTERVEL两个选项
	*/
	ME_Result SetOption( DWORD dwOptionType, VOID* pOptionValue );

	/* 继承自ITimerSink */
public:
	/**
	* 定时检查该条连接状态。
	* 如果该连接忙，则不发送心跳请求包
	* 如果超过了设定的最大尝试次数，则认为该条连接超时（不可用）。 调用用户的Ondisconnect回调
	*/
	//virtual VOID OnTimer( CMETimer* pTimerID );
	virtual VOID TimerCheck();

private:
	enum KEEPALIVE_TYPE
	{
		KEEPALIVE_TYPE_REQ = 0,
		KEEPALIVE_TYPE_RSP,
		KEEPALIVE_TYPE_COMMON,
	};

    BOOL IsTimeOut();

	ME_Result HandleTimerOut();

	BOOL Idle();

	VOID SendHBMessage( KEEPALIVE_TYPE iType );

	ME_Result SetInterval( UINT nInterval );

	ME_Result SetCheckNum( UINT nCheckNum );

	ME_Result AddKeepAliveHead( 
		CMEMessageBlock& mbSend, 
		KEEPALIVE_TYPE iType );

	CHAR* SelfInfo();

private:
	/**
	* 设定Option时需要抛到网络线程设定
	*/
	typedef EVENT::EventSheduleTimer<CMETransportKeepAliveTimer, IMETransportTimerOperator> KeepAliveSheduleTimer;
	typedef EVENT::EventCloseRawT<IMETransport> EventCloseWrapper;

private:

private:
	/* 所监视的Transport */
    IMETransport*                  m_pTransport;

	/* 最近活跃时间，这个字段是最后一次收到或发送包成功的时间 */
	CMETimeValue					m_tvActiveTime;

	BOOL							m_bLastSendOK;

	/**
	* 最大检查次数 
	* 可通过OPTION_TYPE_SET_KEEPALIVE_CHECKNUM选项设定
	*/
	UINT							m_nMaxCheckNum;

	/* 还有多少次检查连接的机会 */
	UINT							m_nCheckNum;

	KEEPALIVE_TYPE					m_iHeadType;

	BOOL							m_bNewPackage;

#ifdef ME_DEBUG
private:
	/* 单元测试友元类 */
	friend class TestTOKA;
#endif

};

ME_NAME_SPACE_END

#endif	// __ME_TRANSPORT_OPERATOR_KEEPALIVE_H_
