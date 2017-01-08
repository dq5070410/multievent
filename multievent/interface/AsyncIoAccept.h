/**
* 异步IO操作类 - Accept动作
* 
*
* History
*	9/04/2016		Created
*/

#ifndef __ASYCH_IO_ACCEPT_H_
#define __ASYCH_IO_ACCEPT_H_

#include "Base.h"
#include "AsyncIoResult.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

/**
* 异步IO操作类 - Accept
* 
* 用于等待并获取一个新连接
*/
class ME_OS_EXPORT CMEAsyncIoAccept
{
public:
	/**
	* 投递一个Accept请求，为下一个连接到来做准备
	*
	* <pSink>, 事件接收器
	* <hListenHandle>, 已经处于侦听状态的socket句柄
	*
	* 返回值:
	*		ME_OK为成功, 其他为失败
	*/
	ME_Result Post(
		IMEAsyncIoSink* pSink,
		ME_HANDLE hListenHandle );

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
	* 异步IO操作类Accept的专用结果类
	* 
	* 当Accept操作完成时, 会透过这个类的对象进行通知
	*/
	class CMEAsyncIoAcceptResult : public CMEAsyncIoResult
	{
	public:
		CMEAsyncIoAcceptResult( 
			IMEAsyncIoSink* pSink,
			DWORD dwLocalAddrLen,
			DWORD dwRemoteAddrLen );

	protected:
		/**
		* 在Accept IO执行完成之后会被call
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

	public:
		static char					s_cBuf;				// 用于填充::AcceptEx的<lpOutputBuffer>参数，该参数不能为NULL
		DWORD						m_dwLocalAddrLen;	// 保存::AcceptEx的<dwLocalAddressLength>参数
		DWORD						m_dwRemoteAddrLen;	// 保存::AcceptEx的<dwRemoteAddressLength>参数
        BOOL                        m_bResult;          // 保存OnComplete的结果 01/26/2015 by fangyu
	};

	typedef CMEAsyncIoAcceptResult Result;
};

ME_NAME_SPACE_END

#endif	// ME_WIN

#endif	// __ASYCH_IO_ACCEPT_H_











