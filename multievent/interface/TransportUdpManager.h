/**
* 专门管理由CMEAcceptorUdp产生的transport
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
* 专门管理由CMEAcceptorUdp产生的transport
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
	* 查找transport
	*
	* <peerAddress>，根据一个对端地址查找transport
	*
	* 返回值：
	*		成功为非NULL；NULL为失败
	*/
	TransportType* FindTransport( const CMEInetAddress& peerAddress );

	/**
	* 注册transport
	*
	* <peerAddress>，根据一个对端地址注册transport
	* <pTransport>，哪条transport
	*
	* 返回值：
	*		成功为ME_OK；其他为失败
	*/
	ME_Result RegisterTransport( 
		const CMEInetAddress& peerAddress,
		TransportType* pTransport );

	/**
	* 取消注册transport
	*
	* <peerAddress>，根据一个对端地址取消注册transport
	* <pTransport>，哪条transport
	*
	* 返回值：
	*		成功为ME_OK；其他为失败
	*/
	ME_Result UnRegisterTransport( 
		const CMEInetAddress& peerAddress,
		TransportType* pTransport );

public:
	/**
	* 有一个新的连接产生了
	*
	* <dwType>，什么类型的连接(取值于IMEConnectionManager::CONNECTION_TYPE)
	* <peerAddress>，对端地址是什么
	* <pNewTransport>，out，成功为非NULL
	*/
	void OnUserConnect( 
		DWORD dwType,
		const CMEInetAddress& peerAddress,
		TransportType*& pNewTransport );

	/**
	* 收到数据
	*
	* <pTransport>，什么连接收到数据
	* <pszData>，收到的数据
	* <uLenght>，数据的长度
	*/
	void OnUserDataReceive( 
		TransportType* pTransport,
		const char* pszData,
		UINT uLenght );

private:
	/**
	* 创建一个transport
	*
	* <dwType>，什么类型的连接(取值于IMEConnectionManager::CONNECTION_TYPE)
	* <peerAddress>，对端地址
	* <pNewTransport>，out，成功为非NULL
	*/
	void CreateTransport( 
		DWORD dwType,
		const CMEInetAddress& peerAddress,
		TransportType*& pNewTransport );


private:
	/* 保存transport的哈希表定义 */
#ifdef ME_WIN
	typedef stdext::hash_map<CMEInetAddress, TransportType*, CMEInetAddress::HashCompare> MapType;

#elif defined( ME_LINUX )
	typedef __gnu_cxx::hash_map<CMEInetAddress, TransportType*, CMEInetAddress::HashCompare> MapType;

#endif  // ME_WIN

	typedef CMELockThreadMutex MutexType;

private:

	MapType							m_TransportMap;		// 保存transport的哈希表
	MutexType						m_lock;

	CMESocketUdp					m_SocketUdp;		// 接收set进来的handle，然后进行操作，说白了就是利用它code以避免重复实现
};

typedef CMESingletonT<CMETransportUdpManager> CMETransportUdpManagerSingleton;	// udp manager的单例

ME_NAME_SPACE_END

#endif	// __TRANSPORT_UDP_MANAGER_H_




