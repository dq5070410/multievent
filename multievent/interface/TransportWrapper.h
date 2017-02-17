/**
* Transport的wrapper类
* 
* 把原生类的transport与reactor对接，以及把将来可能存在
* 的各种operator(如，带长度的TCP，带keep-alive的tcp等等) 
* plug-in到transport，当然，还有作为IMETransport的adapter存在
*
*
* History
*	2/22/2016		Created
*   3/22/2016		将operator裸指针改为智能指针，operator处理改为循环遍历
*/

#ifndef __TRANSPORT_WRAPPER_H_
#define __TRANSPORT_WRAPPER_H_

#include "Base.h"
#include "ConnectionInterface.h"
#include "ConnectionDefines.h"
#include "TransportImpl.h"
#include "TransportOperator.h"
#include "LockThreadMutex.h"

ME_NAME_SPACE_BEGIN

/* 使用到的事件预声明 */
namespace EVENT
{
	/* 网络线程call DeliveryData() */
	template <class TransportType> class EventUdpDeliveryDataT;
}

/**
* Transport的wrapper类
* 
* 把原生类的transport与reactor对接，以及把将来可能存在
* 的各种operator(如，带长度的TCP，带keep-alive的tcp等等) 
* plug-in到transport，当然，还有作为IMETransport的adapter存在
*/
class CMETransportWrapper
	: public ITransport
	, protected IMETransportImplSink
	, public CMEMutexTypeTraits<CMEConnDummy>
	, public CMEReferenceControlT<CMEConnDummy::MutexType>
{
public:
	typedef CMETransportWrapper TransportType;

public:
	CMETransportWrapper();

	virtual ~CMETransportWrapper();

public:
	/**
	* 根据不同的类型初始化一个真正可用的acceptor
	*
	* <dwType>, 标识该acceptor是什么类型的, tcp/udp等等
	*
	* 返回值:
	*		ME_OK为成功; 其他失败
	*/
	ME_Result Initialize( 
		DWORD dwType,
		ME_HANDLE hHandle );

	ME_Result RegisterOperator( IMETransportOperator* pOperator );

	ME_Result UnRegisterOperator( IMETransportOperator* pOperator );

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
	virtual ME_Result Open( ITransportSink* pSink );

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
	virtual ITransportSink* GetSink() const;

	/* 继承自IMEReferenceControl */
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

public:
	/**
	* 仅仅是UDP的情况下这个方法会被call
	*/
	void OnReceive_c( 
		const char* pszData,
		UINT uLength );

	/* 继承自IMETranpsortImplSink */
protected:
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
		CMEMessageBlock* pmbData );

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
		ME_Result hReason );

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
		ME_Result hResult = ME_OK );

private:
	/**
	* 仅仅是UDP的情况下这个方法会被call
	* 
	* 当上层没有调用Open()而之前已经收到包的情况这个方法才会被调用
	* 而且，是再上层最终调用了Open()之后，它才会发生
	*/
	void DeliveryData();

	ME_Result OperatorPreSend( 
		CMEMessageBlock*& pmbBlock );

	ME_Result OperatorPostSend( 
		CMEMessageBlock*& pmbBlock );

	ME_Result OperatorPreRecv( 
		CMEMessageBlock*& pmbBlock );

	ME_Result OperatorPostRecv( 
		CMEMessageBlock*& pmbBlock );

private:
	ME_Result CreateSync( 
		DWORD dwType,
		ME_HANDLE hHandle );

	ME_Result CreateAsync(
		DWORD dwType,
		ME_HANDLE hHandle );

private:
	DWORD									m_dwType;		// 原生transport的类型
	CMEAutoPtrT<IMETransportImpl>			m_pTransport;   // 原生transport

	ITransportSink*						m_pSink;		// 上层回调对象，一般是transport cross thread或者上层使用者

// 	typedef std::map<DWORD, IMETransportOperatorAutoPtr> MapType;
// 	MapType									m_mapOperator;	// 处理器存储结构

    typedef std::deque<IMETransportOperatorAutoPtr> OperatorDequeType;
    OperatorDequeType                       m_dequeOperator;

	/**
	*
	* 用于确认是否close以保护在OnReceive() callback中可能存在的 
	* 使用者删除transport的行为
	*/
	CMEAtomicOperationT<CMELockMutex>		m_bClosed;

private:
	typedef CMEMessageBlock PayloadType;

private:
	typedef CMEAutoPtrT<PayloadType> ListItemType;
	typedef std::list<ListItemType> DataBufferType;

	/**
	* 当且仅当在以下情况使用
	*
	* udp类型且transport创建出来之后，上层没有open
	*
	* 在这种情况下，所有的包都会被存放到<m_UdpDataBuf>，且在Open之后，
	* 缓存的包会被一个个的callback上去
	*/
	DataBufferType							m_UdpDataBuf;
	CMELockThreadMutex						m_DataBufLock;

	/* 事件友元类的声明 */
private:
	friend class EVENT::EventUdpDeliveryDataT<TransportType>;

	/* 友元事件的再定义 */
private:
	typedef EVENT::EventUdpDeliveryDataT<TransportType> DeliveryDataEvent;
};

ME_NAME_SPACE_END

#endif	// __TRANSPORT_WRAPPER_H_






