/**
* Transport��ʵ�ֽӿ�
*
* ������ʲô���͵�Transport��Ҫ������̳�
* 

*
* History
*	12/17/2016		Created
*/
#ifndef __TRANSPORT_IMPL_H_
#define __TRANSPORT_IMPL_H_

#include "HikBase.h"

#include "ReferenceControlT.h"
#include "ConnectionDefines.h"

ME_NAME_SPACE_BEGIN

class CMEReactor;
class CMEMessageBlock;

class IMETransportImplSink;

class IMETransportImpl
	/**
	* �����������̳У����ڰ�ԭ�����transport/acceptor/connector������������Ŀ��Ʒ�Χ
	* һ������Ϊ����Ӧ�첽IO�İ�ȫɾ��Ҫ��
	* ��һ��������Ϊ����ͨtransport�ڶ��̻߳�����(���������ϲ�ʹ��MTDestroy
	* ����ɾ������ʱ)�İ�ȫ��
	*/
	: public CMEMutexTypeTraits<CMEConnDummy>
	/**
	* �������������̳У���ֹ�ڶ��ؼ̳��£��̳����ĳ������Ҳ�̳������ü���������
	* �Ӷ��������ü��������������������ڶ�������ʱ��������Ԥ��ĺ��
	*/
	, virtual public CMEReferenceControlT<CMEConnDummy::MutexType>
{
public:
	virtual ~IMETransportImpl() {}

public:
	/**
	 * ��һ����������һ���¼�������
	 *
	 * <pReactor>, ʹ���ĸ�reactor
	 * <pSink>, ����󶨵��¼�������
	 *
	 * ����ֵ:
	 *		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result Open( 
		CMEReactor* pReactor,
		IMETransportImplSink* pSink ) = 0;

	/**
	* �ر�һ��������
	* һ���������������, ��ô�����������Ҳ���ܱ�ʹ����, ֮ǰ��֮
	* �󶨵Ľ�����Ҳ�����ٽ��յ��κ��¼�, ����㻹��Ҫһ��Transport,
	* ��ô��ֻ��ͨ��Connector����Acceptor��Open()����, Ȼ��ȴ�
	* Sink��OnConnect()�����
	*
	* <pReactor>, ʹ���ĸ�reactor
	* <hReason>, ��ΪʲôҪ�ر��������
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result Close( 
		CMEReactor* pReactor,
		ME_Result hReason = ME_OK ) = 0;

	/**
	* ��������
	*
	* <pReactor>, ʹ���ĸ�reactor
	* <pmbBlock>, ���͵�����
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result SendData( 
		CMEReactor* pReactor,
		CMEMessageBlock* pmbBlock ) = 0;

	/**
	* ����transport������
	*
	* <dwOptionType>, Ҫ����ʲô����
	* <pOptionValue>, ��Ӧ��<dwOptionType>��ֵ
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result SetOption(
		DWORD dwOptionType,
		void* pOptionValue ) = 0;

	/**
	* ȡ��transport������
	*
	* <dwOptionType>, Ҫȡ��ʲô����
	* <pOptionValue>, out, ��Ӧ��<dwOptionType>��ֵ
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result GetOption(
		DWORD dwOptionType,
		void*& pOptionValue ) = 0;
};

class IMETransportImplSink : public IMEReferenceControl
{
public:
	virtual ~IMETransportImplSink() {}

public:
	/**
	* �յ�����
	*
	* <pTransport>���ĸ�transport�յ�����
	* <pmbData>���յ�������
	*
	* ����ֵ��
	*		��
	*/
	virtual void OnReceive( 
		IMETransportImpl* pTransport,
		CMEMessageBlock* pmbData ) = 0;

	/**
	* ���ӶϿ�
	*
	* <pTransport>���ĸ�transport�Ͽ�
	* <hReason>���Ͽ���ԭ����ʲô
	*
	* ����ֵ��
	*		��
	*/
	virtual void OnDisconnect( 
		IMETransportImpl* pTransport,
		ME_Result hReason ) = 0;

	/**
	* �յ�����
	*
	* <pTransport>���ĸ�transport�ܹ����·���������
	* <hResult>��ԭ����ʲô
	*
	* ����ֵ��
	*		��
	*/
	virtual void OnCanSendAgain(
		IMETransportImpl* pTransport,
		ME_Result hResult = ME_OK ) = 0;
};

ME_NAME_SPACE_END

#endif	// __TRANSPORT_IMPL_H_

