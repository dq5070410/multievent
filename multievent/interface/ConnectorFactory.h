/**
* Connector的创建工厂
*
* 各类的Connector都由它创建
* 
*
* History
*	1/30/2016		Created
*/

#ifndef __CONNECTOR_FACTORY_H_
#define __CONNECTOR_FACTORY_H_

#include "Base.h"
#include "LockThreadMutex.h"
#include "SingletonT.h"

ME_NAME_SPACE_BEGIN


class IMEConnectorImpl;

/**
* Connector的创建工厂
*
* 各类的Connector都由它创建
*/
class CMEConnectorFactory
{
public:
	/**
	* 创建Connector
	*
	* 返回值:
	*		ME_OK为成功; 其他为失败
	*/
	ME_Result Create( 
		DWORD dwType,
		IMEConnectorImpl*& pConnector );

	/* 2013.9.5 added by 韦珂 */
private:
	ME_Result CreateSync(
		DWORD dwType,
		IMEConnectorImpl*& pConnector );


	ME_Result CreateAsync(
		DWORD dwType,
		IMEConnectorImpl*& pConnector );
};

typedef CMESingletonT<CMEConnectorFactory> CMEConnectorFactorySingleton;


ME_NAME_SPACE_END

#endif	// __CONNECTOR_FACTORY_H_








