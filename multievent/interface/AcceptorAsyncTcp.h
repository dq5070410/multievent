/**
* �첽IO���͵�tcp acceptor
* 
*
* History
*	9/07/2016		Created
*/

#ifndef __ACCEPTOR_ASYCH_TCP_H_
#define __ACCEPTOR_ASYCH_TCP_H_

#include "Base.h"
#include "AcceptorTcp.h"
#include "AsyncIoDefines.h"
#include "AsyncIoAccept.h"

ME_NAME_SPACE_BEGIN

/**
* �첽IO���͵�tcp acceptor
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
class ME_OS_EXPORT CMEAcceptorAsyncTcp 
	: public CMEAcceptorTcp

#ifdef ME_WIN
	, public IMEAsyncIoSink

#elif defined( ME_LINUX )
	, public CMEAsyncIoResult

#endif	// ME_WIN
{
public:
	CMEAcceptorAsyncTcp();

	virtual ~CMEAcceptorAsyncTcp();

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
	/**
	* ���¶Զ˵�ַ
	*
	* ����ʹ��::AccepteEx()���������ص�ַ�Լ�Զ�˵�ַ�������ݿ� - ȡ����::AcceptEx()ʱ
	* �Ƿ����<lpOutputBuffer>����������һ��ϵͳ�ڲ���ʽ(TDI)��¼������Ҫ���¶Զ˵�ַ��
	* ��Ҫ -- �����ǵ�һʱ����£�ʹ��::GetAcceptExSockaddrs()����
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�, ����Ϊʧ��
	*/
	ME_Result GetPeerAddress( 
		CMEInetAddress& hiaPeerAddr,
		CMEInetAddress& hiaLocalAddr,
		CMEAsyncIoAccept::Result* pIoResult );

protected:
	CMEAsyncIoAccept				m_asyncAccept;	// �첽Accept����

#elif defined( ME_LINUX )

public:
	/**
	* ��ָ����IOִ�����֮��ᱻcall
	* �̳�����Ҫʵ������ӿ�
	*
	* <bResult>, �������
	* <iActiveEvent>, ʲô�¼�������
	* <hInterface>, ˭�����IO
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnCompleteImpl( 
		BOOL bResult,
		int iActiveEvent,
		ME_HANDLE hInterface );

	/**
	* ��ָ����OnCompleteImpl()ִ�����֮��ᱻcall��
	* Ĭ�϶�����delete this������̳�����������Ҫ������
	* ����ʵ��OnDestroy()
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnDestroy();

#endif	// ME_WIN

protected:
	AcceptorSinkType*				m_pRefSink;
};

ME_NAME_SPACE_END

#endif	// __ACCEPTOR_ASYCH_TCP_H_


