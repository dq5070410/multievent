/**
* 异步IO操作类 - Recv动作
* 
*
* History
*	9/04/2016		Created
*/
#ifndef __ASYCH_IO_RECV_H_
#define __ASYCH_IO_RECV_H_

#include "Base.h"
#include "AsyncIoResult.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

class CMEMessageBlock;

/**
* 异步IO操作类 - Recv
* 
* 用于读取一个已经“Ready”的数据块
*/
class ME_OS_EXPORT CMEAsyncIoRecv
{
public:
	/**
	* 投递一个Recv请求，为下一个到来的数据块做准备
	*
	* <pSink>, 事件接收器
	* <hRecvHandle>, 准备从哪一个socket句柄读取
	* <pmbBlock>, 预先准备好的数据存储空间(它必须是被new出来的)
	*
	* 返回值:
	*		ME_OK为成功, 其他为失败
	*/
	ME_Result Post(
		IMEAsyncIoSink* pSink,
		ME_HANDLE hRecvHandle,
		CMEMessageBlock* pmbBlock );

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
	* 异步IO操作类Recv的专用结果类
	* 
	* 当Recv操作完成时, 会透过这个类的对象进行通知
	*/
	class CMEAsyncIoRecvResult : public CMEAsyncIoResult
	{
	public:
		CMEAsyncIoRecvResult(
			IMEAsyncIoSink* pSink,
			ME_HANDLE hRecvHandle,
			CMEMessageBlock* pmbBlock );
	
		virtual ~CMEAsyncIoRecvResult();

	protected:
		/**
		* 在Recv IO执行完成之后会被call
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
		CMEAutoPtrT<CMEMessageBlock>	m_pmbBlock;	// 读取到的数据块
	};

	typedef CMEAsyncIoRecvResult Result;
};

ME_NAME_SPACE_END

#endif	// ME_WIN

#endif	// __ASYCH_IO_RECV_H_


