/**
* Acceptor的实现接口
*
*
* History
*	12/13/2016		Created
*/

#ifndef __ACCEPTOR_IMPL_H_
#define __ACCEPTOR_IMPL_H_

#include "Base.h"
#include "ReferenceControlT.h"
#include "ConnectionDefines.h"

ME_NAME_SPACE_BEGIN

class CMEInetAddress;
class CMEReactor;
class IMEAcceptorImplSink;

/**
* Acceptor的实现接口
*
* 不管是什么类型的Acceptor都要从这里继承
*/
class IMEAcceptorImpl
	/**
	* 增加这两个继承，意在把原生类的transport/acceptor/connector纳入引入计数的控制范围
	* 一方面是为了适应异步IO的安全删除要求
	* 另一方面则是为了普通transport在多线程环境下(尤其在于上层使用MTDestroy
	* 进行删除控制时)的安全性
	*/
	: public CMEMutexTypeTraits<CMEConnDummy>
	/**
	* 这里必须用虚拟继承，防止在多重继承下，继承类的某个基类也继承了引用计数控制类
	* 从而导致引用计数有两个副本，最终在对象析构时产生不可预测的后果
	*/
	, virtual public CMEReferenceControlT<CMEConnDummy::MutexType>
{
public:
	virtual ~IMEAcceptorImpl() {}

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
		const CMEInetAddress& hiaLocalAddress ) = 0;

	/**
	* 关闭, 即停止侦听
	*
	* 返回值:
	*		无
	*/
	virtual void Close( CMEReactor* pReactor ) = 0;
};

class IMEAcceptorImplSink : public IMEReferenceControl
{
public:
	virtual ~IMEAcceptorImplSink() {}

public:

	/**
	* 有连接产生
	*
	* <hResult>, 对端连接的结果, 一般都是ME_OK
	* <hHandle>, 新产生的socket句柄
	* <pPeerAddress>, 对端地址
	* 
	* 返回值,
	*		无
	*/
	virtual void OnConnect(
		ME_Result hResult,
		ME_HANDLE hHandle,
		CMEInetAddress* pPeerAddress ) = 0;
};

class IMEAcceptorUdpImplSink : public IMEAcceptorImplSink//protected IMEAcceptorImplSink	// 不允许非继承类做type cast转换
{
public:
	virtual ~IMEAcceptorUdpImplSink() {}

public:

	/**
	* 有连接产生
	*
	* <hResult>, 对端连接的结果, 一般都是ME_OK
	* <hHandle>, UDP acceptor自身的句柄
	* 
	* 返回值,
	*		无
	*/
	virtual void OnInput(
		ME_Result hResult,
		ME_HANDLE hHandle ) = 0;
};


ME_NAME_SPACE_END


#endif	// __ACCEPTOR_IMPL_H_










