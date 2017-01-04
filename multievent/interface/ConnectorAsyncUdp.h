/**
* �첽IO���͵�udp connector
* 
*
* History
*	9/07/2016		Created
*/
#ifndef __CONNECTOR_ASYCH_UDP_H_
#define __CONNECTOR_ASYCH_UDP_H_

#include "ConnectorUdp.h"
#include "AsyncIoDefines.h"
#include "AsyncIoConnect.h"

ME_NAME_SPACE_BEGIN

/**
* �����ڵ��������ǣ�
* 1. ����ϱ���һ�£�����TCP/UDPϵ�еĶ���������ͨ��Async����
* 2. ����ԭ�е�ʵ�֣���Ȼfactory�ڹ���connectorʱ������������Ƿ����Async������������
*	 (���ܴӹ�����˵����ûʲô��ͬ����Ϊudp���������ӵĹ��̣�������ͨ���첽��һ��)
*/
class ME_OS_EXPORT CMEConnectorAsyncUdp
	: public CMEConnectorUdp
	, public IMEAsyncIoSink
{
public:
	virtual ~CMEConnectorAsyncUdp();

public:
	/**
	* input�¼�����
	*
	* <hHandle>���ĸ����������input�¼�
	* <iEvent>��������ʲô�����¼�
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnInput(
		ME_HANDLE hHandle,
		int iEvent );

	/**
	* output�¼�����
	*
	* <hHandle>���ĸ����������output�¼�
	* <iEvent>��������ʲô�����¼�
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnOutput(
		ME_HANDLE hHandle,
		int iEvent );

	/**
	* close�¼�����
	*
	* <hHandle>���ĸ����������close�¼�
	* <iEvent>��������ʲô�����¼�
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnClose(
		ME_HANDLE hHandle,
		int iEvent );
};

ME_NAME_SPACE_END

#endif	// __CONNECTOR_ASYCH_UDP_H_

















