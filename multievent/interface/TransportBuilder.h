/**
* Transport�Ĺ�����
*
* �����߸�����designer��ָʾȥ����transport
* ��������CMETransportDesigner���Ƶġ���ʽ��������
* ֻ��builder�ľ���ʵ�ֲ��������⣬��ô������Դ�����
* �̳У��������Լ���builder
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
* transport�������ӿ���
*/
class IMETransportBuilder
{
public:
	virtual ~IMETransportBuilder() {}

public:
	/**
	* �������͹���
	*/
	virtual ME_Result Build( 
		DWORD dwType,
		ME_HANDLE hHandle ) = 0;

	/**
	* �������е�transport���й���
	*/
	virtual ME_Result Build( IMETransport* pTransport ) = 0;

	virtual ME_Result Build( const CMEInetAddress& peerAddress ) = 0;

	/**
	* ���ݴ���������
	*/
	virtual ME_Result Build( IMETransportOperator* pOperator ) = 0;

	/**
	* ��ȡ���չ���õ�transport
	*/
	virtual IMETransport* GetResult() = 0;
};

/**
* Ĭ�ϵ�transportʵ�ʹ�����
*
* ���������ִ�й��죬ͨ��������Ҫһ��Designer��
* ָ������ι���
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









