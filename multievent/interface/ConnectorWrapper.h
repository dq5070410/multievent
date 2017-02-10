/**
* Connector�Ĺ�����
*
* ��Connector���ڲ�ʵ����IMEConnector���жԽ�
* ����ԭ����connector������transport���������ü����Ŀ���
* �Լ����������Ӳ���ֱ����صĹ���(�磬���ӳ�ʱ�ȵ�)��ճ��
*
*
* History
*	1/29/2016		Created
*/

#ifndef __CONNECTOR_WRAPPER_H_
#define __CONNECTOR_WRAPPER_H_

#include "ConnectionInterface.h"
#include "ConnectionDefines.h"
#include "ConnectorImpl.h"
#include "ReferenceControlT.h"
#include "Timer.h"
#include "InetAddress.h"
#include "ConnectionOperatorInterface.h"
#include "Thread.h"

ME_NAME_SPACE_BEGIN

/**
* Connector�Ĺ�����
*
* ��Connector���ڲ�ʵ����IMEConnector���жԽ�
* ����ԭ����connector������transport���������ü����Ŀ���
* �Լ����������Ӳ���ֱ����صĹ���(�磬���ӳ�ʱ�ȵ�)��ճ��
*
* TCP/UDP��wrap
*/
class CMEConnectorWrapper
	: public IConnector
	, protected IMEConnectorImplSink
	, protected ITimerSink
	, public CMEMutexTypeTraits<CMEConnDummy>
	, public CMEReferenceControlT<CMEConnDummy::MutexType>
{
public:
	CMEConnectorWrapper();

	virtual ~CMEConnectorWrapper();

public:
	/**
	* ���ݲ�ͬ�����ͳ�ʼ��һ���������õ�connector
	*
	* <dwType>, ��ʶ��connector��ʲô���͵�, tcp/udp�ȵ�
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����ʧ��
	*/
	ME_Result Initialize( DWORD dwType );

    ME_Result RegisterOperator( IMEConnectionOperator* pOperator );

    ME_Result UnregisterOperator( IMEConnectionOperator* pOperator );

	/* �̳���IMEConnector */
public:
	/**
	* ��һ��Connector���ҳ������ӵ�<peerAddress>
	* ��������ӽ���ʱ, ��ͨ��<pSink>��OnConnect()��֪ͨ
	*
	* <pSink>, �¼�������
	* <peerAddress>, ��Ҫ���ӵĶԶ˵�ַ
	* <pTimeout>, �������ӹ��೤ʱ��û��Ӧ�㳬ʱ
	* <pLocalAddress>, ��Ҫ�󶨵ı��ص�ַ
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ��ME_OKΪʧ��
	*/
	virtual ME_Result Open(
		IConnectorSink* pSink,
		const CMEInetAddress& peerAddress,
		CTimeValue* pTimeout = NULL,
		CMEInetAddress* pLocalAddress = NULL );

	
	/**
	* �ر�һ��Connector����ֹͣ����
	* ���������������֮��, ��ǰ�󶨵Ľ������������յ��κ��¼�
	*
	* ����ֵ:
	*		��
	*/
	virtual void Close();

	/**
	* ����connector������
	*
	* <dwOptionType>, Ҫ����ʲô����
	* <pOptionValue>, ��Ӧ��<dwOptionType>��ֵ
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
    virtual ME_Result SetOption(
        DWORD dwOptionType,
        void* pOptionValue );

	/**
	* ȡ��connector������
	*
	* <dwOptionType>, Ҫȡ��ʲô����
	* <pOptionValue>, out, ��Ӧ��<dwOptionType>��ֵ
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
    virtual ME_Result GetOption(
        WORD dwOptionType,
        void*& pOptionValue );

	/* �̳���IMEReferenceControl */
public:
	/**
	* �������ü���1
	*
	* ����ֵ:
	*		����֮������ü���, DWORD
	*/
	virtual DWORD AddReference();

	/**
	* �������ü���1
	*
	* ����ֵ:
	*		�ݼ�֮������ü���, DWORD
	*/
	virtual DWORD RemoveReference();

	/* �̳���IMEConnectorImplSink */
protected:
	/**
	* �����Ӳ���
	*
	* <hResult>, �Զ����ӵĽ��, һ�㶼��ME_OK
	* <hHandle>, ���Ӿ��
	* 
	* ����ֵ,
	*		��
	*/
	virtual void OnConnect( 
		ME_Result hResult,
		ME_HANDLE hHandle );

	/* �̳���ITimerSink */
protected:
	/**
	* ��schedule��ʱ�ӷ�����
	*
	* <pTimerID>���ĸ�ʱ�ӷ�����
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnTimer( CMETimer* pTimerID );

private:
	void CloseHandle();

    ME_Result PreOnConnect(
        ME_HANDLE hSocket,
        CTransportAutoPtr& pTransportAutoPtr );

private:
    typedef std::deque<IMEConnectionOperatorAutoPtr> OperatorDequeType;

private:
	DWORD							m_dwType;			// ��ʶ��connector��ʲô���͵ģ�tcp/udp...
	CMEAutoPtrT<IMEConnectorImpl>	m_pConnector;		// ԭ����ָ�룬������<m_dwType>��Ӧ

	IConnectorSink*			    	m_pSink;			// �ϲ�ص�����
	CMETimer				 		m_Timer;			// �������ӳ�ʱ�Լ���������
	BOOL							m_bConnected;		// ���ڱ�ʶOpen()�������óɹ����
	ME_HANDLE						m_hHandle;			// ������û�����ӳɹ�(��û�в���transport)ʱ�����Ѿ������ľ������������ֹ���й©

	CMEInetAddress					m_hiaPeerAddress;	// �Զ˵�ַ

    OperatorDequeType               m_dequeOperator;

	CMEThread*						m_pTimerThread; // Ͷ��Timer�߳�
};

ME_NAME_SPACE_END

#endif	// __CONNECTOR_WRAPPER_H_


