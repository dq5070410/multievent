/**
* Acceptor��ʵ�ֽӿ�
*
*
* History
*	12/13/2016		Created
*/

#ifndef __ACCEPTOR_IMPL_H_
#define __ACCEPTOR_IMPL_H_

#include "Base.h"
#include "ReferenceControlT.h"
#include "ConnectionDefines.h"

ME_NAME_SPACE_BEGIN

class CMEInetAddress;
class CMEReactor;
class IMEAcceptorImplSink;

/**
* Acceptor��ʵ�ֽӿ�
*
* ������ʲô���͵�Acceptor��Ҫ������̳�
*/
class IMEAcceptorImpl
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
	virtual ~IMEAcceptorImpl() {}

public:
	/**
	* ��, �������뱾��ĳ���˿�
	*
	* <pSink>, �¼�������
	* <pReactor>, �¼�����������
	* <hiaLocalAddress>, ��Ҫ�󶨵ı��ص�ַ
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�, ����Ϊʧ��
	*/
	virtual ME_Result Open( 
		IMEAcceptorImplSink* pSink,
		CMEReactor* pReactor,
		const CMEInetAddress& hiaLocalAddress ) = 0;

	/**
	* �ر�, ��ֹͣ����
	*
	* ����ֵ:
	*		��
	*/
	virtual void Close( CMEReactor* pReactor ) = 0;
};

class IMEAcceptorImplSink : public IMEReferenceControl
{
public:
	virtual ~IMEAcceptorImplSink() {}

public:

	/**
	* �����Ӳ���
	*
	* <hResult>, �Զ����ӵĽ��, һ�㶼��ME_OK
	* <hHandle>, �²�����socket���
	* <pPeerAddress>, �Զ˵�ַ
	* 
	* ����ֵ,
	*		��
	*/
	virtual void OnConnect(
		ME_Result hResult,
		ME_HANDLE hHandle,
		CMEInetAddress* pPeerAddress ) = 0;
};

class IMEAcceptorUdpImplSink : public IMEAcceptorImplSink//protected IMEAcceptorImplSink	// ������Ǽ̳�����type castת��
{
public:
	virtual ~IMEAcceptorUdpImplSink() {}

public:

	/**
	* �����Ӳ���
	*
	* <hResult>, �Զ����ӵĽ��, һ�㶼��ME_OK
	* <hHandle>, UDP acceptor����ľ��
	* 
	* ����ֵ,
	*		��
	*/
	virtual void OnInput(
		ME_Result hResult,
		ME_HANDLE hHandle ) = 0;
};


ME_NAME_SPACE_END


#endif	// __ACCEPTOR_IMPL_H_










