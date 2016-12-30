/**
* Transport的基础实现类
*
* 如果没有其他特殊需求，其他类型的transport最好都
* 从这里继承
* 
*
* History
*	12/18/2016		Created
*/

#ifndef __TRANSPORT_BASE_H_
#define __TRANSPORT_BASE_H_

#include "Base.h"
#include "TransportImpl.h"
#include "IO.h"
#include "AutoPtrT.h"

ME_NAME_SPACE_BEGIN

class CMESocketBase;
class CMEReactor;

/**
* Transport的基础实现类
*
* 如果没有其他特殊需求，其他类型的transport最好都
* 从这里继承
*/
class ME_OS_EXPORT CTransportBase 
	: public ITransportImpl
	, public IIOSink
	, public IIO
{
public:
	typedef ITransportImplSink TransportSinkType;

public:
	virtual ~CTransportBase();

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
		CReactor* pReactor,
		ITransportImplSink* pSink );

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
		CReactor* pReactor,
		ME_Result hReason = ME_OK );

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

public:
	virtual void SetHandle( ME_HANDLE hHandle );

	ME_HANDLE GetHandle() const;

	/* 继承自IMEIO */
protected:
	/**
	* input事件发生
	*
	* <hHandle>，哪个句柄发生了input事件
	* <iEvent>，具体是什么样的事件
	*
	* 返回值:
	*		无
	*/
	virtual void OnInput(
		ME_HANDLE hHandle,
		int iEvent );

	/**
	* output事件发生
	*
	* <hHandle>，哪个句柄发生了output事件
	* <iEvent>，具体是什么样的事件
	*
	* 返回值:
	*		无
	*/
	virtual void OnOutput(
		ME_HANDLE hHandle,
		int iEvent );

	/**
	* close事件发生
	*
	* <hHandle>，哪个句柄发生了close事件
	* <iEvent>，具体是什么样的事件
	*
	* 返回值:
	*		无
	*/
	virtual void OnClose(
		ME_HANDLE hHandle,
		int iEvent );

protected:
	CTransportBase( 
		DWORD dwType, 
		ME_HANDLE hSocketHandle );

protected:
	void SetBuffSize();

protected:
	/**
	* 清空tcp/ip栈中可能存在的缓存(还没有来得及上送的数据)
	*
	* <dwOptionType>, 要取得什么属性
	* <pOptionValue>, out, 对应于<dwOptionType>的值
	*
	* 返回值:
	*		ME_OK表示成功; 非ME_OK表示失败
	*/
	virtual DWORD CheckUnReadDataImpl() = 0;

	/**
	* 从socket读取数据
	*
	* <pszData>, 所读取的数据
	* <dwDataSize>, <pszData>的长度
	*
	* 返回值:
	*		实际读取的数据长度; -1为读取失败
	*/
	virtual int ReceiveImpl(
		char*& pszData,
		DWORD dwDataSize ) = 0;

protected:
	TransportSinkType*				m_pSink;
	
	/**
	* 这个变量用于保存当前操作使用的reactor实例
	* 
	* 它的存在，使得每一个操作都可以在不同的reactor注册
	* 鉴于reactor与线程的关系，这相当于间接的让transport
	* 的操作可以在不同的线程中进行，它更多是为HikUtility
	* 后继的并发框架而准备的，尽管它在这里出现的确给了你
	* 上述的操作可能，但这并不意味着你必须这么用它
	*
	* 同样，由于这个特点，这使得多线程间操作的序列化变得
	* 非常的复杂，如果你不是特别的熟悉HikUtility以及多线程
	* 控制能力极端出色，我建议你不要轻易的尝试这种方式
	* 
	* 最安全的方法是，对于同一个transport实例，每一个操作
	* 都是用同一个reactor
	*/
	CMEReactor*					m_pReactor;
	int								m_iIndex;

	CMESocketBase*					m_pSocket;
};

ME_NAME_SPACE_END

#endif	// __TRANSPORT_BASE_H_








