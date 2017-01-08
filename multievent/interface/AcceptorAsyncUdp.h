/**
* 异步IO类型的udp acceptor
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
* 异步IO类型的Udp acceptor
* 
* 注意：它只适用于Async系列的Reactor
*
* 注意2：Async系列的acceptor/connector/transport针对win32/linux平台
*        异步IO捕捉方式的不同同时又为了统一设计方式(一个设备/一次IO/多个线程)
*		 而专门设计的
*
* 注意3：在win32环境中，作为动作的持有者，通过特定AsyncIo发出动作，并根据动作
*		 的特定结果来做下一步操作；在linux环境中，作为“结果”本身，当动作完成
*		 时自身接受到通知(详见IOCP/epoll的异同以及看类的继承关系)
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
	* 打开, 即侦听与本地某个端口
	*
	* <pSink>, 事件接收器
	* <pReactor>, 事件分离器对象
	* <hiaLocalAddress>, 需要绑定的本地地址
	*
	* 返回值:
	*		ME_OK为成功, 其他为失败
	*/
	virtual ME_Result Open( 
		IMEAcceptorImplSink* pSink,
		CMEReactor* pReactor,
		const CMEInetAddress& hiaLocalAddress );

	/**
	* 关闭, 即停止侦听
	*
	* 返回值:
	*		无
	*/
	virtual void Close( CMEReactor* pReactor );

	/* 继承自IMEIO */
public:
	/**
	* input事件发生
	*
	* <hHandle>，哪个句柄发生了input事件
	* <iEvent>，具体是什么样的事件
	*
	* 返回值:
	*		无
	*/
	virtual void OnInput(
		ME_HANDLE hHandle,
		int iEvent );

	/**
	* output事件发生
	*
	* <hHandle>，哪个句柄发生了output事件
	* <iEvent>，具体是什么样的事件
	*
	* 返回值:
	*		无
	*/
	virtual void OnOutput(
		ME_HANDLE hHandle,
		int iEvent );

	/**
	* close事件发生
	*
	* <hHandle>，哪个句柄发生了close事件
	* <iEvent>，具体是什么样的事件
	*
	* 返回值:
	*		无
	*/
	virtual void OnClose(
		ME_HANDLE hHandle,
		int iEvent );

protected:
	ME_Result GetPeerAddress( CMEInetAddress& peerAddress );

	CMEMessageBlock* CreateAsyncIoBuf();

protected:
	CMEInetAddress					m_hiaFromAddress;	// 对端地址
	CMEAsyncIoRecvFrom				m_asyncRecvFrom;	// 异步RecvFrom动作

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




