/**
* 异步IO操作类 - SendTo动作(UDP)
* 
*
* History
*	9/04/2016		Created
*/

#ifndef __ASYCH_IO_SENDTO_H_
#define __ASYCH_IO_SENDTO_H_

#include "Base.h"
#include "AsyncIoResult.h"
#include "LockThreadMutex.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

class CMEMessageBlock;
class CMEInetAddress;

/**
* 异步IO操作类 - SendTo(UDP)
* 
* 向特定的地址发送数据
*/
class ME_OS_EXPORT CMEAsyncIoSendTo
{
public:
	/**
	* 投递一个Send请求，准备发送数据
	*
	* <pSink>, 事件接收器
	* <hSendHandle>, 使用哪一个socket句柄进行数据发送
	* <pmbBlock>, 准备发送的数据
	* <pToAddress>, 向什么地址发送
	*
	* 返回值:
	*		ME_OK为成功, 其他为失败
	*/
	ME_Result Post(
		IMEAsyncIoSink* pSink,
		ME_HANDLE hSendHandle,
		CMEMessageBlock* pmbBlock,
		CMEInetAddress* pToAddress );

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
	class CMEAsyncIoSendToResult : public CMEAsyncIoResult
	{
	public:
		CMEAsyncIoSendToResult(
			IMEAsyncIoSink* pSink,
			ME_HANDLE hRecvHandle,
			CMEMessageBlock* pmbBlock,
			CMEInetAddress* pToAddress );

		virtual ~CMEAsyncIoSendToResult();

	protected:
		/**
		* 在SendTo IO执行完成之后会被call
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
		CMEAutoPtrT<CMEMessageBlock>	m_pmbBlock;		// 需要发送的数据块
		CMEInetAddress*				m_pToAddress;	// 往什么地方发送

		CMELockThreadMutex				m_Lock;			// 用于保护以下这两个变量
		int								m_iLastResult;	// 操作动作结果，如WSASend函数的返回值
		DWORD							m_dwLastError;	// 操作动作的系统错误提示，如997(WSA_IO_PENDING)等等
	};

	typedef CMEAsyncIoSendToResult Result;
};

ME_NAME_SPACE_END

#endif	// ME_WIN

#endif	// __ASYCH_IO_SENDTO_H_






























