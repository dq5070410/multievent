/**
* ר��TCPЭ���������ԭ����
* 
*
* History
*	1/28/2016		Created
*/

#ifndef __CONNECTOR_TCP_H_
#define __CONNECTOR_TCP_H_

#include "Base.h"
#include "IO.h"
#include "ConnectorImpl.h"
#include "SocketTcp.h"
#include "AutoPtrT.h"

ME_NAME_SPACE_BEGIN

namespace EVENT
{
	template <class ConnectorType> class EventOnConnectCurrentT;
}

/**
* ר��TCPЭ���������ԭ����
*
* ��ֻ��������������ǹ���������Ϊ�������ľ����
* ������ʹ���� -- ͨ����wrapper
*/
class ME_OS_EXPORT CMEConnectorTcp
	: public IMEIO
	, public IMEIOSink
	, public IMEConnectorImpl
{
public:
	typedef CMEConnectorTcp ConnectorType;
	typedef IMEConnectorImplSink ConnectorSinkType;

public:
	CMEConnectorTcp();

	virtual ~CMEConnectorTcp();

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

protected:
	ME_Result Connect(
		CMESocketTcp* pSocket,
		const CMEInetAddress& peerAddr,
		CMEReactor* pReactor,
		int iIndex );

	void NotifySuccess();

	/* �̳���IMEIOSink */
protected:
	/**
	* input�¼�����
	*
	* <hHandle>���ĸ����������input�¼�
	* <iEvent>��������ʲô�����¼�
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnInput(
		ME_HANDLE hHandle,
		int iEvent );

	/**
	* output�¼�����
	*
	* <hHandle>���ĸ����������output�¼�
	* <iEvent>��������ʲô�����¼�
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnOutput(
		ME_HANDLE hHandle,
		int iEvent );

	/**
	* close�¼�����
	*
	* <hHandle>���ĸ����������close�¼�
	* <iEvent>��������ʲô�����¼�
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnClose(
		ME_HANDLE hHandle,
		int iEvent );

private:
	void OnConnect_c( ME_Result hResult );

protected:
	ConnectorSinkType*				m_pSink;		// �ص�����һ�㶼��connector wrapper

	CMESocketTcp					m_SocketTcp;	// ��ʵֻ��һ�����ס��������������
	CMEReactor*					m_pReactor;		// io ������
	int								m_iIndex;		// ע���¼�ʱ��<m_pReactor>���ص��¼�����

	/* �¼���Ԫ������ */
private:
	friend class EVENT::EventOnConnectCurrentT<ConnectorType>;

	/* �¼���Ԫ���ٶ��� */
private:
	typedef EVENT::EventOnConnectCurrentT<ConnectorType> OnConnectEvent;
};

ME_NAME_SPACE_END

#endif	// __CONNECTOR_TCP_H_

