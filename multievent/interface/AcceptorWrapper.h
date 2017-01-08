/**
* Acceptor�Ĺ�����
*
* ��acceptor���ڲ�ʵ����IMEAcceptor���жԽ�
* ��ԭ�����acceptor����������transport����
* ���ϣ������������ü������Ա������²㽻��ʹ��
* 
*
* History
*	12/14/2016		Created
*/

#ifndef __ACCEPTOR_WRAPPER_H_
#define __ACCEPTOR_WRAPPER_H_

#include "Base.h"
#include "ConnectionInterface.h"
#include "ConnectionDefines.h"
#include "AcceptorImpl.h"
#include "ReferenceControlT.h"
#include "SocketUdp.h"
#include "ConnectionOperatorInterface.h"

ME_NAME_SPACE_BEGIN

class CMEInetAddress;

namespace EVENT
{
	/* �ڵ�ǰ�߳���ִ��OnConnect() */
	template <class AcceptorType, class TransportType> class EventOnAcceptCurrentT;
}

/**
* Acceptor�Ĺ�����
*
* ��acceptor���ڲ�ʵ����IMEAcceptor���жԽ�
* ��ԭ�����acceptor����������transport����
* ���ϣ������������ü������Ա������²㽻��ʹ��
*
* TCP/UDP����wrap
*/
class CMEAcceptorWrapper
	: public IMEAcceptor
	, public IMEAcceptorUdpImplSink
	, public CMEMutexTypeTraits<CMEConnDummy>
	, public CMEReferenceControlT<CMEConnDummy::MutexType>
{
public:
	typedef CMEAcceptorWrapper AcceptorType;
	typedef IMETransport TransportType;

public:
	CMEAcceptorWrapper();
	virtual ~CMEAcceptorWrapper();

public:
	/**
	* ���ݲ�ͬ�����ͳ�ʼ��һ���������õ�acceptor
	*
	* <dwType>, ��ʶ��acceptor��ʲô���͵�, tcp/udp�ȵ�
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����ʧ��
	*/
	ME_Result Initialize( DWORD dwType );

    ME_Result RegisterOperator( IMEConnectionOperator* pOperator );

    ME_Result UnregisterOperator( IMEConnectionOperator* pOperator );

	/* �̳���IMEAcceptor */
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
		const CMEInetAddress& localAddress );

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

	/* �̳���IMEAcceptorImplSink */
protected:
	/**
	* �����Ӳ���
	*
	* <hResult>, �Զ����ӵĽ��, һ�㶼��ME_OK
	* <pHandle>, �²�����socket���
	* <pPeerAddress>, �Զ˵�ַ
	* 
	* ����ֵ,
	*		��
	*/
	virtual void OnConnect(
		ME_Result hResult,
		ME_HANDLE hHandle,
		CMEInetAddress* pPeerAddress );

	/**
	* �����Ӳ��� -- ������UDP������´���
	*
	* <hResult>, �Զ����ӵĽ��, һ�㶼��ME_OK
	* <hHandle>, UDP acceptor����ľ��
	* 
	* ����ֵ,
	*		��
	*/
	virtual void OnInput(
		ME_Result hResult,
		ME_HANDLE hHandle );

private:
	/**
	* ��ȡ����
	*
	* <hHandle>, socket�����CMEAsyncIoRecvFrom::Result*ָ��
	* <iRecvLen>, ��<hHandle>�л�ȡ�������ݳ���
	* <pszData>, ��<hHandle>�л�ȡ��������
	* <hiaPeerAddr>, �Զ˵�ַ
	* 
	* ����ֵ,
	*		��
	*/
	void ReadData(
		ME_HANDLE hHandle,
		int& iRecvLen,
		char*& pszData,
		CMEInetAddress& hiaPeerAddr );

	/**
	* ֪ͨTransportUdpManager
	*
	* <pTransport>, ����һ��transport��������������
	* <iRecvLen>, ��ȡ�������ݳ���
	* <pszData>, ��ȡ��������
	* <hiaPeerAddr>, �Զ˵�ַ
	* 
	* ����ֵ,
	*		��
	*/
	void NotifyUdpManager( 
		IMETransport* pTransport,
		int iRecvLen,
		char* pszData,
		const CMEInetAddress& hiaPeerAddr );

private:
	void OnConnect_c( TransportType* pTransport );

    ME_Result PreOnConnect(
        ME_HANDLE hSocket,
        CMETransportAutoPtr& pTransportAutoPtr );

	/* �¼���Ԫ������� */
private:
	friend class EVENT::EventOnAcceptCurrentT<AcceptorType, TransportType>;

	/* ��Ԫ�¼����ٶ��� */
private:
	typedef EVENT::EventOnAcceptCurrentT<AcceptorType, TransportType> OnAcceptCurrentEvent;

private:
    typedef std::deque<IMEConnectionOperatorAutoPtr> OperatorDequeType;

private:
	DWORD							m_dwType;		// ��ʶ��acceptor��ʲô���͵ģ�tcp/udp...
	CMEAutoPtrT<IMEAcceptorImpl>	m_pAcceptor;	// 2013.9.6 editedy by Τ�� // ԭ����ָ�룬������<m_dwType>��Ӧ

	int								m_iReactorType;	// ���ڼ�¼�������̵߳�reactor����

	IMEAcceptorSink*				m_pSink;		// �ϲ�ص�����

	CMESocketUdp					m_SocketUdp;	// ���ҽ�����UDPʱ��������

    OperatorDequeType               m_dequeOperator;
};

ME_NAME_SPACE_END

#endif	// __ACCEPTOR_WRAPPER_H_




