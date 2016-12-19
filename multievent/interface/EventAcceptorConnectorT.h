/**
* 与acceptor、connector相关的一些事件
* 
*
* History
*	12/14/2012		Created
*/

#ifndef __EVENT_ACCEPTOR_CONNECTOR_H_
#define __EVENT_ACCEPTOR_CONNECTOR_H_

#include "CustomizeEventRun.h"

NAME_SPACE_BEGIN

namespace EVENT
{

/**
* Open事件(Acceptor专属)
* 
* 到网络线程执行Open()
*/
template <class AcceptorType, class AddressType>
class EventOpenT : public CEventRun
{
public:
	EventOpenT( 
		AcceptorType* pObject,
		AddressType* pLocalAddress )
		: m_pObject( pObject )
		, m_pLocalAddress( pLocalAddress )
	{

	}

public:

	/**
	* 当事件发生时, 调用目标对象的<Open_n>函数
	*/
	virtual int OnEvent()
	{
		return m_pObject->Open_n( m_pLocalAddress );
	}

private:
	AcceptorType*					m_pObject;
	AddressType*					m_pLocalAddress;
};

template <class AcceptorType, class TransportType>
class EventOnAcceptCurrentT : public CEventRun
{
public:
	EventOnAcceptCurrentT(
		AcceptorType* pAcceptor,
		TransportType* pTransport )
		: m_pAcceptor( pAcceptor )
		, m_pTransport( pTransport )
	{

	}

public:
	/**
	* 当事件发生时, 调用目标对象的<Open_n>函数
	*/
	virtual int OnEvent()
	{
		m_pAcceptor->OnConnect_c( m_pTransport );

		return HIK_OK;
	}

private:
	AcceptorType*					m_pAcceptor;
	TransportType*					m_pTransport;
};

/**
* Open事件(Connector专属)
* 
* 到网络线程执行Open()
*/
template <class ConnectorType, class AddressType, class TimeType>
class EventConnectorOpenT : public CEventRun
{
public:
	EventConnectorOpenT( 
		ConnectorType* pObject,
		TimeType* pTimeout,
		AddressType* pPeerAddress,
		AddressType* pLocalAddress )
		: m_pObject( pObject )
		, m_pTimeout( pTimeout )
		, m_pPeerAddress( pPeerAddress )
		, m_pLocalAddress( pLocalAddress )
	{

	}

public:

	/**
	* 当事件发生时, 调用目标对象的<Open_n>函数
	*/
	virtual int OnEvent()
	{
		return m_pObject->Open_n(
			*m_pPeerAddress,
			m_pTimeout,
			m_pLocalAddress );
	}

private:
	ConnectorType*					m_pObject;
	TimeType*						m_pTimeout;
	AddressType*					m_pPeerAddress;
	AddressType*					m_pLocalAddress;
};

/*
* Close事件
* 
* 到网络线程执行Close()
*/
template <class AcceptorConnectorType>
class EventCloseT : public CEventRun
{
public:
	EventCloseT( AcceptorConnectorType* pObject )
		: m_pObject( pObject )
	{
	}

public:

	/*
	* 当事件发生时, 调用目标对象的<Close_n>函数
	*/
	virtual int OnEvent()
	{
		m_pObject->Close_n();

		return HIK_OK;
	}

private:
	AcceptorConnectorType*			m_pObject;
};

/**
* OnConnect事件(用户线程)
*
* 把在网络线程收到的OnConnect()事件通知到用户线程
* 并在用户线程callback
*/
template <class AcceptorConnectorType, class PtrType>
class EventOnConnectUserT : public CEventRun
{
public:
	EventOnConnectUserT( 
		AcceptorConnectorType* pConnector,
		PtrType pPtr,
		HikResult hResult )
		: m_pConnector( pConnector )
		, m_pPtr( pPtr )
		, m_hResult( hResult )
	{

	}

public:

	/**
	* 当事件发生时, 调用目标对象的<OnConnect_u>函数
	*/
	virtual int OnEvent()
	{
		m_pConnector->OnConnect_u(
			m_pPtr,
			m_hResult );

		return HIK_OK;
	}

private:
	AcceptorConnectorType*			m_pConnector;
	PtrType							m_pPtr;
	HikResult						m_hResult;
};

/**
* OnConnect事件
*
* 有的时候, 由于某种原因而导致尝试连接之后立即得到系统成功的响应
* 但又无法在当前函数callback(在执行函数没有返回之前, 直接在函数
* 执行过程中callback是不符合逻辑的), 那么就可以利用这个事件在函数
* 返回之后, 再callback
*/
template <class ConnectorType>
class EventOnConnectCurrentT : public CEventRun
{
public:
	EventOnConnectCurrentT( 
		ConnectorType* pConnector,
		HikResult hResult )
		: m_pConnector( pConnector )
		, m_hResult( hResult )
	{

	}

public:
	/**
	* 当事件发生时, 调用目标对象的<OnConnect_c>函数
	*/
	virtual int OnEvent()
	{
		m_pConnector->OnConnect_c( m_hResult );

		return HIK_OK;
	}

private:
	ConnectorType*					m_pConnector;
	ME_Result						m_hResult;
};


}

NAME_SPACE_END

#endif	// __EVENT_ACCEPTOR_CONNECTOR_H_








