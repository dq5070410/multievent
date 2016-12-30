/**
* Transport的构建者
*
* 构建者负责按照designer的指示去构建transport
* 如果你觉得CMETransportDesigner定制的“样式”还不错，
* 只是builder的具体实现不尽如人意，那么，你可以从这里
* 继承，构建你自己的builder
*
*
* History
*	12/20/2016		Created
*/
#ifndef __TRANSPORT_BUILDER_H_
#define __TRANSPORT_BUILDER_H_

#include "Base.h"
#include "AutoPtrT.h"

ME_NAME_SPACE_BEGIN

class IMETransport;
class IMETransportOperator;
class CMEInetAddress;

/**
* transport构造器接口类
*/
class IMETransportBuilder
{
public:
	virtual ~IMETransportBuilder() {}

public:
	/**
	* 根据类型构造
	*/
	virtual ME_Result Build( 
		DWORD dwType,
		ME_HANDLE hHandle ) = 0;

	/**
	* 根据已有的transport进行构造
	*/
	virtual ME_Result Build( IMETransport* pTransport ) = 0;

	virtual ME_Result Build( const CMEInetAddress& peerAddress ) = 0;

	/**
	* 根据处理器构造
	*/
	virtual ME_Result Build( IMETransportOperator* pOperator ) = 0;

	/**
	* 获取最终构造好的transport
	*/
	virtual IMETransport* GetResult() = 0;
};

/**
* 默认的transport实际构造者
*
* 它负责具体执行构造，通常，它需要一个Designer来
* 指导它如何构造
*/
class CMETransportBuilder : public IMETransportBuilder
{
public:
	virtual ~CMETransportBuilder();

public:
	virtual ME_Result Build( 
		DWORD dwType,
		ME_HANDLE hHandle );

	virtual ME_Result Build( IMETransport* pTransport );

	virtual ME_Result Build( const CMEInetAddress& peerAddress );

	virtual ME_Result Build( IMETransportOperator* pOperator );


	virtual IMETransport* GetResult();

protected:
	CMEAutoPtrT<IMETransport>		m_pTransport;
};

ME_NAME_SPACE_END

#endif	// __TRANSPORT_BUILDER_H_









