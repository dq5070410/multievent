/**
* Transport������
*
* ���������������(��acceptor/connector�ȵ�)�Ĺ�����
* ���в�ͬ�������������������/����transport����Ϊtransport
* ����Զ����������Ҫ����һЩ�����ԣ���ֻ�ǰ������designer/builder
* ʵ������������ȥ��������Ȼ�������������Ծ���ʹ��ʲô����designer/builder
*
*
* History
*	12/20/2012		Created
*/

#ifndef __TRANSPORT_FACTORY_H_
#define __TRANSPORT_FACTORY_H_

#include "Base.h"
#include "LockThreadMutex.h"
#include "SingletonT.h"

ME_NAME_SPACE_BEGIN

class IMETransport;
class CMEInetAddress;

/**
* Transport������
*
* ���������������(��acceptor/connector�ȵ�)�Ĺ�����
* ���в�ͬ�������������������/����transport����Ϊtransport
* ����Զ����������Ҫ����һЩ�����ԣ���ֻ�ǰ������designer/builder
* ʵ������������ȥ��������Ȼ�������������Ծ���ʹ��ʲô����designer/builder
*/
class CMETransportFactory
{
public:
	/**
	* ����һ��transport
	*
	* <dwType>��ʲô����(ȡֵ��IMEConnectionManager::CONNECTION_TYPE)
	* <hHandle>�����
	* <pTransport>��out���ɹ�Ӧ���Ƿ�NULL
	* <pPeerAddress>���Զ˵�ַ
	*/
	ME_Result Create( 
		DWORD dwType,
		ME_HANDLE hHandle,
		IMETransport*& pTransport,
		CMEInetAddress* pPeerAddress = NULL );

	/**
	* ����һ��transport
	*
	* <pSrcTransport>������ʲôtransport������<pTransport>
	* <pTransport>��out���ɹ�Ӧ���Ƿ�NULL
	*/
	ME_Result Create(
		IMETransport* pSrcTransport,
		IMETransport*& pTransport );
};

typedef CMESingletonT<CMETransportFactory> CMETransportFactorySingleton;

ME_NAME_SPACE_END

#endif	// __TRANSPORT_FACTORY_H_








