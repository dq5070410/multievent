/**
* ����TCP�Զ����������Ƿ��ǿ��ô�����
* 
* HikTransportOperatorKeepAlive.h
*
* Hikvision System Technology Co., Ltd.
* All rights reserved
* 
* Author
*	���Z			(wangjingyf@hikvision.com)
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
* ��������Ƿ������  
*/
class CMETransportOperatorKeepAlive 
    : public CMETransportOperatorBase	
	, public IMETransportTimerOperator
{
public:
	CMETransportOperatorKeepAlive( IMETransport* pTransport );

	virtual ~CMETransportOperatorKeepAlive();

    /* �̳���CMETransportOperatorBase */
public:
	ME_Result PreSendData( CMEMessageBlock*& pmbBlock );

    /**
    * ���������Ƿ��ڷ����ݣ��Լ��ôεķ��ͽ�������ж��Ƿ�Ϊ����״̬
	*/
	ME_Result PostSendData( CMEMessageBlock*& pmbBlock );

    /**
    * �ж��յ��������Ƿ���������������������ͨ�����÷���ֵ���Ʋ����ص����û���
	* �����յ�ʲô���İ������¡���Ծʱ�䡱
	*/
	ME_Result PreOnReceive( CMEMessageBlock*& pmbBlock );

	DWORD GetType();

	/**
	* ���趨OPTION_TYPE_SET_KEEPALIVE_CHECKNUM��OPTION_TYPE_SET_KEEPALIVE_INTERVEL����ѡ��
	*/
	ME_Result SetOption( DWORD dwOptionType, VOID* pOptionValue );

	/* �̳���ITimerSink */
public:
	/**
	* ��ʱ����������״̬��
	* ���������æ���򲻷������������
	* ����������趨������Դ���������Ϊ�������ӳ�ʱ�������ã��� �����û���Ondisconnect�ص�
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
	* �趨Optionʱ��Ҫ�׵������߳��趨
	*/
	typedef EVENT::EventSheduleTimer<CMETransportKeepAliveTimer, IMETransportTimerOperator> KeepAliveSheduleTimer;
	typedef EVENT::EventCloseRawT<IMETransport> EventCloseWrapper;

private:

private:
	/* �����ӵ�Transport */
    IMETransport*                  m_pTransport;

	/* �����Ծʱ�䣬����ֶ������һ���յ����Ͱ��ɹ���ʱ�� */
	CMETimeValue					m_tvActiveTime;

	BOOL							m_bLastSendOK;

	/**
	* �������� 
	* ��ͨ��OPTION_TYPE_SET_KEEPALIVE_CHECKNUMѡ���趨
	*/
	UINT							m_nMaxCheckNum;

	/* ���ж��ٴμ�����ӵĻ��� */
	UINT							m_nCheckNum;

	KEEPALIVE_TYPE					m_iHeadType;

	BOOL							m_bNewPackage;

#ifdef ME_DEBUG
private:
	/* ��Ԫ������Ԫ�� */
	friend class TestTOKA;
#endif

};

ME_NAME_SPACE_END

#endif	// __ME_TRANSPORT_OPERATOR_KEEPALIVE_H_
