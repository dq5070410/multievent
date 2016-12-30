/**
* Transport工厂类
*
* 这个工厂类与其他(如acceptor/connector等等)的工厂类
* 稍有不同，它并不负责具体的设计/构建transport，因为transport
* 对象远比上述的类要复杂一些，所以，它只是把所需的designer/builder
* 实例化，让它们去构建，当然，这个工厂类可以决定使用什么样的designer/builder
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
* Transport工厂类
*
* 这个工厂类与其他(如acceptor/connector等等)的工厂类
* 稍有不同，它并不负责具体的设计/构建transport，因为transport
* 对象远比上述的类要复杂一些，所以，它只是把所需的designer/builder
* 实例化，让它们去构建，当然，这个工厂类可以决定使用什么样的designer/builder
*/
class CMETransportFactory
{
public:
	/**
	* 创建一个transport
	*
	* <dwType>，什么类型(取值于IMEConnectionManager::CONNECTION_TYPE)
	* <hHandle>，句柄
	* <pTransport>，out，成功应该是非NULL
	* <pPeerAddress>，对端地址
	*/
	ME_Result Create( 
		DWORD dwType,
		ME_HANDLE hHandle,
		IMETransport*& pTransport,
		CMEInetAddress* pPeerAddress = NULL );

	/**
	* 创建一个transport
	*
	* <pSrcTransport>，根据什么transport来创建<pTransport>
	* <pTransport>，out，成功应该是非NULL
	*/
	ME_Result Create(
		IMETransport* pSrcTransport,
		IMETransport*& pTransport );
};

typedef CMESingletonT<CMETransportFactory> CMETransportFactorySingleton;

ME_NAME_SPACE_END

#endif	// __TRANSPORT_FACTORY_H_








