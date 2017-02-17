/**
* Transport的TCP实现类
* 
*
* History
*	12/18/2016		Created
*/

#ifndef __TRANSPORT_TCP_H_
#define __TRANSPORT_TCP_H_

#include "Base.h"
#include "TransportBase.h"

ME_NAME_SPACE_BEGIN

/* 使用的事件预声明 */
namespace EVENT
{
	/* 在当前的线程中callback OnDisconnect */
	template <class TransportType, class TransportSinkType> class EventOnDisconnectCurrentT;
}

/**
* Transport的TCP实现类
*/
class ME_OS_EXPORT CMETransportTcp : public CTransportBase
{
public:
	typedef CMETransportTcp TransportType;

public:
	CMETransportTcp( ME_HANDLE hHandle );

	virtual ~CMETransportTcp();

public:
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
		CMEMessageBlock* pmbBlock );

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
	virtual DWORD CheckUnReadDataImpl();

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
		DWORD dwDataSize );

public:
	void OnDisconnect_c( 
		TransportSinkType* pSink,
		ME_Result hReason );

	/* 事件友元类的声明 */
protected:
	friend class EVENT::EventOnDisconnectCurrentT<TransportType, TransportSinkType>;

	/* 友元事件的再定义 */
protected:
	typedef class EVENT::EventOnDisconnectCurrentT<TransportType, TransportSinkType> OnDisconnectUserEvent;
};

ME_NAME_SPACE_END

#endif	// __TRANSPORT_TCP_H_



