/**
* Connector�Ĵ�������
*
* �����Connector����������
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
* Connector�Ĵ�������
*
* �����Connector����������
*/
class CMEConnectorFactory
{
public:
	/**
	* ����Connector
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��
	*/
	ME_Result Create( 
		DWORD dwType,
		IMEConnectorImpl*& pConnector );

	/* 2013.9.5 added by Τ�� */
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








