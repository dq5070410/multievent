/**
* �첽IO���͵�tcp transport
*
* History
*	9/07/2013		Created
*/

#ifndef __TRANSPORT_ASYCH_TCP_H_
#define __TRANSPORT_ASYCH_TCP_H_

#include "TransportTcp.h"
#include "AsyncIoDefines.h"
#include "AsyncIoSend.h"
#include "AsyncIoRecv.h"

#include "AtomicOperationT.h"
#include "LockMutex.h"

ME_NAME_SPACE_BEGIN

/**
* �첽IO���͵�tcp transport
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
class ME_OS_EXPORT CMETransportAsyncTcp
	: virtual public CMETransportTcp

#ifdef ME_WIN
	, public IMEAsyncIoSink

#elif defined( ME_LINUX )
	, public CMEAsyncIoResult

#endif	// ME_WIN
{
public:
	CMETransportAsyncTcp( ME_HANDLE hSocketHandle );

	virtual ~CMETransportAsyncTcp();

public:
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
	CMEAsyncIoSend						m_asyncSend;
	CMEAsyncIoRecv						m_asyncRecv;

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

#endif	// __TRANSPORT_ASYCH_TCP_H_





