/**
* �첽IO���͵�udp acceptor
* 
*
* History
*	9/07/2016		Created
*/
#ifndef __ACCEPTOR_ASYCH_UDP_H_
#define __ACCEPTOR_ASYCH_UDP_H_

#include "AcceptorUdp.h"
#include "AsyncIoDefines.h"
#include "AsyncIoRecvFrom.h"

ME_NAME_SPACE_BEGIN

/**
* �첽IO���͵�Udp acceptor
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
class ME_OS_EXPORT CMEAcceptorAsyncUdp
	: public CMEAcceptorUdp

#ifdef ME_WIN
	, public IMEAsyncIoSink

#elif defined( ME_LINUX )
	, public CMEAsyncIoResult

#endif	// ME_WIN
{
public:
	CMEAcceptorAsyncUdp();

	virtual ~CMEAcceptorAsyncUdp();

public:
	virtual DWORD AddReference();

	virtual DWORD RemoveReference();

#ifdef ME_WIN

public:
	/**
	* ��, �������뱾��ĳ���˿�
	*
	* <pSink>, �¼�������
	* <pReactor>, �¼�����������
	* <hiaLocalAddress>, ��Ҫ�󶨵ı��ص�ַ
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�, ����Ϊʧ��
	*/
	virtual ME_Result Open( 
		IMEAcceptorImplSink* pSink,
		CMEReactor* pReactor,
		const CMEInetAddress& hiaLocalAddress );

	/**
	* �ر�, ��ֹͣ����
	*
	* ����ֵ:
	*		��
	*/
	virtual void Close( CMEReactor* pReactor );

	/* �̳���IMEIO */
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
	ME_Result GetPeerAddress( CMEInetAddress& peerAddress );

	CMEMessageBlock* CreateAsyncIoBuf();

protected:
	CMEInetAddress					m_hiaFromAddress;	// �Զ˵�ַ
	CMEAsyncIoRecvFrom				m_asyncRecvFrom;	// �첽RecvFrom����

#elif defined( ME_LINUX )

public:
	virtual void OnCompleteImpl( 
		BOOL bResult,
		int iActiveEvent,
		ME_HANDLE hInterface );

	virtual void OnDestroy();

#endif	// ME_WIN

protected:
	AcceptorSinkType*				m_pRefSink;
};

ME_NAME_SPACE_END

#endif	// __ACCEPTOR_ASYCH_UDP_H_




