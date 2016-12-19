/**
* ��acceptor��connector��ص�һЩ�¼�
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
* Open�¼�(Acceptorר��)
* 
* �������߳�ִ��Open()
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
	* ���¼�����ʱ, ����Ŀ������<Open_n>����
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
	* ���¼�����ʱ, ����Ŀ������<Open_n>����
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
* Open�¼�(Connectorר��)
* 
* �������߳�ִ��Open()
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
	* ���¼�����ʱ, ����Ŀ������<Open_n>����
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
* Close�¼�
* 
* �������߳�ִ��Close()
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
	* ���¼�����ʱ, ����Ŀ������<Close_n>����
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
* OnConnect�¼�(�û��߳�)
*
* ���������߳��յ���OnConnect()�¼�֪ͨ���û��߳�
* �����û��߳�callback
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
	* ���¼�����ʱ, ����Ŀ������<OnConnect_u>����
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
* OnConnect�¼�
*
* �е�ʱ��, ����ĳ��ԭ������³�������֮�������õ�ϵͳ�ɹ�����Ӧ
* �����޷��ڵ�ǰ����callback(��ִ�к���û�з���֮ǰ, ֱ���ں���
* ִ�й�����callback�ǲ������߼���), ��ô�Ϳ�����������¼��ں���
* ����֮��, ��callback
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
	* ���¼�����ʱ, ����Ŀ������<OnConnect_c>����
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








