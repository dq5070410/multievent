/**
* ר���ڴ�����̻߳�����Transport
*
* �������߱��κ�ʵ�幦��(ʵ�幦�ܶ���ԭ����transport����)
* ��ֻ�ǲ�ͣ��ͨ���¼�Ͷ���������߳��л�����ȷ����о�
* �������в���������һ���߳�����ɵ�
*
* History
*	3/1/2016		Created
*/

#ifndef __ME_TRANSPORT_CROSS_THREAD_H_
#define __ME_TRANSPORT_CROSS_THREAD_H_

#include "Base.h"
#include "ConnectionInterface.h"
#include "ConnectionDefines.h"
#include "MTSafeDestroy.h"
#include "LockThreadMutex.h"

ME_NAME_SPACE_BEGIN

/* ʹ�õ��¼�Ԥ���� */
namespace EVENT
{
	/* �������߳�ִ��Open() */
	template <class TransportType> class EventOpenT;

	/* �������߳�ִ��Close() */
	template <class TransportType> class EventCloseT;

	/* �������߳�ִ��SendData() */
	template <class TransportType, class PayloadTypeAutoPtr> class EventSendDataT;

	/* ���û��߳�ִ��OnDisconnect() */
	template <class TransportType> class EventOnDisconnectT;

	/* ���û��߳�ִ��OnReceive() */
	template <class TransportType, class PayloadType> class EventOnReceiveT;

	/* ���û��߳�ִ��OnCanSendAgain() */
	template <class TransportType> class EventOnCanSendAgainT;
}

/**
* ר���ڴ�����̻߳�����Transport
*
* �������߱��κ�ʵ�幦��(ʵ�幦�ܶ���ԭ����transport����)
* ��ֻ�ǲ�ͣ��ͨ���¼�Ͷ���������߳��л�����ȷ����о�
* �������в���������һ���߳�����ɵ�
*/
class CMETransportCrossThread 
	: public IMETransport
	, public IMETransportSink
	, public CMEMutexTypeTraits<CMEConnDummy>
	, public CMEReferenceControlT<CMEConnDummy::MutexType>
	, protected CMEMTSafeDestroy
{
public:
	typedef CMETransportCrossThread TransportType;
	typedef CMEMessageBlock PayloadType;
	typedef CMEAutoPtrT<PayloadType> PayloadTypeAutoPtr;

public:
	CMETransportCrossThread( IMETransport* pSrcTransport );

	virtual ~CMETransportCrossThread();

	/* �̳���IMETransport */
public:
	/**
	 * ��һ����������һ���¼�������
	 *
	 * <pSink>, ����󶨵��¼�������
	 *
	 * ����ֵ:
	 *		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result Open( IMETransportSink* pSink );

	/**
	* �ر�һ��������
	* һ���������������, ��ô�����������Ҳ���ܱ�ʹ����, ֮ǰ��֮
	* �󶨵Ľ�����Ҳ�����ٽ��յ��κ��¼�, ����㻹��Ҫһ��Transport,
	* ��ô��ֻ��ͨ��Connector����Acceptor��Open()����, Ȼ��ȴ�
	* Sink��OnConnect()�����
	*
	* <hReason>, ��ΪʲôҪ�ر��������
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result Close( ME_Result hReason = ME_OK );

	/**
	* ��������
	*
	* <pmbBlock>, ���͵�����
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result SendData( CMEMessageBlock* pmbBlock );

	/**
	* ����transport������
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
	* ȡ��transport������
	*
	* <dwOptionType>, Ҫȡ��ʲô����
	* <pOptionValue>, out, ��Ӧ��<dwOptionType>��ֵ
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result GetOption(
		DWORD dwOptionType,
		void*& pOptionValue );

	/**
	* ȡ�õ�ǰ���transport�趨��sink
	*
	* ����ֵ:
	*		��NULL��ʾ�ɹ�; NULL��ʾʧ��
	*/
	virtual IMETransportSink* GetSink() const;

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

	/* ��CMEMTSafeDestroy�̳� */
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

	/*
	* �������߳�ִ��Open()����
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��
	*/
	ME_Result Open_n();

	/*
	* �������߳�ִ��Close()����
	*
	* ����ֵ:
	*		��
	*/
	void Close_n();

	/*
	* �������߳�ִ��SendData()����
	*
	* <pmbBlock>, ��Ҫ���͵�����
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��
	*/
	ME_Result SendData_n( PayloadType* pmbBlock );

	/* �̳���IMETransportSink */
public:
	/**
	* �յ�����
	*
	* <pTransport>, ��һ����������һ�������յ�������
	* <pmbBlock>, �յ����ݵ�����
	*/
	virtual void OnReceive( 
		IMETransport* pTransport,
		CMEMessageBlock* pmbBlock );

	/**
	* ���ӱ��Ͽ�
	* 
	* <pTransport>, ��һ�����ӶϿ���
	* <hReason>, Ϊʲô�Ͽ���
	*/
	virtual void OnDisconnect( 
		IMETransport* pTransport,
		ME_Result hReason = ME_OK );

	/**
	* ĳ�����ӿ������·���������
	* �����ݷ����ر�Ƶ��ʱ, ����tcp/ip buffer�Ѿ�����, �޷������������ݵ�ʱ��,
	* Transport��SendData()�����᷵��ʧ��, ���ʱ��, �Ͳ�Ӧ�ü����ٷ�����,
	* (�ֵ�, �ǵü��SendData(), �����������ʧ����, �ǵñ���÷���ʧ�ܵ������, ׼���´��ٷ�)
	* ��Ϊ����ô��, ������ɹ���, ��Ψһ��Ҫ���ľ��ǵȴ�����ص�
	* �����յ�����ص���ʱ��, ��ϲ, ���ֿ��Լ�������������Ϸ���������
	*
	* <pTransport>, ��һ�����ӿ��Լ������������� 
	* <hResult>, Ϊʲô���Լ�������? (ͨ��, ���ﶼֻ�᷵��ME_OK) 
	*
	*/
	virtual void OnCanSendAgain( 
		IMETransport* pTransport,
		ME_Result hReason = ME_OK );

private:

	/*
	* ���û��̻߳ص�OnDisConnect()�¼�
	*
	* <hResult>, ʲôԭ�������ӶϿ�
	*
	* ����ֵ:
	*		��
	*/
	void OnDisconnect_u( ME_Result hResult );

	/*
	* ���û��̻߳ص�OnReceive()�¼�
	*
	* <pmbBlock>, �յ�������
	*
	* ����ֵ:
	*		��
	*/
	void OnReceive_u( PayloadTypeAutoPtr& pmbBlock );

	/*
	* ���û��̻߳ص�OnCanSendAgain()�¼�
	*
	* <hResult>, �����ʲô - һ�㶼��ME_OK
	*
	* ����ֵ:
	*		��
	*/
	void OnCanSendAgain_u( ME_Result hResult = ME_OK );

private:
	IMETransportSink*					m_pSink;					// �ϲ���¼�������
	CMETransportAutoPtr				m_pTransport;				// ԭ����transport������ָ��
	CMELockThreadMutex					m_sinkLock;					// �Իص�������б���

	BOOL								m_bSendFailed;				// �Ƿ��ڷ���ʧ��״̬
	BOOL								m_bNeedToCallCanSendAgain;	// �Ƿ���Ҫ֪ͨ�ϲ�OnCanSendAgain
	CMEAtomicOperationT<CMELockMutex>	m_bClosed;					// �Ƿ��Ѿ����ù�Close()����

private:
	typedef CMEAutoPtrT<PayloadType> ListItemType;
	typedef std::list<ListItemType> DataBufferType;
	DataBufferType						m_DataBuf;					// ���ڱ��洦�ڷ���ʧ��״̬ʱ�İ�

	/* �¼���Ԫ������� */
private:
	friend class EVENT::EventOpenT<TransportType>;
	friend class EVENT::EventCloseT<TransportType>;
	friend class EVENT::EventSendDataT<TransportType, PayloadType>;
	friend class EVENT::EventOnReceiveT<TransportType, PayloadTypeAutoPtr>;
	friend class EVENT::EventOnDisconnectT<TransportType>;
	friend class EVENT::EventOnCanSendAgainT<TransportType>;

	/* ��Ԫ�¼����ٶ��� */
private:
	typedef EVENT::EventOpenT<TransportType> OpenNetworkEvent;
	typedef EVENT::EventCloseT<TransportType> CloseNetworkEvent;
	typedef EVENT::EventSendDataT<TransportType, PayloadType> SendDataNetworkEvent;
	typedef EVENT::EventOnReceiveT<TransportType, PayloadTypeAutoPtr> OnReceiveUserEvent;
	typedef EVENT::EventOnDisconnectT<TransportType> OnDisconnectUserEvent;
	typedef EVENT::EventOnCanSendAgainT<TransportType> OnCanSendAgainUserEvent;
};

ME_NAME_SPACE_END

#endif	// __TRANSPORT_CROSS_THREAD_H_






