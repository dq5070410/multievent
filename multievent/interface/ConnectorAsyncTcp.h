/**
* �첽IO���͵�tcp connector
* 
*
* History
*	9/07/2016		Created
*/

#ifndef __CONNECTOR_ASYCH_TCP_H_
#define __CONNECTOR_ASYCH_TCP_H_

#include "ConnectorTcp.h"
#include "AsyncIoDefines.h"
#include "AsyncIoConnect.h"

ME_NAME_SPACE_BEGIN

/**
* �첽IO���͵�tcp connector
* 
* ע�⣺��ֻ������Asyncϵ�е�Reactor
*
* ע��2��Asyncϵ�е�acceptor/connector/transport���win32/linuxƽ̨
*        �첽IO��׽��ʽ�Ĳ�ͬͬʱ��Ϊ��ͳһ��Ʒ�ʽ(һ���豸/һ��IO/����߳�)
*		 ��ר����Ƶ�
*
* ע��3����win32�����У���Ϊ�����ĳ����ߣ�ͨ���ض�AsyncIo���������������ݶ���
*		 ���ض����������һ����������linux�����У���Ϊ��������������������
*		 ʱ������ܵ�֪ͨ(���IOCP/epoll����ͬ�Լ�����ļ̳й�ϵ)
*/
class ME_OS_EXPORT CMEConnectorAsyncTcp
	: virtual public CMEConnectorTcp

#ifdef ME_WIN
	, public IMEAsyncIoSink

#elif defined( ME_LINUX )
	, public CMEAsyncIoResult

#endif	// ME_WIN
{
public:
	CMEConnectorAsyncTcp();

	virtual ~CMEConnectorAsyncTcp();

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

	virtual DWORD AddReference();

	virtual DWORD RemoveReference();

#ifdef ME_WIN

public:
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

protected:
	CMEAsyncIoConnect				m_asyncConnect;

#elif defined( ME_LINUX )

public:
	virtual void OnOutput(
		ME_HANDLE hHandle,
		int iEvent );

	virtual void OnCompleteImpl( 
		BOOL bResult,
		int iActiveEvent,
		ME_HANDLE hInterface );

	virtual void OnDestroy();

#endif	// ME_WIN

protected:
	ConnectorSinkType*				m_pRefSink;

};

ME_NAME_SPACE_END

#endif	// __CONNECTOR_ASYCH_TCP_H_



