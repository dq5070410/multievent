/**
* ר����transport�Ĵ������ӿ���
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
* ר����transport�Ĵ������ӿ���
*
* ����ӿ��ඨ������transport�շ��ļ���������
* ���Բ���ġ���Ϊ��������ͨ��������Զ���transport
* �ڼ����ȶ��Ĺ���������Ҫ��������
*
* ���������Ǵ�IMEConnectionManager::CONNECTION_TYPEʶ�����
* ����CONNECTION_TYPE_TCP��CONNECTION_TYPE_UDP��CONNECTION_TYPE_KEEP_ALIVE
* �Ȼ������ͣ�����λ(bit set)�����ᱻʶ��Ϊ�����������̻���ӵ�HTTP(�Ƿ�ʹ��http)
* PROXY_SOCK(�Ƿ�ʹ��sock4����)��SSL(�Ƿ�ʹ��SSL)�ȵ�
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
	* �ڷ�������֮ǰ������Ҫ��ʲô��
	*
	* <pmbBlock>�����������͵�����(����ԶԷ��͵�֮ǰ��������һЩ��������һЩ����Ҫ�Ķ���)
	*/
	virtual ME_Result PreSendData( CMEMessageBlock*& pmbBlock ) = 0;

	/**
	* �ڷ�������֮ǰ������Ҫ��ʲô��
	*
	* <pmbBlock>���ڷ�������֮����Ҫ��ʲô(һ����˵�����ʱ����Ҫ����һ��PreSendData�����Ļ��棬����еĻ�)
	*/
	virtual ME_Result PostSendData( CMEMessageBlock*& pmbBlock ) = 0;

	/**
	* ���ܵ�����֮��(��֪ͨ�ϲ�֮ǰ)������Ҫ��ʲô��
	*
	* <pmbBlock>�����������͵��ϲ������(����Զ����͵�֮ǰ��������һЩ��������һЩ����Ҫ�Ķ���)
	*/
	virtual ME_Result PreOnReceive( CMEMessageBlock*& pmbBlock ) = 0;

	/**
	* ���ܵ�����֮��(��֪ͨ�ϲ�֮ǰ)������Ҫ��ʲô��
	*/
	virtual ME_Result PostOnReceive( CMEMessageBlock*& pmbBlock ) = 0;

	/**
	* ��֪ͨ�ϲ�OnCanSendAgain֮ǰ������Ҫ��ʲô��
	*/
	virtual ME_Result PreOnCanSendAgain() = 0;

	virtual DWORD GetType() = 0;

	/**
	* ����operator��һЩ����
	*/
	virtual ME_Result SetOption( DWORD dwOptionType, VOID* pOptionValue ) = 0;
};

typedef CMEAutoPtrT<IMETransportOperator> IMETransportOperatorAutoPtr;

ME_NAME_SPACE_END

#endif	// __TRANSPORT_OPERATOR_H_






