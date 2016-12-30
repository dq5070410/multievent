/**
* �첽IO���͵�udp transport
* 
*
* History
*	9/07/2013		Created
*/

#ifndef __TRANSPORT_ASYCH_UDP_H_
#define __TRANSPORT_ASYCH_UDP_H_

#include "TransportUdp.h"
#include "AsyncIoDefines.h"
#include "AsyncIoSendTo.h"
#include "AsyncIoRecvFrom.h"

#include "AtomicOperationT.h"
#include "LockMutex.h"

ME_NAME_SPACE_BEGIN

/**
* �첽IO���͵�udp transport
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
class ME_OS_EXPORT CMETransportAsyncUdp
	: public CMETransportUdp

#ifdef ME_WIN
	, public IMEAsyncIoSink

#elif defined( ME_LINUX )
	, public CMEAsyncIoResult

#endif	// ME_WIN
{
public:
	CMETransportAsyncUdp( ME_HANDLE hSocketHandle );

	virtual ~CMETransportAsyncUdp();

public:
	virtual DWORD AddReference();

	virtual DWORD RemoveReference();

#ifdef ME_WIN

public:
	/**
	 * ��һ����������һ���¼�������
	 *
	 * <pReactor>, ʹ���ĸ�reactor
	 * <pSink>, ����󶨵��¼�������
	 *
	 * ����ֵ:
	 *		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result Open( 
		CMEReactor* pReactor,
		IMETransportImplSink* pSink );

	/**
	* ��������
	*
	* <pReactor>, ʹ���ĸ�reactor
	* <pmbBlock>, ���͵�����
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result SendData( 
		CMEReactor* pReactor,
		CMEMessageBlock* pmbBlock );

	/**
	* �ر�һ��������
	* һ���������������, ��ô�����������Ҳ���ܱ�ʹ����, ֮ǰ��֮
	* �󶨵Ľ�����Ҳ�����ٽ��յ��κ��¼�, ����㻹��Ҫһ��Transport,
	* ��ô��ֻ��ͨ��Connector����Acceptor��Open()����, Ȼ��ȴ�
	* Sink��OnConnect()�����
	*
	* <pReactor>, ʹ���ĸ�reactor
	* <hReason>, ��ΪʲôҪ�ر��������
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result Close( 
		CMEReactor* pReactor,
		ME_Result hReason = ME_OK );

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
	// ���ڵ����ڴ���Ƭ��������⣬���������ע�͵�����ʹ��
	//CMEMessageBlock* CreateAsyncIoBuf();

protected:
	CMEAsyncIoSendTo					m_asyncSendTo;
	CMEAsyncIoRecvFrom					m_asyncRecvFrom;
	CMEMessageBlock*					m_pmbBlock;		// ��������ʹ�õ�buf

	CMEAtomicOperationT<CMELockMutex>	m_lSentFailed;

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

protected:
	CMEAtomicOperationT<CMELockMutex>	m_bInUpcall;

#endif	// ME_WIN

protected:
	TransportSinkType*				m_pRefSink;
};

ME_NAME_SPACE_END

#endif	// __TRANSPORT_ASYCH_UDP_H_


















