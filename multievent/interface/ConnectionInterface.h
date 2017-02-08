/**
* 网络连接框架接口
* 
*
* History
*	12/13/2016		Created
*/

#ifndef __CONNECTION_INTERFACE_H_
#define __CONNECTION_INTERFACE_H_

#include "Base.h"
#include "AutoPtrT.h"
#include "ReferenceControlT.h"

ME_NAME_SPACE_BEGIN

/* 预声明 */
class CMEMessageBlock;
class CTimeValue;
class IAcceptor;
class IAcceptorSink;
class IConnector;
class IConnectorSink;
class ITransport;
class ITransportSink;
class CMEInetAddress;

typedef CMEAutoPtrT<IAcceptor> CAcceptorAutoPtr;
typedef CMEAutoPtrT<IConnector> CConnectorAutoPtr;
typedef CMEAutoPtrT<ITransport> CTransportAutoPtr;

class ME_OS_EXPORT IConnectionManager
{
public:
	virtual ~IConnectionManager() {}

public:
	/* 连接的类型以及属性 */
	enum CONNECTION_TYPE
	{
		CONNECTION_TYPE_NONE = 0,

		/* 纯TCP */
		CONNECTION_TYPE_TCP = (1 << 0),

		/* 纯UDP */
		CONNECTION_TYPE_UDP = (1 << 1),

		/* 使连接带有数据整理属性, 即每一个送上去的包都是完整的, 支持TCP */
		CONNECTION_TYPE_LENGTH = (1 << 16),

		/* 自动检测该连接是否还是可用的, 支持TCP/UDP */
		CONNECTION_TYPE_KEEP_ALIVE = (1 << 17),

        /* SSL, 支持TCP */
        CONNECTION_TYPE_SSL = (1 << 18),
	};

public:
	static IConnectionManager* Instance();

public:

	/**
	* 创建一个客户端
	* 
	* <dwType>, 什么协议的客户端, 类型必须从ME_CONNECTION_TYPE中或的方式取值
	* <pConnector>, out, 传入的连接器智能指针对象
	*
	* 返回值:
	*		ME_OK为成功; 其他为失败
	*/
	virtual ME_Result CreateClient(
		DWORD dwType,
		CConnectorAutoPtr& pConnector ) = 0;

	/**
	* 创建一个服务器端
	* 
	* <dwType>, 什么协议的服务器端, 类型必须从ME_CONNECTION_TYPE中或的方式取值
	* <pAcceptor>, out, 传入的连接器智能指针对象
	*
	* 返回值:
	*		ME_OK为成功; 其他为失败
	*/
	virtual ME_Result CreateServer(
		DWORD dwType,
		CAcceptorAutoPtr& pAcceptor ) = 0;
};

/**
 * 主动侦听的接口类
 * 侦听时, 如果没有指定绑定的地址, 而且机器上有多块网卡的话,
 * 那么最终将默认选择第一块网卡的第一个地址作为侦听地址
*/
class ME_OS_EXPORT IAcceptor : public IMEReferenceControl
{
public:
	virtual ~IAcceptor() {}

public:

	/**
	* 打开一个Acceptor并且侦听<hiaLocalAddr>
	* 当有新的连接建立时, 会通过<pSink>的OnConnect()来通知
	*
	* <pSink>, 事件接收器
	* <pTransport>, 一个新产生的流对象 -- 对应一个连接
	*
	* 返回值:
	*		无
	*/
	virtual ME_Result Open( 
		IAcceptorSink* pSink,
		const CMEInetAddress& hiaLocalAddr ) = 0;

	/**
	* 关闭一个Acceptor并且停止侦听
	* 当这个方法被调用之后, 先前绑定的接收器不会再收到任何事件
	*
	* <hReason>, 关闭的原因是什么
	*
	* 返回值:
	*		无
	*/
	virtual void Close( ME_Result hReason = ME_OK ) = 0;

	/**
	* 设置acceptor的属性
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
	* 取得acceptor的属性
	*
	* <dwOptionType>, 要取得什么属性
	* <pOptionValue>, out, 对应于<dwOptionType>的值
	*
	* 返回值:
	*		ME_OK表示成功; 非ME_OK表示失败
	*/
    virtual ME_Result GetOption(
        WORD dwOptionType,
        void*& pOptionValue ) = 0;
};

/**
 * IMEAcceptor的事件接收类
 * 如果你想收到由IMEAcceptor产生的事件, 比如, 有连接连进来了, 等等
 * 就必须继承这个类, 并实现相关函数
*/
class ME_OS_EXPORT IAcceptorSink
{
public:
	virtual ~IAcceptorSink() {}

public:

	/**
	* 当你收到这个回调的时候, 说明有连接已经产生了
	* 保存<pTransport>, 在使用之前, 必须调用<pTransport>的Open()
	* 并给它指定一个事件接收器, 之后所有由<pTransport>产生的
	* 事件, 都会由这个接收器通知上来
	*
	* <pTransport>, 一个新产生的流对象 -- 对应一个连接
	*
	* 返回值:
	*		无
	*/
	virtual void OnConnect( CTransportAutoPtr& pTransportAutoPtr ) = 0;
};

/**
* 主动连接的接口类
* 连接时, 如果没有指定绑定的地址, 而且机器上有多块网卡的话,
* 那么最终将默认选择第一块网卡的第一个地址作为侦听地址
*/
class ME_OS_EXPORT IConnector : public IMEReferenceControl
{
public:
	virtual ~IConnector() {}

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
		CMEInetAddress* pLocalAddress = NULL ) = 0;

	
	/**
	* 关闭一个Connector并且停止连接
	* 当这个方法被调用之后, 先前绑定的接收器不会再收到任何事件
	*
	* 返回值:
	*		无
	*/
	virtual void Close() = 0;

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
        void* pOptionValue ) = 0;

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
        void*& pOptionValue ) = 0;
};

/**
* IConnector的事件接收类
* 如果你想收到由IConnector产生的事件, 比如, 有连接连进来了, 等等
* 就必须继承这个类, 并实现相关函数
*/
class ME_OS_EXPORT IConnectorSink
{
public:
	virtual ~IConnectorSink() {}

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
		ME_Result hResult ) = 0;
};

/**
* 传输接口类
*/
class ME_OS_EXPORT ITransport : public IMEReferenceControl
{
public:
	enum OPTION_TYPE
	{
		/* 最小值, 用于边界检查 */
		OPTION_TYPE_NONE = 100,

		/* 获取已经绑定的本地地址 */
		OPTION_TYPE_LOCAL_ADDRESS,

		/* 获取远端的地址 */
		OPTION_TYPE_PEER_ADDRESS,

		/* 获知当前属于该连接的socket是否还可用 */
		OPTION_TYPE_SOCKET_STILL_ALIVE,

		/* 获取当前连接的类型 */
		OPTION_TYPE_TRANSPORT_TYPE,

		/* Disable或Enable TCP keepalive */
		OPTION_TYPE_TCP_KEEP_AVLIE,

		/* 获取当前连接发送缓冲区大小 */
		OPTION_TYPE_SEND_BUF_LENGTH,

		/* 获取当前接收发送缓冲区大小 */
		OPTION_TYPE_RECEIVE_BUF_LENGTH,

		/* 获取当前连接所拥有的socket对象 */
		OPTION_TYPE_GET_SOCKET,

		/* 连接检查间隔（在创建Server、Client时需使用CONNECTION_TYPE_KEEP_ALIVE选项） */
		OPTION_TYPE_SET_KEEPALIVE_INTERVEL,

		/* 连接检查次数（在创建Server、Client时需使用CONNECTION_TYPE_KEEP_ALIVE选项） */
		OPTION_TYPE_SET_KEEPALIVE_CHECKNUM,

        /* SSL对象（内部设置完成握手的SSL对象时使用） */
        OPTION_TYPE_SET_SSL,
	};

public:
	virtual ~ITransport() {}

public:
	/**
	 * 打开一个流对象或绑定一个事件接收器
	 *
	 * <pSink>, 你想绑定的事件接收器
	 *
	 * 返回值:
	 *		ME_OK表示成功; 非ME_OK表示失败
	*/
	virtual ME_Result Open( ITransportSink* pSink ) = 0;

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
	virtual ME_Result Close( ME_Result hReason = ME_OK ) = 0;

	/**
	* 发送数据
	*
	* <pmbBlock>, 发送的内容
	*
	* 返回值:
	*		ME_OK表示成功; 非ME_OK表示失败
	*/
	virtual ME_Result SendData( CMEMessageBlock* pmbBlock ) = 0;

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

	/**
	* 取得当前你给transport设定的sink
	*
	* 返回值:
	*		非NULL表示成功; NULL表示失败
	*/
	virtual ITransportSink* GetSink() const = 0;
};

class ME_OS_EXPORT ITransportSink
{
public:
	virtual ~ITransportSink() {}

public:
	/**
	* 收到数据
	*
	* <pTransport>, 哪一个流对象即哪一条连接收到了数据
	* <pmbBlock>, 收到数据的内容
	*/
	virtual void OnReceive( 
		ITransport* pTransport,
		CMEMessageBlock* pmbBlock ) = 0;

	/**
	* 连接被断开
	* 
	* <pTransport>, 哪一条连接断开了
	* <hResult>, 为什么断开了
	*/
	virtual void OnDisconnect( 
		ITransport* pTransport,
		ME_Result hResult = ME_OK ) = 0;

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
		ITransport* pTransport,
		ME_Result hReason = ME_OK ) = 0;
};

ME_NAME_SPACE_END

#endif	// __CONNECTION_INTERFACE_H_







