/**
* 异步IO结构的定义
* 
*
* History
*	9/04/2016		Created
*/

#ifndef _ASYCH_IO_DEFINES_H_
#define _ASYCH_IO_DEFINES_H_

#include "Base.h"
#include "IO.h"
#include "ReferenceControlT.h"
#include "ConnectionDefines.h"

ME_NAME_SPACE_BEGIN

#ifdef ME_WIN
	extern LPFN_ACCEPTEX g_lpfnAcceptEx;							// ::AcceptEx函数指针
	extern LPFN_CONNECTEX g_lpfnConnectEx;							// ::ConnectEx函数指针
	extern LPFN_GETACCEPTEXSOCKADDRS g_lpfnGetAcceptExSockaddrs;	// ::GetAcceptExSockaddrs函数指针

	const DWORD ME_MS_ACCEPTEX_FUNC_REMAIN_LENGTH = 16;			// ::AcceptEx函数需要的地址保留长度，详见MSDN
#endif	// ME_WIN
/**
* HikUtility异步IO使用的数据结构
* 
* 继承于OVERLAPPED，并加入了一些必要数据类型
*/
typedef struct tagHikAsyncIoInfo : public ME_OVERLAPPED
{
	int iIoType;			// 本次IO的类型
	ME_HANDLE hHandle;		// IO操作相关句柄
} ME_ASYCH_IO_INFO;

/**
* 由于异步IO的特殊性(投递/完成/通知是异步的，不是立即完成)
* 为确保回调对象的安全，因此特地新增了这么一个类，在原有的IMEIOSink
* 的基础上加入了引用计数(继承于CMEReferenceControlT)
*
* 也正是因为加入了引用计数控制，其继承类的使用者要注意 :
* 不要直接用delete
*/
class IMEAsyncIoSink 
	: public IMEIOSink
	, public CMEMutexTypeTraits<CMEConnDummy>
	, virtual public CMEReferenceControlT<CMELockMutex>
{};

ME_NAME_SPACE_END

#endif	// _ASYCH_IO_DEFINES_H_


