/**
* 专用于transport的处理器接口类
* 
*
* History
*	12/19/2016		Created
*/

#ifndef __TRANSPORT_OPERATOR_H_
#define __TRANSPORT_OPERATOR_H_

#include "HikBase.h"
#include "HikReferenceControlT.h"
#include "HikAutoPtrT.h"

ME_NAME_SPACE_BEGIN

class CMEMessageBlock;

/**
* 专用于transport的处理器接口类
*
* 这个接口类定义了在transport收发的几个过程中
* 可以插入的“行为”，即，通过它你可以定义transport
* 在几个既定的过程中所需要做的事情
*
* 处理器都是从IMEConnectionManager::CONNECTION_TYPE识别而来
* 除了CONNECTION_TYPE_TCP，CONNECTION_TYPE_UDP，CONNECTION_TYPE_KEEP_ALIVE
* 等基础类型，其他位(bit set)，都会被识别为处理器，如后继会添加的HTTP(是否使用http)
* PROXY_SOCK(是否使用sock4代理)，SSL(是否使用SSL)等等
*/
class IMETransportOperator : public IMEReferenceControl
{
public:
	enum RETURN_VALUE
	{
		RETURN_VALUE_OK = 0,

		RETURN_VALUE_AGAIN = (1 << 0),

		RETURN_VALUE_FINISH = (1 << 1),

		RETURN_VALUE_FINISH_AGAIN = RETURN_VALUE_AGAIN | RETURN_VALUE_FINISH,
	};

public:
	virtual ~IMETransportOperator() {}

public:
	/**
	* 在发送数据之前，你需要做什么？
	*
	* <pmbBlock>，即将被发送的数据(你可以对发送的之前的数据做一些处理，加入一些你需要的东西)
	*/
	virtual ME_Result PreSendData( CMEMessageBlock*& pmbBlock ) = 0;

	/**
	* 在发送数据之前，你需要做什么？
	*
	* <pmbBlock>，在发送数据之后，你要干什么(一般来说，这个时候，需要清理一下PreSendData产生的缓存，如果有的话)
	*/
	virtual ME_Result PostSendData( CMEMessageBlock*& pmbBlock ) = 0;

	/**
	* 在受到数据之后(在通知上层之前)，你需要做什么？
	*
	* <pmbBlock>，即将被推送到上层的数据(你可以对推送的之前的数据做一些处理，加入一些你需要的东西)
	*/
	virtual ME_Result PreOnReceive( CMEMessageBlock*& pmbBlock ) = 0;

	/**
	* 在受到数据之后(在通知上层之前)，你需要做什么？
	*/
	virtual ME_Result PostOnReceive( CMEMessageBlock*& pmbBlock ) = 0;

	/**
	* 在通知上层OnCanSendAgain之前，你需要做什么？
	*/
	virtual ME_Result PreOnCanSendAgain() = 0;

	virtual DWORD GetType() = 0;

	/**
	* 设置operator的一些属性
	*/
	virtual ME_Result SetOption( DWORD dwOptionType, VOID* pOptionValue ) = 0;
};

typedef CMEAutoPtrT<IMETransportOperator> IMETransportOperatorAutoPtr;

ME_NAME_SPACE_END

#endif	// __TRANSPORT_OPERATOR_H_






