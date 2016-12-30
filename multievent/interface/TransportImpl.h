/**
* Transport的实现接口
*
* 不管是什么类型的Transport都要从这里继承
* 

*
* History
*	12/17/2016		Created
*/
#ifndef __TRANSPORT_IMPL_H_
#define __TRANSPORT_IMPL_H_

#include "HikBase.h"

#include "ReferenceControlT.h"
#include "ConnectionDefines.h"

ME_NAME_SPACE_BEGIN

class CMEReactor;
class CMEMessageBlock;

class IMETransportImplSink;

class IMETransportImpl
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
	virtual ~IMETransportImpl() {}

public:
	/**
	 * 打开一个流对象或绑定一个事件接收器
	 *
	 * <pReactor>, 使用哪个reactor
	 * <pSink>, 你想绑定的事件接收器
	 *
	 * 返回值:
	 *		ME_OK表示成功; 非ME_OK表示失败
	*/
	virtual ME_Result Open( 
		CMEReactor* pReactor,
		IMETransportImplSink* pSink ) = 0;

	/**
	* 关闭一个流对象
	* 一旦调用了这个方法, 那么这个流对象再也不能被使用了, 之前与之
	* 绑定的接收器也不会再接收到任何事件, 如果你还需要一个Transport,
	* 那么你只能通过Connector或者Acceptor的Open()方法, 然后等待
	* Sink的OnConnect()来获得
	*
	* <pReactor>, 使用哪个reactor
	* <hReason>, 你为什么要关闭这个对象
	*
	* 返回值:
	*		ME_OK表示成功; 非ME_OK表示失败
	*/
	virtual ME_Result Close( 
		CMEReactor* pReactor,
		ME_Result hReason = ME_OK ) = 0;

	/**
	* 发送数据
	*
	* <pReactor>, 使用哪个reactor
	* <pmbBlock>, 发送的内容
	*
	* 返回值:
	*		ME_OK表示成功; 非ME_OK表示失败
	*/
	virtual ME_Result SendData( 
		CMEReactor* pReactor,
		CMEMessageBlock* pmbBlock ) = 0;

	/**
	* 设置transport的属性
	*
	* <dwOptionType>, 要设置什么属性
	* <pOptionValue>, 对应于<dwOptionType>的值
	*
	* 返回值:
	*		ME_OK表示成功; 非ME_OK表示失败
	*/
	virtual ME_Result SetOption(
		DWORD dwOptionType,
		void* pOptionValue ) = 0;

	/**
	* 取得transport的属性
	*
	* <dwOptionType>, 要取得什么属性
	* <pOptionValue>, out, 对应于<dwOptionType>的值
	*
	* 返回值:
	*		ME_OK表示成功; 非ME_OK表示失败
	*/
	virtual ME_Result GetOption(
		DWORD dwOptionType,
		void*& pOptionValue ) = 0;
};

class IMETransportImplSink : public IMEReferenceControl
{
public:
	virtual ~IMETransportImplSink() {}

public:
	/**
	* 收到数据
	*
	* <pTransport>，哪个transport收到数据
	* <pmbData>，收到的数据
	*
	* 返回值：
	*		无
	*/
	virtual void OnReceive( 
		IMETransportImpl* pTransport,
		CMEMessageBlock* pmbData ) = 0;

	/**
	* 连接断开
	*
	* <pTransport>，哪个transport断开
	* <hReason>，断开的原因是什么
	*
	* 返回值：
	*		无
	*/
	virtual void OnDisconnect( 
		IMETransportImpl* pTransport,
		ME_Result hReason ) = 0;

	/**
	* 收到数据
	*
	* <pTransport>，哪个transport能够重新发送数据了
	* <hResult>，原因是什么
	*
	* 返回值：
	*		无
	*/
	virtual void OnCanSendAgain(
		IMETransportImpl* pTransport,
		ME_Result hResult = ME_OK ) = 0;
};

ME_NAME_SPACE_END

#endif	// __TRANSPORT_IMPL_H_

