/**
* ר���ڴ�����̻߳�����Connector
*
* �������߱��κ�ʵ�幦��(ʵ�幦�ܶ���ԭ����Connector����)
* ��ֻ�ǲ�ͣ��ͨ���¼�Ͷ���������߳��л�����ȷ����о�
* �������в���������һ���߳�����ɵ�
* 
*
* History
*	2/4/2016		Created
*/

#ifndef __CONNECTOR_CROSS_THREAD_H_
#define __CONNECTOR_CROSS_THREAD_H_

#include "Base.h"
#include "ConnectionInterface.h"
#include "ConnectionDefines.h"
#include "ReferenceControlT.h"
#include "TimeValue.h"
#include "InetAddress.h"
#include "MTSafeDestroy.h"

ME_NAME_SPACE_BEGIN

/* ���漰�¼���Ԥ���� */
namespace EVENT
{
	/* �������߳�ִ��Open() */
	template <class ConnectorType, class AddressType, class TimeType> class EventConnectorOpenT;

	/* �������߳�ִ��Close() */
	template <class ConnectorType> class EventCloseT;

	/* ���û��߳�ִ��OnConnect() */
	template <class ConnectorType, class TransportPtrType> class EventOnConnectUserT;
}

/**
* ר���ڴ�����̻߳�����Connector
*
* �������߱��κ�ʵ�幦��(ʵ�幦�ܶ���ԭ����connector����)
* ��ֻ�ǲ�ͣ��ͨ���¼�Ͷ���������߳��л�����ȷ����о�
* �������в���������һ���߳�����ɵ�
*/
class CMEConnectorCrossThread
	: public IConnector
	, public IConnectorSink
	, public CMEMutexTypeTraits<CMEConnDummy>
	, public CMEReferenceControlT<CMEConnDummy::MutexType>
	, protected CMEMTSafeDestroy
{
public:
	typedef CMEConnectorCrossThread ConnectorType;
	typedef CTimeValue TimeType;
	typedef CMEInetAddress AddressType;
	typedef CMEAutoPtrT<ITransport> TransportPtrType;

public:
	CMEConnectorCrossThread( DWORD dwType );

	virtual ~CMEConnectorCrossThread();

	/* �̳���IMEConnector */
public:
	/**
	* ��һ��Connector���ҳ������ӵ�<peerAddress>
	* ��������ӽ���ʱ, ��ͨ��<pSink>��OnConnect()��֪ͨ
	*
	* <pSink>, �¼�������
	* <pTransport>, һ���²����������� -- ��Ӧһ������
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

	/* �̳���CMEReferenceControlT */
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

	/**
	* �����ü���Ϊ0ʱ, ����¼����ᷢ��
	*
	* ��Ĭ�ϵ���Ϊ��ɾ���Լ�
	* �̳������������Ҫ��, ���������������, ��һ��Ҫ��ס
	* ��ִ���ض��߼�֮��, ��Ҫ���˵���CMEReferenceControlT<xx>::OnReferenceRelease()
	* ����������֮���OnReferenceRelease()����delete this
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnReferenceRelease();

private:
	/**
	* �������߳�ִ��Open()����
	*
	* <peerAddress>, ��Ҫ�������ӵĶԶ˵�ַ
	* <pTimeout>, ���ӳ�ʱʱ��
	* <pLocalAddress>, ��Ҫ�󶨵ı��ص�ַ
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��
	*/
	ME_Result Open_n( 
		const CMEInetAddress& peerAddress,
		CTimeValue* pTimeout = NULL,
		CMEInetAddress* pLocalAddress = NULL );

	/**
	* �������߳�ִ��Close()����
	*
	* ����ֵ:
	*		��
	*/
	void Close_n();

	/* �̳���IMEConnectorSink */
public:
	/**
	 * �����յ�����ص���ʱ��, ˵���������Ѿ�������
	 * ����<pTransport>, ��ʹ��<pTransport>֮ǰ, �������<pTransport>��Open()
	 * ������ָ��һ���¼�������, ֮��������<pTransport>�������¼�, ���������
	 * ������֪ͨ����; ���û��ָ��������, �㽫�ղ����κ����ԶԶ˵���Ϣ
	 *
	 * <pTransport>, һ�����������ݷ��͵�������, �Ƿ�ΪNULLȡ����<hResult>
	 * <hResult>, ���ӳɹ����, ME_OKΪ�ɹ�, ����Ϊʧ��ԭ��; ��ME_OKʱ, <pTransport>��ΪNULL, ������ΪNULL
	 *
	 * ����ֵ:
	 *		��
	*/
	virtual void OnConnect( 
		CTransportAutoPtr& pTransportAutoPtr,
		ME_Result hResult );

protected:

	/*
	* �ڱ������̳߳���ɾ������
	* ���ݵ�ǰ״̬��ͬ, ����Ҳ��ͬ
	* ��״̬����DESTROY_STATUS_CALL_IN_CURRENT_THREADʱ,
	* ����ֻ���������߳�Ͷ��һ���¼�, ȷ���������߳���,
	* �й��ڸö�����¼�������ִ��ɾ��֮ǰ, �����;
	* ��״̬����DESTROY_STATUS_BACK_TO_CURRENT_THREADʱ,
	* ִ��ɾ������
	*
	* ����ֵ:
	*		��
	*/
	virtual void Destroy_u();

	/*
	* �������̳߳���ɾ������
	* �������������������ɾ������, ֻ������󱻴������߳�
	* Ͷ��һ���¼�, ֪ͨ����, �������������̵߳��¼��Ѿ�
	* �Ѿ������, ����ɾ��������
	*
	* ����ֵ:
	*		��
	*/
	virtual void Destroy_n();

private:
	/**
	* ���û��̻߳ص�OnConnect()�¼�
	*
	* <pTransport>, �����ڷ���/�������ݵ�transport����ָ�����
	* <hResult>, ���ӵĽ��
	*
	* ����ֵ:
	*		��
	*/
	void OnConnect_u(
		TransportPtrType pTransport,
		ME_Result hResult );

	/* �¼���Ԫ������� */
private:
	friend class EVENT::EventConnectorOpenT<ConnectorType, AddressType, TimeType>;
	friend class EVENT::EventCloseT<ConnectorType>;
	friend class EVENT::EventOnConnectUserT<ConnectorType, TransportPtrType>;

	/* ��Ԫ�¼����ٶ��� */
private:
	typedef EVENT::EventConnectorOpenT<ConnectorType, AddressType, TimeType> OpenNetworkEvent;
	typedef EVENT::EventCloseT<ConnectorType> CloseNetworkEvent;
	typedef EVENT::EventOnConnectUserT<ConnectorType, TransportPtrType> OnConnectUserEvent;

private:
	IConnectorSink*				m_pSink;				// �ϲ�����Ļص�����

	DWORD							m_dwType;				// ʲô���͵�connector��tcp��udp
	CConnectorAutoPtr			m_pConnectorAutoPtr;	// connector wrapper
	CTimeValue					m_htvTimeout;			// ���ӳ�ʱ����
	CMEInetAddress					m_hiaPeerAddr;			// �Զ˵�ַ
	CMEInetAddress					m_hiaLocalAddr;			// ��Ҫ�󶨵ı�����ַ
};



ME_NAME_SPACE_END

#endif	// __CONNECTOR_CROSS_THREAD_H_









