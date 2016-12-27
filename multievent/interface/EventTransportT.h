/**
* 与transport相关的一些事件
* 
*
* History
*	12/1/2016		Created
*/
#ifndef __ME_EVENT_TRANSPORT_H_
#define __ME_EVENT_TRANSPORT_H_

#include "Base.h"
#include "CustomizeEventRun.h"

ME_NAME_SPACE_BEGIN

namespace EVENT
{

template <class TransportType>
class EventOpenT : public CHikEventRun
{
public:
	EventOpenT( TransportType* pTransport )
		: m_pTransport( pTransport )

	{

	}

public:
	virtual int OnEvent()
	{
		return m_pTransport->Open_n();
	}

private:
	TransportType*					m_pTransport;
};

template <class TransportType>
class EventCloseT : public CEventRun
{
public:
	EventCloseT( TransportType* pTransport )
		: m_pTransport( pTransport )

	{

	}

public:
	virtual int OnEvent()
	{
		m_pTransport->Close_n();

		return ME_OK;
	}

private:
	TransportType*					m_pTransport;
};

template <class TransportType, class PayloadType>
class EventSendDataT : public CEventRun
{
public:
	EventSendDataT( 
		TransportType* pTransport,
		PayloadType* pData )
		: m_pTransport( pTransport )
		, m_pData( pData )
	{

	}

public:
	virtual int OnEvent()
	{
		return m_pTransport->SendData_n( m_pData );
	}

private:
	TransportType*					m_pTransport;
	PayloadType*					m_pData;
};

template <class TransportType>
class EventOnDisconnectT : public CEventRun
{
public:
	EventOnDisconnectT( 
		TransportType* pTransport,
		ME_Result hResult )
		: m_pTransport( pTransport )
		, m_hResult( hResult )
	{

	}

public:
	virtual int OnEvent()
	{
		m_pTransport->OnDisconnect_u( m_hResult );

		return ME_OK;
	}

private:
	TransportType*					m_pTransport;
	ME_Result						m_hResult;
};


template <class TransportType, class PayloadType>
class EventOnReceiveT : public CEventRun
{
public:
	EventOnReceiveT( 
		TransportType* pTransport,
		PayloadType pData )
		: m_pTransport( pTransport )
		, m_pData( pData )
	{

	}

public:
	virtual int OnEvent()
	{
		m_pTransport->OnReceive_u( m_pData );

		return ME_OK;
	}

private:
	TransportType*					m_pTransport;
	PayloadType						m_pData;
};


template <class TransportType>
class EventOnCanSendAgainT : public CEventRun
{
public:
	EventOnCanSendAgainT( 
		TransportType* pTransport,
		ME_Result hResult )
		: m_pTransport( pTransport )
		, m_hResult( hResult )
	{

	}

public:
	virtual int OnEvent()
	{
		m_pTransport->OnCanSendAgain_u( m_hResult );

		return ME_OK;
	}

private:
	TransportType*					m_pTransport;
	ME_Result						m_hResult;
};


template <class TransportType, class TransportSinkType>
class EventOnDisconnectCurrentT : public CEventRun
{
public:
	EventOnDisconnectCurrentT( 
		TransportType* pTransport,
		TransportSinkType* pSink,
		ME_Result hResult )
		: m_pTransport( pTransport )
		, m_pSink( pSink )
		, m_hResult( hResult )
	{

	}

public:
	virtual int OnEvent()
	{
		m_pTransport->OnDisconnect_c( 
			m_pSink,
			m_hResult );

		return ME_OK;
	}

private:
	TransportType*					m_pTransport;
	TransportSinkType*				m_pSink;
	ME_Result						m_hResult;
};

template <class TransportType>
class EventUdpDeliveryDataT : public CEventRun
{
public:
	EventUdpDeliveryDataT( TransportType* pTransport )
		: m_pTransport( pTransport )
	{

	}

public:
	virtual int OnEvent()
	{
		m_pTransport->DeliveryData();

		return ME_OK;
	}

private:
	TransportType*					m_pTransport;
};

template <class Timer, class Operator>
class EventSheduleTimer : public CEventRun
{
public:
	EventSheduleTimer( 
		Timer* pTimer,
		Operator* pOperator,
		UINT iInterval )
		: m_pTimer( pTimer )
		, m_pOperator( pOperator )
		, m_iInterval( iInterval )
	{

	}

public:
	virtual int OnEvent()
	{
		return m_pTimer->Update( m_pOperator, m_iInterval );
	}

private:
	Timer*							m_pTimer;
	Operator*						m_pOperator;
	UINT							m_iInterval;

};

template <class TransportType>
class EventCloseRawT : public CEventRun
{
public:
	EventCloseRawT( 
		TransportType* pTransport,
		ME_Result hCloseReason )
		: m_pTransport( pTransport )
		, m_hCloseReason( hCloseReason )
	{

	}

public:
	virtual int OnEvent()
	{
		m_pTransport->Close( m_hCloseReason );

		return ME_OK;
	}

private:
	TransportType*					m_pTransport;
	ME_Result						m_hCloseReason;
};

}

ME_NAME_SPACE_END

#endif	// __ME_EVENT_TRANSPORT_H_
