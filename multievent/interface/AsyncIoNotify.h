/**
* 异步IO操作类 - Notify动作
* 
*
* History
*	9/04/2016		Created
*/

#ifndef __ASYCH_IO_NOTIFY_H_
#define __ASYCH_IO_NOTIFY_H_

#include "AsyncIoResult.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

/**
* 异步IO操作类 - Notify
* 
* 用于投递一个“完成”的信号，以便于唤醒
* 或者说，这是一个Wakener专用的操作类
*/
class CMEAsyncIoNotify
{
public:
	/**
	* 投递一个Notify请求，以唤醒处于阻塞状态的IOCP对象或持有者(IOCP Reactor)
	*
	* <pSink>, 事件接收器
	* <hInterface>, 要唤醒哪一个IOCP对象
	*
	* 返回值:
	*		ME_OK为成功, 其他为失败
	*/
	ME_Result Post(
		IMEAsyncIoSink* pSink,
		ME_HANDLE hInterface );

public:
	/**
	* 异步IO操作类Notify的专用结果类
	* 
	* 当Notify操作完成时, 会透过这个类的对象进行通知
	*/
	class CMEAsyncIoNotifyResult : public CMEAsyncIoResult
	{
	public:
		CMEAsyncIoNotifyResult(
			IMEAsyncIoSink* pSink,
			ME_HANDLE hInterface );

	protected:
		/**
		* 在Notify IO执行完成之后会被call
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

	typedef CMEAsyncIoNotifyResult Result;
};

ME_NAME_SPACE_END

#endif	// ME_WIN

#endif	// __ASYCH_IO_NOTIFY_H_














