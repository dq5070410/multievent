/**
* 异步IO类型的tcp connector
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
* 异步IO类型的tcp connector
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
	* 打开, 即连接到某一个合法地址
	*
	* <pSink>, 事件接收器
	* <pReactor>, 事件分离器对象
	* <hHandle>, 连接成功后的句柄
	* <peerAddress>, 连接到哪里
	* <localAddress>, 需要绑定的本地地址
	*
	* 返回值:
	*		ME_OK为成功, 其他为失败
	*/
	virtual ME_Result Open( 
		IMEConnectorImplSink* pSink,
		const CMEInetAddress& peerAddress,
		CMEReactor* pReactor,
		ME_HANDLE& hHandle,
		CMEInetAddress* pLocalAddress = NULL );

	/**
	* 关闭, 即停止连接
	*
	* 返回值:
	*		无
	*/
	virtual void Close();

	virtual DWORD AddReference();

	virtual DWORD RemoveReference();

#ifdef ME_WIN

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



