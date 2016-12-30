
#ifndef __TRANSPORT_OPERATOR_BASE_H_
#define __TRANSPORT_OPERATOR_BASE_H_

#include "Base.h"
#include "TransportOperator.h"
#include "ConnectionDefines.h"
#include "ReferenceControlT.h"

ME_NAME_SPACE_BEGIN


/**
* ������Ĵ��ڣ��������Ŀ���ǣ������IMETransportOperator��
* �ӿ����ӣ���ֻ��Ҫ���������������Ӧ��ʵ�ּ���
* �������Ĵ����ҵ����ж��ٸ�operator��Ȼ������ĸĵ�
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
	* �ڷ�������֮ǰ������Ҫ��ʲô��
	*
	* <pmbBlock>�����������͵�����(����ԶԷ��͵�֮ǰ��������һЩ��������һЩ����Ҫ�Ķ���)
	*/
	virtual ME_Result PreSendData( CMEMessageBlock*& pmbBlock );

	/**
	* �ڷ�������֮ǰ������Ҫ��ʲô��
	*
	* <pmbBlock>���ڷ�������֮����Ҫ��ʲô(һ����˵�����ʱ����Ҫ����һ��PreSendData�����Ļ��棬����еĻ�)
	*/
	virtual ME_Result PostSendData( CMEMessageBlock*& pmbBlock );

	/**
	* ���ܵ�����֮��(��֪ͨ�ϲ�֮ǰ)������Ҫ��ʲô��
	*
	* <pmbBlock>�����������͵��ϲ������(����Զ����͵�֮ǰ��������һЩ��������һЩ����Ҫ�Ķ���)
	*/
	virtual ME_Result PreOnReceive( CMEMessageBlock*& pmbBlock );

	virtual ME_Result PostOnReceive( CMEMessageBlock*& pmbBlock );

	/**
	* ��֪ͨ�ϲ�OnCanSendAgain֮ǰ������Ҫ��ʲô��
	*/
	virtual ME_Result PreOnCanSendAgain();

	/* ����operator��һЩ���� */
	virtual ME_Result SetOption( DWORD dwOptionType, VOID* pOptionValue );

	/* �̳���IMEReferenceControl */
public:
	/**
	* �������ü���1
	*
	* ����ֵ:
	*		����֮������ü���, DWORD
	*/
	virtual DWORD AddReference();

	/**
	* �������ü���1
	*
	* ����ֵ:
	*		�ݼ�֮������ü���, DWORD
	*/
	virtual DWORD RemoveReference();

protected:
	CMETransportOperatorBase();
};

ME_NAME_SPACE_END

#endif	// __TRANSPORT_OPERATOR_BASE_H_







