/**
* Connector��ʵ�ֽӿ�
*
* ������ʲô���͵�Connector��Ҫ������̳�
* 
*
* History
*	1/28/2016		Created
*/

#ifndef __CONNECTOR_IMPL_H_
#define __CONNECTOR_IMPL_H_

#include "Base.h"
#include "ReferenceControlT.h"
#include "ConnectionDefines.h"

ME_NAME_SPACE_BEGIN

class CMEReactor;
class CMESocketBase;
class CMEInetAddress;

/**
* ʵ��connectorʵ�ֶ�����¼���������
*/
class IMEConnectorImplSink : public IMEReferenceControl
{
public:
	virtual ~IMEConnectorImplSink() {}

public:

	/**
	* �����Ӳ���
	*
	* <hResult>, �Զ����ӵĽ��, һ�㶼��ME_OK
	* <pSocket>, socket����
	* 
	* ����ֵ,
	*		��
	*/
	virtual void OnConnect( 
		ME_Result hResult,
		ME_HANDLE hHandle ) = 0;
};

/**
* connector������ʵ����ӿ�
*/
class IMEConnectorImpl
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
	virtual ~IMEConnectorImpl() {}

public:

	/**
	* ��, �����ӵ�ĳһ���Ϸ���ַ
	*
	* <pSink>, �¼�������
	* <pReactor>, �¼�����������
	* <hHandle>, ���ӳɹ���ľ��
	* <peerAddress>, ���ӵ�����
	* <localAddress>, ��Ҫ�󶨵ı��ص�ַ
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�, ����Ϊʧ��
	*/
	virtual ME_Result Open( 
		IMEConnectorImplSink* pSink,
		const CMEInetAddress& peerAddress,
		CMEReactor* pReactor,
		ME_HANDLE& hHandle,
		CMEInetAddress* pLocalAddress = NULL ) = 0;

	/**
	* �ر�, ��ֹͣ����
	*
	* ����ֵ:
	*		��
	*/
	virtual void Close() = 0;
};

ME_NAME_SPACE_END

#endif	// __CONNECTOR_IMPL_H_