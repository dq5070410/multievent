/**
* ���ӹ��������ʵ��
* 
*
* History
*	12/14/2016		Created
*/

#ifndef __CONNECTION_MANAGER_H_
#define __CONNECTION_MANAGER_H_

#include "Base.h"
#include "ConnectionInterface.h"
#include "ConnectionDefines.h"

ME_NAME_SPACE_BEGIN

class ME_OS_EXPORT CMEConnectionManager
	: public IConnectionManager
	, public CMEMutexTypeTraits<CMEConnDummy>
{
public:
	static void SocketStartup();

	static void SocketCleanup();

public:
	CMEConnectionManager();

	virtual ~CMEConnectionManager();

public:
	/**
	* ����һ���ͻ���
	* 
	* <dwType>, ʲôЭ��Ŀͻ���, ���ͱ����ME_CONNECTION_TYPE�л�ķ�ʽȡֵ
	* <pConnector>, out, ���������������ָ�����
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��
	*/
	virtual ME_Result CreateClient(
		DWORD dwType,
		CConnectorAutoPtr& pConnector );

	/**
	* ����һ����������
	* 
	* <dwType>, ʲôЭ��ķ�������, ���ͱ����ME_CONNECTION_TYPE�л�ķ�ʽȡֵ
	* <pAcceptor>, out, ���������������ָ�����
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��
	*/
	virtual ME_Result CreateServer(
		DWORD dwType,
		CAcceptorAutoPtr& pAcceptor );

protected:

	/**
	* ����һ���ͻ���
	* 
	* <dwType>, ʲôЭ��Ŀͻ���, ���ͱ����ME_CONNECTION_TYPE�л�ķ�ʽȡֵ
	* <pConnector>, out, ���������������ָ�����
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��
	*/
	ME_Result CreateClientImpl(
		DWORD dwType,
		CConnectorAutoPtr& pConnector );

	/**
	* ����һ����������
	* 
	* <dwType>, ʲôЭ��ķ�������, ���ͱ����ME_CONNECTION_TYPE�л�ķ�ʽȡֵ
	* <pAcceptor>, out, ���������������ָ�����
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��
	*/
	virtual ME_Result CreateServerImpl(
		DWORD dwType,
		CAcceptorAutoPtr& pAcceptor );

private:

#ifdef ME_WIN32
	/**
	* ͨ��WSAIoctl������Mswsock.lib����load AcceptEx����
	*
	* �����û�����Ȼ�ú���ָ��������ֱ�ӵ��ú���(Ҳ����˵������ʱ��̬����mswsock.lib���ڳ�����ֱ�ӵ��ú���)��
	* ��ô���ܽ�����Ӱ�졣��ΪAcceptEx()������Winsock2�ܹ�֮�⣬ÿ�ε���ʱ��������ͨ��WSAIoctl()ȡ�ú���ָ�롣
	* Ҫ��������������ʧ����Ҫʹ����ЩAPI��Ӧ�ó���Ӧ��ͨ������WSAIoctl()ֱ�Ӵӵײ���ṩ������ȡ�ú�����ָ��
	*/
	static void LoadAcceptEx();

	/**
	* ͨ��WSAIoctl������Mswsock.lib����load ConnectEx����
	*
	* �����û�����Ȼ�ú���ָ��������ֱ�ӵ��ú���(Ҳ����˵������ʱ��̬����mswsock.lib���ڳ�����ֱ�ӵ��ú���)��
	* ��ô���ܽ�����Ӱ�졣��ΪConnectEx()������Winsock2�ܹ�֮�⣬ÿ�ε���ʱ��������ͨ��WSAIoctl()ȡ�ú���ָ�롣
	* Ҫ��������������ʧ����Ҫʹ����ЩAPI��Ӧ�ó���Ӧ��ͨ������WSAIoctl()ֱ�Ӵӵײ���ṩ������ȡ�ú�����ָ��
	*/
	static void LoadConnectEx();

	/**
	* ����õ�AcceptEx���ܶԶ�����ʱ���ͱ���Ҫʹ��GetAcceptExSockaddrs������ȷ��ȡ�Զ˵ĵ�ַ
	*
	* �ر���Ҫע���������AcceptEx��ƥ�����⣬���������
	* 1. �����ֱ��ʹ��::AcceptEx()��������ôֻ��ֱ��ʹ��GetAcceptExSockaddrs()������ȡ�Զ˵�ַ
	* 2. �����ȡ��ȡAcceptEx����ָ��ķ�ʽ���ͱ�����WSAIoctl()����load��GetAcceptExSockaddrs����ָ��
	*	 ��ʹ�����ָ���ȡ�Զ˵�ַ���ܱ�֤��ȡ����ȷ�ĶԶ˵�ַ
	*/
	static void LoadGetAcceptExSockaddrs();

#endif	// ME_WIN32
};

ME_NAME_SPACE_END


#endif	// __CONNECTION_MANAGER_H_
