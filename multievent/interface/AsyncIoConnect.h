/**
* 异步IO操作类 - Connect动作
* 
*
* History
*	9/04/2016		Created
*/
#ifndef __ASYCH_IO_CONNECT_H_
#define __ASYCH_IO_CONNECT_H_

#include "Base.h"
#include "AsyncIoResult.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

class CMEInetAddress;

/**
* 异步IO操作类 - Connect
* 
* 用于尝试向某个地址建立连接
*/
class ME_OS_EXPORT CMEAsyncIoConnect
{
public:
	/**
	* 投递一个Connect请求，尝试向某个地址建立连接
	*
	* <pSink>, 事件接收器
	* <hConnectHandle>, 使用哪一个句柄进行连接
	* <pPeerAddress>, 对端地址
	* <bIPv4>, 地址类型, 是IPv4还是IPv6
	* <bAutoBind>, 是否自动绑定本地地址, 如果设置为FALSE, 那么你需要确保<hConnectHandle>已经被
	*			   绑定到某个本地地址上, 否则, 连接会失败
	*
	* 返回值:
	*		ME_OK为成功, 其他为失败
	*/
	ME_Result Post(
		IMEAsyncIoSink* pSink,
		ME_HANDLE hConnectHandle,
		const CMEInetAddress* pPeerAddress,
		BOOL bIPv4 = TRUE,
		BOOL bAutoBind = TRUE );

	/**
	* Cancel某个句柄的上的所有在队列当中的请求
	*
	* <hHandle>, 需要cancel的socket句柄
	*
	* 返回值:
	*		ME_OK为成功, 其他为失败
	*/
	void Cancel( ME_HANDLE hHandle );

public:
	/**
	* 异步IO操作类Connect的专用结果类
	* 
	* 当Connect操作完成时, 会透过这个类的对象进行通知
	*/
	class CMEAsyncIoConnectResult : public CMEAsyncIoResult
	{
	public:
		CMEAsyncIoConnectResult(
			IMEAsyncIoSink* pSink,
			ME_HANDLE hConnectHandle );

	protected:
		/**
		* 在Connect IO执行完成之后会被call
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
	};

	typedef CMEAsyncIoConnectResult Result;
};

ME_NAME_SPACE_END

#endif	// ME_WINs

#endif	// __ASYCH_IO_CONNECT_H_

















