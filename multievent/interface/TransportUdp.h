/**
* Transport的UDP实现类
* 
*
* History
*	12/18/2016		Created
*/

#ifndef __TRANSPORT_UDP_H_
#define __TRANSPORT_UDP_H_

#include "Base.h"
#include "TransportBase.h"
#include "InetAddress.h"

ME_NAME_SPACE_BEGIN

/**
* Transport的TCP实现类
*/
class CMETransportUdp : public CMETransportBase
{
public:
	CMETransportUdp( ME_HANDLE hHandle );

	virtual ~CMETransportUdp();

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

protected:
	CMEInetAddress					m_PeerAddress;	// 对端地址
};

ME_NAME_SPACE_END

#endif	// __TRANSPORT_UDP_H_