/**
* 异步IO操作类 - RecvFrom动作(用于UDP)
* 
*
* History
*	9/04/2016		Created
*/
#ifndef __ASYCH_IO_RECVFROM_H_
#define __ASYCH_IO_RECVFROM_H_

#include "Base.h"
#include "AsyncIoResult.h"
#include "InetAddress.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

class CMEMessageBlock;
class CMEInetAddress;

/**
* 异步IO操作类 - RecvFrom
* 
* 用于等待并获取一个新连接
*/
class ME_OS_EXPORT CMEAsyncIoRecvFrom
{
public:
	/**
	* 投递一个数据读取的请求，为下一个到来的数据包(UDP)做准备
	*
	* <pSink>, 事件接收器
	* <hRecvHandle>, 从哪一个socket句柄读取
	* <pmbBlock>, 预分配的数据空间(必须是new出来的)
	* <pFromAddress>, 数据来源的对端地址
	*
	* 返回值:
	*		ME_OK为成功, 其他为失败
	*/
	ME_Result Post(
		IMEAsyncIoSink* pSink,
		ME_HANDLE hRecvHandle,
		CMEMessageBlock* pmbBlock,
		CMEInetAddress* pFromAddress );

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
	* 异步IO操作类RecvFrom的专用结果类
	* 
	* 当RecvFrom操作完成时, 会透过这个类的对象进行通知
	*/
	class CMEAsyncIoRecvFromResult : public CMEAsyncIoResult
	{
	public:
		CMEAsyncIoRecvFromResult(
			int iIoType,
			IMEAsyncIoSink* pSink,
			ME_HANDLE hRecvHandle,
			CMEMessageBlock* pmbBlock,
			CMEInetAddress* pFromAddress );

		virtual ~CMEAsyncIoRecvFromResult();

	protected:
		/**
		* 在RecvFrom IO执行完成之后会被call
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
		CMEAutoPtrT<CMEMessageBlock>	m_pmbBlock;				// 读取到的数据块
		CMEInetAddress*				m_pFromAddress;			// 对端地址
		int								m_iFromAddressSize;		// 地址长度，也就是IPv4或IPv6地址结构的长度，取决于对端是用什么类型的地址
	};

	typedef CMEAsyncIoRecvFromResult Result;
};

ME_NAME_SPACE_END

#endif	// ME_WIN

#endif	// __ASYCH_IO_RECVFROM_H_




