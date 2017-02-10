/**
* 专用于处理多线程环境的Connector
*
* 它本身不具备任何实体功能(实体功能都在原生类Connector那里)
* 它只是不停的通过事件投递来控制线程切换，以确保你感觉
* 好像所有操作都是在一个线程里完成的
* 
*
* History
*	2/4/2016		Created
*/

#ifndef __CONNECTOR_CROSS_THREAD_H_
#define __CONNECTOR_CROSS_THREAD_H_

#include "Base.h"
#include "ConnectionInterface.h"
#include "ConnectionDefines.h"
#include "ReferenceControlT.h"
#include "TimeValue.h"
#include "InetAddress.h"
#include "MTSafeDestroy.h"

ME_NAME_SPACE_BEGIN

/* 所涉及事件的预声明 */
namespace EVENT
{
	/* 到网络线程执行Open() */
	template <class ConnectorType, class AddressType, class TimeType> class EventConnectorOpenT;

	/* 到网络线程执行Close() */
	template <class ConnectorType> class EventCloseT;

	/* 到用户线程执行OnConnect() */
	template <class ConnectorType, class TransportPtrType> class EventOnConnectUserT;
}

/**
* 专用于处理多线程环境的Connector
*
* 它本身不具备任何实体功能(实体功能都在原生类connector那里)
* 它只是不停的通过事件投递来控制线程切换，以确保你感觉
* 好像所有操作都是在一个线程里完成的
*/
class CMEConnectorCrossThread
	: public IConnector
	, public IConnectorSink
	, public CMEMutexTypeTraits<CMEConnDummy>
	, public CMEReferenceControlT<CMEConnDummy::MutexType>
	, protected CMEMTSafeDestroy
{
public:
	typedef CMEConnectorCrossThread ConnectorType;
	typedef CTimeValue TimeType;
	typedef CMEInetAddress AddressType;
	typedef CMEAutoPtrT<ITransport> TransportPtrType;

public:
	CMEConnectorCrossThread( DWORD dwType );

	virtual ~CMEConnectorCrossThread();

	/* 继承自IMEConnector */
public:
	/**
	* 打开一个Connector并且尝试连接到<peerAddress>
	* 当这个连接建立时, 会通过<pSink>的OnConnect()来通知
	*
	* <pSink>, 事件接收器
	* <pTransport>, 一个新产生的流对象 -- 对应一个连接
	* <pTimeout>, 尝试连接过多长时间没响应算超时
	* <pLocalAddress>, 需要绑定的本地地址
	*
	* 返回值:
	*		ME_OK为成功; 非ME_OK为失败
	*/
	virtual ME_Result Open(
		IConnectorSink* pSink,
		const CMEInetAddress& peerAddress,
		CTimeValue* pTimeout = NULL,
		CMEInetAddress* pLocalAddress = NULL );

	
	/**
	* 关闭一个Connector并且停止连接
	* 当这个方法被调用之后, 先前绑定的接收器不会再收到任何事件
	*
	* 返回值:
	*		无
	*/
	virtual void Close();

	/**
	* 设置connector的属性
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
	* 取得connector的属性
	*
	* <dwOptionType>, 要取得什么属性
	* <pOptionValue>, out, 对应于<dwOptionType>的值
	*
	* 返回值:
	*		ME_OK表示成功; 非ME_OK表示失败
	*/
    virtual ME_Result GetOption(
        WORD dwOptionType,
        void*& pOptionValue );

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

private:
	/**
	* 在网络线程执行Open()函数
	*
	* <peerAddress>, 需要尝试连接的对端地址
	* <pTimeout>, 连接超时时间
	* <pLocalAddress>, 需要绑定的本地地址
	*
	* 返回值:
	*		ME_OK为成功; 其他为失败
	*/
	ME_Result Open_n( 
		const CMEInetAddress& peerAddress,
		CTimeValue* pTimeout = NULL,
		CMEInetAddress* pLocalAddress = NULL );

	/**
	* 在网络线程执行Close()函数
	*
	* 返回值:
	*		无
	*/
	void Close_n();

	/* 继承自IMEConnectorSink */
public:
	/**
	 * 当你收到这个回调的时候, 说明有连接已经产生了
	 * 保存<pTransport>, 在使用<pTransport>之前, 必须调用<pTransport>的Open()
	 * 并给它指定一个事件接收器, 之后所有由<pTransport>产生的事件, 都会由这个
	 * 接收器通知上来; 如果没有指定接收器, 你将收不到任何来自对端的消息
	 *
	 * <pTransport>, 一个可用于数据发送的流对象, 是否为NULL取决于<hResult>
	 * <hResult>, 连接成功与否, ME_OK为成功, 其他为失败原因; 当ME_OK时, <pTransport>不为NULL, 其他都为NULL
	 *
	 * 返回值:
	 *		无
	*/
	virtual void OnConnect( 
		CTransportAutoPtr& pTransportAutoPtr,
		ME_Result hResult );

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
	/**
	* 在用户线程回调OnConnect()事件
	*
	* <pTransport>, 可用于发送/接收数据的transport智能指针对象
	* <hResult>, 连接的结果
	*
	* 返回值:
	*		无
	*/
	void OnConnect_u(
		TransportPtrType pTransport,
		ME_Result hResult );

	/* 事件友元类的声明 */
private:
	friend class EVENT::EventConnectorOpenT<ConnectorType, AddressType, TimeType>;
	friend class EVENT::EventCloseT<ConnectorType>;
	friend class EVENT::EventOnConnectUserT<ConnectorType, TransportPtrType>;

	/* 友元事件的再定义 */
private:
	typedef EVENT::EventConnectorOpenT<ConnectorType, AddressType, TimeType> OpenNetworkEvent;
	typedef EVENT::EventCloseT<ConnectorType> CloseNetworkEvent;
	typedef EVENT::EventOnConnectUserT<ConnectorType, TransportPtrType> OnConnectUserEvent;

private:
	IConnectorSink*				m_pSink;				// 上层给定的回调对象

	DWORD							m_dwType;				// 什么类型的connector，tcp、udp
	CConnectorAutoPtr			m_pConnectorAutoPtr;	// connector wrapper
	CTimeValue					m_htvTimeout;			// 连接超时设置
	CMEInetAddress					m_hiaPeerAddr;			// 对端地址
	CMEInetAddress					m_hiaLocalAddr;			// 需要绑定的本机地址
};



ME_NAME_SPACE_END

#endif	// __CONNECTOR_CROSS_THREAD_H_









