/**
* 用作TCP整理包的处理类
* 
*
* History
*	2/19/2016		Created
*/

#ifndef __TRANSPORT_OPERATOR_PACKAGE_H_
#define __TRANSPORT_OPERATOR_PACKAGE_H_

#include "Base.h"
#include "TransportOperatorBase.h"
#include "ConnectionDefines.h"
#include "MessageBlock.h"
#include "PDU.h"

ME_NAME_SPACE_BEGIN

/**
* 用作TCP整理包的处理类
* 
* 这个处理器会在需要发送数据的前面加入4个字节的长度标识，
* 用于保证收发包的完整性
*
* 大致结构如下：
* 
*  4 bytes  <用户数据>
* |_ _ _ _|________________________
*
* 简单来说，你发出去多长的包，收到的也是多长的包，而不会
* 受到TCP分片的干扰
*/
class CMETransportOperatorPackage : public CMETransportOperatorBase
{
public:
	CMETransportOperatorPackage();

	virtual ~CMETransportOperatorPackage();

	/* 继承自IMETransportOperator */
public:
	/**
	* 在包头加上4个字节的包长度字段 
	* 如果是大包（大于64K）则分开返回
	*
	* 返回值：
	*      RETURN_VALUE_OK：处理完成
	*      RETURN_VALUE_AGAIN：还没有处理完，还需要紧接着再一次调用此方法
	*/
	virtual ME_Result PreSendData( CMEMessageBlock*& pmbBlock );

	/**
	* 检查发送结果，如果发送失败则回退到上一次PreSendData之前（设置MessageBlock的写位置）。
	*
	* 返回值：
	*		RETURN_VALUE_OK：处理完成
	*/
	virtual ME_Result PostSendData( CMEMessageBlock*& pmbBlock );

	/**
	* 对从网络上接收到得包进行处理，主要处理分片、粘包及PreSendData的人为分包
	* 
	* 返回值：
	*		RETURN_VALUE_OK：处理完成
	*		RETURN_VALUE_AGAIN：发生了粘包，需要紧接着调用一次此方法
	*		RETURN_VALUE_FINISH：发生了分片，尚未收集到一个完整的用户包，需要等待下一个网络包的到来
	*/
	virtual ME_Result PreOnReceive( CMEMessageBlock*& pmbBlock );

	/**
	* PreOnReceive会申请内存，这里对申请内存进行回收
	*/
	virtual ME_Result PostOnReceive( CMEMessageBlock*& pmbBlock );

	virtual DWORD GetType();

private:
	CHAR* SelfInfo();

	ME_Result RecvOK();

	ME_Result RecvSlice();

	ME_Result RecvStick();

private:
	/**
	* 发送数据用，确切的说是发送大包数据会用到（大于64k）
	* 发送大包时需要不停的切换写位置，所以这里需要保存原始的写位置
	*/
	BOOL							m_bNewPackage;

	BOOL							m_bStickPackage;

	CMEMessageBlockAutoPtr			m_pMB;

    CMEMessageBlockAutoPtr         m_pmbRecvOut;

#ifdef ME_DEBUG
private:
	/* 单元测试友元类 */
	friend class TestTOP;
#endif

};

ME_NAME_SPACE_END

#endif	// __TRANSPORT_OPERATOR_PACKAGE_H_
