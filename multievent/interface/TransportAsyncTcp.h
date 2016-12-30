/**
* 异步IO类型的tcp transport
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
* 异步IO类型的tcp transport
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
	* 发送数据
	*
	* <pReactor>, 使用哪个reactor
	* <pmbBlock>, 发送的内容
	*
	* 返回值:
	*		ME_OK表示成功; 非ME_OK表示失败
	*/
	virtual ME_Result SendData(
        CMEReactor* pReactor,
        CMEMessageBlock* pmbBlock );

	virtual DWORD AddReference();

	virtual DWORD RemoveReference();

#ifdef ME_WIN

public:
	/**
	 * 打开一个流对象或绑定一个事件接收器
	 *
	 * <pReactor>, 使用哪个reactor
	 * <pSink>, 你想绑定的事件接收器
	 *
	 * 返回值:
	 *		ME_OK表示成功; 非ME_OK表示失败
	*/
	virtual ME_Result Open( 
		CMEReactor* pReactor,
		IMETransportImplSink* pSink );

	/**
	* 关闭一个流对象
	* 一旦调用了这个方法, 那么这个流对象再也不能被使用了, 之前与之
	* 绑定的接收器也不会再接收到任何事件, 如果你还需要一个Transport,
	* 那么你只能通过Connector或者Acceptor的Open()方法, 然后等待
	* Sink的OnConnect()来获得
	*
	* <pReactor>, 使用哪个reactor
	* <hReason>, 你为什么要关闭这个对象
	*
	* 返回值:
	*		ME_OK表示成功; 非ME_OK表示失败
	*/
	virtual ME_Result Close( 
		CMEReactor* pReactor,
		ME_Result hReason = ME_OK );

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
	// 由于担心内存碎片过多的问题，把这个函数注释掉，不使用
	//CMEMessageBlock* CreateAsyncIoBuf();

protected:
	CMEAsyncIoSend						m_asyncSend;
	CMEAsyncIoRecv						m_asyncRecv;

	CMEMessageBlock*					m_pmbBlock;		// 接收数据使用的buf
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





