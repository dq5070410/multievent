/**
* 异步IO操作结果的定义
* 它是所有异步IO操作结果的基类
* 
*
* History
*	9/04/2016		Created
*/
#ifndef __ASYCH_IO_RESULT_H_
#define __ASYCH_IO_RESULT_H_

#include "Base.h"
#include "AsyncIoDefines.h"
#include "AutoPtrT.h"
#include "IO.h"

ME_NAME_SPACE_BEGIN

/**
* 所有异步IO操作结果的基类
*
* 它只有一个回调函数OnComplete()，这个函数在指定的IO执行完成之后会被call，
* 在OnComplete()函数里，它会按顺序调用OnCompleteImpl()和OnDestroy()，前者是纯虚
* 继承类必须定义，后者的默认动作是delete this，如果继承类有特殊需要，可以
* 自行实现OnDestroy()
*/
class ME_OS_EXPORT CMEAsyncIoResult : public ME_ASYCH_IO_INFO 
{
public:
	/**
	* 在指定的IO执行完成之后会被call
	*
	* <bResult>, 操作结果
	* <iActiveEvent>, 什么事件发生了
	* <hInterface>, 谁处理的IO
	*
	* 返回值:
	*		无
	*/
	void OnComplete(
		BOOL bResult,
		int iActiveEvent,
		ME_HANDLE hInterface );

protected:
	CMEAsyncIoResult(
		int iIoType,
		IMEAsyncIoSink* pSink,
		ME_HANDLE hHandle = NULL );

	virtual ~CMEAsyncIoResult();

protected:
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
		ME_HANDLE hInterface ) = 0;

	/**
	* 在指定的OnCompleteImpl()执行完成之后会被call，
	* 默认动作是delete this，如果继承类有特殊需要，可以
	* 自行实现OnDestroy()
	*
	* 返回值:
	*		无
	*/
	virtual void OnDestroy();

protected:
	CMEAutoPtrT<IMEAsyncIoSink>	m_pAioSink;	// 回调对象
};

ME_NAME_SPACE_END

#endif	// __ASYCH_IO_RESULT_H_



