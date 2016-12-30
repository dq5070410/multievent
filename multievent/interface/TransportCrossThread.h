/**
* 专用于处理多线程环境的Transport
*
* 它本身不具备任何实体功能(实体功能都在原生类transport那里)
* 它只是不停的通过事件投递来控制线程切换，以确保你感觉
* 好像所有操作都是在一个线程里完成的
*
* History
*	3/1/2016		Created
*/

#ifndef __ME_TRANSPORT_CROSS_THREAD_H_
#define __ME_TRANSPORT_CROSS_THREAD_H_

#include "Base.h"
#include "ConnectionInterface.h"
#include "ConnectionDefines.h"
#include "MTSafeDestroy.h"
#include "LockThreadMutex.h"

ME_NAME_SPACE_BEGIN

/* 使用的事件预声明 */
namespace EVENT
{
	/* 到网络线程执行Open() */
	template <class TransportType> class EventOpenT;

	/* 到网络线程执行Close() */
	template <class TransportType> class EventCloseT;

	/* 到网络线程执行SendData() */
	template <class TransportType, class PayloadTypeAutoPtr> class EventSendDataT;

	/* 到用户线程执行OnDisconnect() */
	template <class TransportType> class EventOnDisconnectT;

	/* 到用户线程执行OnReceive() */
	template <class TransportType, class PayloadType> class EventOnReceiveT;

	/* 到用户线程执行OnCanSendAgain() */
	template <class TransportType> class EventOnCanSendAgainT;
}

/**
* 专用于处理多线程环境的Transport
*
* 它本身不具备任何实体功能(实体功能都在原生类transport那里)
* 它只是不停的通过事件投递来控制线程切换，以确保你感觉
* 好像所有操作都是在一个线程里完成的
*/
class CMETransportCrossThread 
	: public IMETransport
	, public IMETransportSink
	, public CMEMutexTypeTraits<CMEConnDummy>
	, public CMEReferenceControlT<CMEConnDummy::MutexType>
	, protected CMEMTSafeDestroy
{
public:
	typedef CMETransportCrossThread TransportType;
	typedef CMEMessageBlock PayloadType;
	typedef CMEAutoPtrT<PayloadType> PayloadTypeAutoPtr;

public:
	CMETransportCrossThread( IMETransport* pSrcTransport );

	virtual ~CMETransportCrossThread();

	/* 继承自IMETransport */
public:
	/**
	 * 打开一个流对象或绑定一个事件接收器
	 *
	 * <pSink>, 你想绑定的事件接收器
	 *
	 * 返回值:
	 *		ME_OK表示成功; 非ME_OK表示失败
	*/
	virtual ME_Result Open( IMETransportSink* pSink );

	/**
	* 关闭一个流对象
	* 一旦调用了这个方法, 那么这个流对象再也不能被使用了, 之前与之
	* 绑定的接收器也不会再接收到任何事件, 如果你还需要一个Transport,
	* 那么你只能通过Connector或者Acceptor的Open()方法, 然后等待
	* Sink的OnConnect()来获得
	*
	* <hReason>, 你为什么要关闭这个对象
	*
	* 返回值:
	*		ME_OK表示成功; 非ME_OK表示失败
	*/
	virtual ME_Result Close( ME_Result hReason = ME_OK );

	/**
	* 发送数据
	*
	* <pmbBlock>, 发送的内容
	*
	* 返回值:
	*		ME_OK表示成功; 非ME_OK表示失败
	*/
	virtual ME_Result SendData( CMEMessageBlock* pmbBlock );

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
		void* pOptionValue );

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
		void*& pOptionValue );

	/**
	* 取得当前你给transport设定的sink
	*
	* 返回值:
	*		非NULL表示成功; NULL表示失败
	*/
	virtual IMETransportSink* GetSink() const;

	/* 继承自CMEReferenceControlT */
public:
	/**
	* 增加引用计数1
	*
	* 返回值:
	*		递增之后的引用计数, DWORD
	*/
	virtual DWORD AddReference();

	/**
	* 减少引用计数1
	*
	* 返回值:
	*		递减之后的引用计数, DWORD
	*/
	virtual DWORD RemoveReference();

	/**
	* 当引用计数为0时, 这个事件将会发生
	*
	* 它默认的行为是删除自己
	* 继承类如果有特殊要求, 可以重载这个函数, 但一定要记住
	* 在执行特定逻辑之后, 不要忘了调用CMEReferenceControlT<xx>::OnReferenceRelease()
	* 或者在重载之后的OnReferenceRelease()增加delete this
	*
	* 返回值:
	*		无
	*/
	virtual void OnReferenceRelease();

	/* 从CMEMTSafeDestroy继承 */
protected:

	/*
	* 在被创建线程尝试删除对象
	* 根据当前状态不同, 处理也不同
	* 当状态处于DESTROY_STATUS_CALL_IN_CURRENT_THREAD时,
	* 函数只是向网络线程投递一个事件, 确保在网络线程里,
	* 有关于该对象的事件在真正执行删除之前, 被清空;
	* 当状态处于DESTROY_STATUS_BACK_TO_CURRENT_THREAD时,
	* 执行删除操作
	*
	* 返回值:
	*		无
	*/
	virtual void Destroy_u();

	/*
	* 在网络线程尝试删除对象
	* 这个函数并不会真正的删除对象, 只是向对象被创建的线程
	* 投递一个事件, 通知对象, 与对象相关网络线程的事件已经
	* 已经清空了, 可以删除对象了
	*
	* 返回值:
	*		无
	*/
	virtual void Destroy_n();

private:

	/*
	* 在网络线程执行Open()函数
	*
	* 返回值:
	*		ME_OK为成功; 其他为失败
	*/
	ME_Result Open_n();

	/*
	* 在网络线程执行Close()函数
	*
	* 返回值:
	*		无
	*/
	void Close_n();

	/*
	* 在网络线程执行SendData()函数
	*
	* <pmbBlock>, 需要发送的数据
	*
	* 返回值:
	*		ME_OK为成功; 其他为失败
	*/
	ME_Result SendData_n( PayloadType* pmbBlock );

	/* 继承自IMETransportSink */
public:
	/**
	* 收到数据
	*
	* <pTransport>, 哪一个流对象即哪一条连接收到了数据
	* <pmbBlock>, 收到数据的内容
	*/
	virtual void OnReceive( 
		IMETransport* pTransport,
		CMEMessageBlock* pmbBlock );

	/**
	* 连接被断开
	* 
	* <pTransport>, 哪一条连接断开了
	* <hReason>, 为什么断开了
	*/
	virtual void OnDisconnect( 
		IMETransport* pTransport,
		ME_Result hReason = ME_OK );

	/**
	* 某条连接可以重新发送数据了
	* 当数据发送特别频繁时, 由于tcp/ip buffer已经满了, 无法继续发送数据的时候,
	* Transport的SendData()函数会返回失败, 这个时候, 就不应该继续再发送了,
	* (兄弟, 记得检查SendData(), 另外如果发送失败了, 记得保存好发送失败的这个包, 准备下次再发)
	* 因为再怎么发, 都不会成功的, 你唯一需要做的就是等待这个回调
	* 当你收到这个回调的时候, 恭喜, 你又可以继续在这个连接上发送数据了
	*
	* <pTransport>, 哪一条连接可以继续发送数据了 
	* <hResult>, 为什么可以继续发送? (通常, 这里都只会返回ME_OK) 
	*
	*/
	virtual void OnCanSendAgain( 
		IMETransport* pTransport,
		ME_Result hReason = ME_OK );

private:

	/*
	* 在用户线程回调OnDisConnect()事件
	*
	* <hResult>, 什么原因导致连接断开
	*
	* 返回值:
	*		无
	*/
	void OnDisconnect_u( ME_Result hResult );

	/*
	* 在用户线程回调OnReceive()事件
	*
	* <pmbBlock>, 收到的数据
	*
	* 返回值:
	*		无
	*/
	void OnReceive_u( PayloadTypeAutoPtr& pmbBlock );

	/*
	* 在用户线程回调OnCanSendAgain()事件
	*
	* <hResult>, 结果是什么 - 一般都是ME_OK
	*
	* 返回值:
	*		无
	*/
	void OnCanSendAgain_u( ME_Result hResult = ME_OK );

private:
	IMETransportSink*					m_pSink;					// 上层的事件接收器
	CMETransportAutoPtr				m_pTransport;				// 原生类transport的智能指针
	CMELockThreadMutex					m_sinkLock;					// 对回调对象进行保护

	BOOL								m_bSendFailed;				// 是否处于发送失败状态
	BOOL								m_bNeedToCallCanSendAgain;	// 是否需要通知上层OnCanSendAgain
	CMEAtomicOperationT<CMELockMutex>	m_bClosed;					// 是否已经调用过Close()函数

private:
	typedef CMEAutoPtrT<PayloadType> ListItemType;
	typedef std::list<ListItemType> DataBufferType;
	DataBufferType						m_DataBuf;					// 用于保存处于发送失败状态时的包

	/* 事件友元类的声明 */
private:
	friend class EVENT::EventOpenT<TransportType>;
	friend class EVENT::EventCloseT<TransportType>;
	friend class EVENT::EventSendDataT<TransportType, PayloadType>;
	friend class EVENT::EventOnReceiveT<TransportType, PayloadTypeAutoPtr>;
	friend class EVENT::EventOnDisconnectT<TransportType>;
	friend class EVENT::EventOnCanSendAgainT<TransportType>;

	/* 友元事件的再定义 */
private:
	typedef EVENT::EventOpenT<TransportType> OpenNetworkEvent;
	typedef EVENT::EventCloseT<TransportType> CloseNetworkEvent;
	typedef EVENT::EventSendDataT<TransportType, PayloadType> SendDataNetworkEvent;
	typedef EVENT::EventOnReceiveT<TransportType, PayloadTypeAutoPtr> OnReceiveUserEvent;
	typedef EVENT::EventOnDisconnectT<TransportType> OnDisconnectUserEvent;
	typedef EVENT::EventOnCanSendAgainT<TransportType> OnCanSendAgainUserEvent;
};

ME_NAME_SPACE_END

#endif	// __TRANSPORT_CROSS_THREAD_H_






