/**
* 异步IO类型的tcp acceptor
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
* 异步IO类型的tcp acceptor
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
	/**
	* 更新对端地址
	*
	* 由于使用::AccepteEx()函数，本地地址以及远端地址还有数据块 - 取决于::AcceptEx()时
	* 是否填充<lpOutputBuffer>参数，被用一种系统内部格式(TDI)记录，所以要更新对端地址就
	* 需要 -- 尤其是第一时间更新，使用::GetAcceptExSockaddrs()函数
	*
	* 返回值:
	*		ME_OK为成功, 其他为失败
	*/
	ME_Result GetPeerAddress( 
		CMEInetAddress& hiaPeerAddr,
		CMEInetAddress& hiaLocalAddr,
		CMEAsyncIoAccept::Result* pIoResult );

protected:
	CMEAsyncIoAccept				m_asyncAccept;	// 异步Accept动作

#elif defined( ME_LINUX )

public:
	/**
	* 在指定的IO执行完成之后会被call
	* 继承类需要实现这个接口
	*
	* <bResult>, 操作结果
	* <iActiveEvent>, 什么事件发生了
	* <hInterface>, 谁处理的IO
	*
	* 返回值:
	*		无
	*/
	virtual void OnCompleteImpl( 
		BOOL bResult,
		int iActiveEvent,
		ME_HANDLE hInterface );

	/**
	* 在指定的OnCompleteImpl()执行完成之后会被call，
	* 默认动作是delete this，如果继承类有特殊需要，可以
	* 自行实现OnDestroy()
	*
	* 返回值:
	*		无
	*/
	virtual void OnDestroy();

#endif	// ME_WIN

protected:
	AcceptorSinkType*				m_pRefSink;
};

ME_NAME_SPACE_END

#endif	// __ACCEPTOR_ASYCH_TCP_H_


