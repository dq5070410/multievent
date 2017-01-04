/**
* ר��UDPЭ���������ԭ����
* 
*
* History
*	3/12/2016		Created
*/

#ifndef __CONNECTOR_UDP_H_
#define __CONNECTOR_UDP_H_

#include "Base.h"
#include "IO.h"
#include "ConnectorImpl.h"
#include "SocketUdp.h"
#include "AutoPtrT.h"

ME_NAME_SPACE_BEGIN

/* ���漰�¼���Ԥ���� */
namespace EVENT
{
	/* �ڵ�ǰ�߳�ִ��OnConnect() */
	template <class ConnectorType> class EventOnConnectCurrentT;
}

/**
* ר��UDPЭ���������ԭ����
*/
class ME_OS_EXPORT CMEConnectorUdp 
	: public IMEIO
	, public IMEConnectorImpl
{
public:
	typedef CMEConnectorUdp ConnectorType;
	typedef IMEConnectorImplSink ConnectorSinkType;

public:
	CMEConnectorUdp();

	virtual ~CMEConnectorUdp();

	/* �̳���IMEIO */
public:
	virtual void SetHandle( ME_HANDLE hHandle );

	virtual ME_HANDLE GetHandle() const;

	/* �̳���IMEConnectorImpl */
public:

	/**
	* ��, �����ӵ�ĳһ���Ϸ���ַ
	*
	* <pSink>, �¼�������
	* <pReactor>, �¼�����������
	* <hHandle>, ���ӳɹ���ľ��
	* <peerAddress>, ���ӵ�����
	* <localAddress>, ��Ҫ�󶨵ı��ص�ַ
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�, ����Ϊʧ��
	*/
	virtual ME_Result Open( 
		IMEConnectorImplSink* pSink,
		const CMEInetAddress& peerAddress,
		CMEReactor* pReactor,
		ME_HANDLE& hHandle,
		CMEInetAddress* pLocalAddress = NULL );

	/**
	* �ر�, ��ֹͣ����
	*
	* ����ֵ:
	*		��
	*/
	virtual void Close();

private:
	void NotifySuccess();

private:
	void OnConnect_c( ME_Result hResult );

protected:
	IMEConnectorImplSink*			m_pSink;		// �ص�����һ����wrapper

	CMESocketUdp					m_SocketUdp;	// udp socket

	/* �¼���Ԫ������ */
protected:
	friend class EVENT::EventOnConnectCurrentT<ConnectorType>;

	/* �¼���Ԫ���ٶ��� */
protected:
	typedef EVENT::EventOnConnectCurrentT<ConnectorType> OnConnectEvent;
};

ME_NAME_SPACE_END

#endif	// __CONNECTOR_UDP_H_




