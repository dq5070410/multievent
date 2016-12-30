
#ifndef __TRANSPORT_OPERATOR_BASE_H_
#define __TRANSPORT_OPERATOR_BASE_H_

#include "Base.h"
#include "TransportOperator.h"
#include "ConnectionDefines.h"
#include "ReferenceControlT.h"

ME_NAME_SPACE_BEGIN


/**
* 这玩意的存在，最根本的目的是，如果，IMETransportOperator的
* 接口增加，你只需要在这个类里增加相应的实现即可
* 而不用四处查找到底有多少个operator，然后逐个的改掉
*/
class CMETransportOperatorBase 
	: public IMETransportOperator
	, public CMEMutexTypeTraits<CMEConnDummy>
	, public CMEReferenceControlT<CMEConnDummy::MutexType>
{
public:
	virtual ~CMETransportOperatorBase();

public:
	/**
	* 在发送数据之前，你需要做什么？
	*
	* <pmbBlock>，即将被发送的数据(你可以对发送的之前的数据做一些处理，加入一些你需要的东西)
	*/
	virtual ME_Result PreSendData( CMEMessageBlock*& pmbBlock );

	/**
	* 在发送数据之前，你需要做什么？
	*
	* <pmbBlock>，在发送数据之后，你要干什么(一般来说，这个时候，需要清理一下PreSendData产生的缓存，如果有的话)
	*/
	virtual ME_Result PostSendData( CMEMessageBlock*& pmbBlock );

	/**
	* 在受到数据之后(在通知上层之前)，你需要做什么？
	*
	* <pmbBlock>，即将被推送到上层的数据(你可以对推送的之前的数据做一些处理，加入一些你需要的东西)
	*/
	virtual ME_Result PreOnReceive( CMEMessageBlock*& pmbBlock );

	virtual ME_Result PostOnReceive( CMEMessageBlock*& pmbBlock );

	/**
	* 在通知上层OnCanSendAgain之前，你需要做什么？
	*/
	virtual ME_Result PreOnCanSendAgain();

	/* 设置operator的一些属性 */
	virtual ME_Result SetOption( DWORD dwOptionType, VOID* pOptionValue );

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

protected:
	CMETransportOperatorBase();
};

ME_NAME_SPACE_END

#endif	// __TRANSPORT_OPERATOR_BASE_H_







