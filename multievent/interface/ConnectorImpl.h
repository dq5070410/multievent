/**
* Connector的实现接口
*
* 不管是什么类型的Connector都要从这里继承
* 
*
* History
*	1/28/2016		Created
*/

#ifndef __CONNECTOR_IMPL_H_
#define __CONNECTOR_IMPL_H_

#include "Base.h"
#include "ReferenceControlT.h"
#include "ConnectionDefines.h"

ME_NAME_SPACE_BEGIN

class CMEReactor;
class CMESocketBase;
class CMEInetAddress;

/**
* 实际connector实现对象的事件接收器类
*/
class IMEConnectorImplSink : public IMEReferenceControl
{
public:
	virtual ~IMEConnectorImplSink() {}

public:

	/**
	* 有连接产生
	*
	* <hResult>, 对端连接的结果, 一般都是ME_OK
	* <pSocket>, socket对象
	* 
	* 返回值,
	*		无
	*/
	virtual void OnConnect( 
		ME_Result hResult,
		ME_HANDLE hHandle ) = 0;
};

/**
* connector的真正实现类接口
*/
class IMEConnectorImpl
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
	virtual ~IMEConnectorImpl() {}

public:

	/**
	* 打开, 即连接到某一个合法地址
	*
	* <pSink>, 事件接收器
	* <pReactor>, 事件分离器对象
	* <hHandle>, 连接成功后的句柄
	* <peerAddress>, 连接到哪里
	* <localAddress>, 需要绑定的本地地址
	*
	* 返回值:
	*		ME_OK为成功, 其他为失败
	*/
	virtual ME_Result Open( 
		IMEConnectorImplSink* pSink,
		const CMEInetAddress& peerAddress,
		CMEReactor* pReactor,
		ME_HANDLE& hHandle,
		CMEInetAddress* pLocalAddress = NULL ) = 0;

	/**
	* 关闭, 即停止连接
	*
	* 返回值:
	*		无
	*/
	virtual void Close() = 0;
};

ME_NAME_SPACE_END

#endif	// __CONNECTOR_IMPL_H_