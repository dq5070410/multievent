/**
* ר���ڴ�����̻߳�����Acceptor
*
* �������߱��κ�ʵ�幦��(ʵ�幦�ܶ���ԭ����acceptor����)
* ��ֻ�ǲ�ͣ��ͨ���¼�Ͷ���������߳��л�����ȷ����о�
* �������в���������һ���߳�����ɵ�
* 
*
* History
*	12/14/2012		Created
*/

#ifndef __ACCEPTOR_CROSS_THREAD_H_
#define __ACCEPTOR_CROSS_THREAD_H_

#include "Base.h"
#include "ConnectionInterface.h"
#include "ConnectionDefines.h"
#include "ReferenceControlT.h"
#include "InetAddress.h"
#include "MTSafeDestroy.h"

ME_NAME_SPACE_BEGIN

/* ���漰�¼���Ԥ���� */
namespace EVENT
{
	/* �������߳�ִ��Open() */
	template <class AcceptorType, class AddressType> class EventOpenT;

	/* �������߳�ִ��Close() */
	template <class AcceptorType> class EventCloseT;

	/* ���û��߳�ִ��OnConnect() */
	template <class AcceptorType, class TransportPtrType> class EventOnConnectUserT;
}

/**
* ר���ڴ�����̻߳�����Acceptor
*
* �������߱��κ�ʵ�幦��(ʵ�幦�ܶ���ԭ����acceptor����)
* ��ֻ�ǲ�ͣ��ͨ���¼�Ͷ���������߳��л�����ȷ����о�
* �������в���������һ���߳�����ɵ�
*/
class CMEAcceptorCrossThread
	: public IMEAcceptor
	, public IMEAcceptorSink
	, public CMEMutexTypeTraits<CMEConnDummy>
	, public CMEReferenceControlT<CMEConnDummy::MutexType>
	, protected CMEMTSafeDestroy
{
public:
	typedef CMEAcceptorCrossThread AcceptorType;
	typedef CMEInetAddress AddressType;
	typedef CMEAutoPtrT<IMETransport> TransportPtrType;

public:
	CMEAcceptorCrossThread( DWORD dwType );
	virtual ~CMEAcceptorCrossThread();

public:
	/**
	* ��һ��Acceptor��������<localAddress>
	* �����µ����ӽ���ʱ, ��ͨ��<pSink>��OnConnect()��֪ͨ
	*
	* <pSink>, �¼�������
	* <pTransport>, һ���²����������� -- ��Ӧһ������
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����ʧ��
	*/
	virtual ME_Result Open( 
		IMEAcceptorSink* pSink,
		const CMEInetAddress& hiaLocalAddr );

	/**
	* �ر�һ��Acceptor����ֹͣ����
	* ���������������֮��, ��ǰ�󶨵Ľ������������յ��κ��¼�
	*
	* <scReason>, �رյ�ԭ����ʲô
	*
	* ����ֵ:
	*		��
	*/
	virtual void Close( ME_Result scReason = ME_OK );

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
	* <pLocalAddress>, ��Ҫ�󶨵ı��ص�ַ
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��
	*/
	ME_Result Open_n( CMEInetAddress* pLocalAddress );

	/**
	* �������߳�ִ��Close()����
	*
	* ����ֵ:
	*		��
	*/
	void Close_n();

	/* �̳���IMEAcceptorSink */
public:
	/**
	* �����յ�����ص���ʱ��, ˵���������Ѿ�������
	* ����<pTransport>, ��ʹ��֮ǰ, �������<pTransport>��Open()
	* ������ָ��һ���¼�������, ֮��������<pTransport>������
	* �¼�, ���������������֪ͨ����
	*
	* <pTransport>, һ���²����������� -- ��Ӧһ������
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnConnect( CMETransportAutoPtr& pTransportAutoPtr ) ;

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
		CMETransportAutoPtr pTransport,
		ME_Result hResult );

	/* �¼���Ԫ������� */
private:
	friend class EVENT::EventOpenT<AcceptorType, AddressType>;
	friend class EVENT::EventCloseT<AcceptorType>;
	friend class EVENT::EventOnConnectUserT<AcceptorType, TransportPtrType>;

	/* ��Ԫ�¼����ٶ��� */
private:
	typedef EVENT::EventOpenT<AcceptorType, AddressType> OpenNetworkEvent;
	typedef EVENT::EventCloseT<AcceptorType> CloseNetworkEvent;
	typedef EVENT::EventOnConnectUserT<AcceptorType, TransportPtrType> OnConnectUserEvent;

private:
	IMEAcceptorSink*				m_pSink;

	DWORD							m_dwType;			// ʲô���͵�acceptor��tcp��udp
	CMEAcceptorAutoPtr				m_pAcceptorAutoPtr;	// acceptor wrapper
	CMEInetAddress					m_hiaLocalAddr;
};

ME_NAME_SPACE_END

#endif	// __ACCEPTOR_CROSS_THREAD_H_











