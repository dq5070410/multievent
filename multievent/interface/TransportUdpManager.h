/**
* ר�Ź�����CMEAcceptorUdp������transport
* 
*
* History
*	3/12/2016		Created
*/

#ifndef __TRANSPORT_UDP_MANAGER_H_
#define __TRANSPORT_UDP_MANAGER_H_

#include "Base.h"
#include "InetAddress.h"
#include "AutoPtrT.h"
#include "SocketUdp.h"
#include "SingletonT.h"
#include "LockThreadMutex.h"

#ifdef ME_WIN
#include <hash_map>

#elif defined( ME_LINUX )
#include <ext/hash_map>

#endif  // ME_WIN

ME_NAME_SPACE_BEGIN

class IMETransport;

/**
* ר�Ź�����CMEAcceptorUdp������transport
*/
class CMETransportUdpManager
{
public:
	typedef IMETransport TransportType;

public:
	CMETransportUdpManager();

	~CMETransportUdpManager();

public:
	/**
	* ����transport
	*
	* <peerAddress>������һ���Զ˵�ַ����transport
	*
	* ����ֵ��
	*		�ɹ�Ϊ��NULL��NULLΪʧ��
	*/
	TransportType* FindTransport( const CMEInetAddress& peerAddress );

	/**
	* ע��transport
	*
	* <peerAddress>������һ���Զ˵�ַע��transport
	* <pTransport>������transport
	*
	* ����ֵ��
	*		�ɹ�ΪME_OK������Ϊʧ��
	*/
	ME_Result RegisterTransport( 
		const CMEInetAddress& peerAddress,
		TransportType* pTransport );

	/**
	* ȡ��ע��transport
	*
	* <peerAddress>������һ���Զ˵�ַȡ��ע��transport
	* <pTransport>������transport
	*
	* ����ֵ��
	*		�ɹ�ΪME_OK������Ϊʧ��
	*/
	ME_Result UnRegisterTransport( 
		const CMEInetAddress& peerAddress,
		TransportType* pTransport );

public:
	/**
	* ��һ���µ����Ӳ�����
	*
	* <dwType>��ʲô���͵�����(ȡֵ��IMEConnectionManager::CONNECTION_TYPE)
	* <peerAddress>���Զ˵�ַ��ʲô
	* <pNewTransport>��out���ɹ�Ϊ��NULL
	*/
	void OnUserConnect( 
		DWORD dwType,
		const CMEInetAddress& peerAddress,
		TransportType*& pNewTransport );

	/**
	* �յ�����
	*
	* <pTransport>��ʲô�����յ�����
	* <pszData>���յ�������
	* <uLenght>�����ݵĳ���
	*/
	void OnUserDataReceive( 
		TransportType* pTransport,
		const char* pszData,
		UINT uLenght );

private:
	/**
	* ����һ��transport
	*
	* <dwType>��ʲô���͵�����(ȡֵ��IMEConnectionManager::CONNECTION_TYPE)
	* <peerAddress>���Զ˵�ַ
	* <pNewTransport>��out���ɹ�Ϊ��NULL
	*/
	void CreateTransport( 
		DWORD dwType,
		const CMEInetAddress& peerAddress,
		TransportType*& pNewTransport );


private:
	/* ����transport�Ĺ�ϣ���� */
#ifdef ME_WIN
	typedef stdext::hash_map<CMEInetAddress, TransportType*, CMEInetAddress::HashCompare> MapType;

#elif defined( ME_LINUX )
	typedef __gnu_cxx::hash_map<CMEInetAddress, TransportType*, CMEInetAddress::HashCompare> MapType;

#endif  // ME_WIN

	typedef CMELockThreadMutex MutexType;

private:

	MapType							m_TransportMap;		// ����transport�Ĺ�ϣ��
	MutexType						m_lock;

	CMESocketUdp					m_SocketUdp;		// ����set������handle��Ȼ����в�����˵���˾���������code�Ա����ظ�ʵ��
};

typedef CMESingletonT<CMETransportUdpManager> CMETransportUdpManagerSingleton;	// udp manager�ĵ���

ME_NAME_SPACE_END

#endif	// __TRANSPORT_UDP_MANAGER_H_




