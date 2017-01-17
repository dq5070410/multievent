/**
* SOCKET��ز�����API
* 
*
* History
*	12/11/2016		Created
*/

#ifndef __OS_SOCKET_H_
#define __OS_SOCKET_H_

#include "Base.h"

NAME_SPACE_OS_BEGIN

/**
* ��һ��socket����þ��
* 
* <iFamily>, Э����
* <iType>, ����
* <iProtocol>, Э��
* <bReuseAddr>, ��ַ�Ƿ���������
* <Handle>, ���ص�socket���
*
* ����ֵ:
*		SOCKET_ERROR��ʧ��; ME_OK�ǳɹ�
*/
ME_Result ME_OS_EXPORT SocketOpen(
	int iFamily,
	int iType,
	int iProtocol,
	ME_HANDLE& Handle );

/**
* �ر�һ��socket
* 
* <Handle>, ��Ҫ�رյ�socket���
*
* ����ֵ:
*		SOCKET_ERROR��ʧ��; ME_OK�ǳɹ�
*/
ME_Result ME_OS_EXPORT SocketClose( ME_HANDLE Handle );

/**
* ��������
* 
* <Handle>, ���ĸ�socket�Ϸ�������
* <pszData>, Ҫ���͵�����
* <dwLength>, Ҫ�������ݵĳ���
* <iFlag>, ���
*
* ����ֵ:
*		���ر��ε��÷��͵��ֽ�������
*/
int ME_OS_EXPORT SocketSend(
	ME_HANDLE Handle,
	const char* pszData,
	DWORD dwLength,
	int iFlag = 0 );

/**
* ��������
* 
* <Handle>, ���ĸ�socket�Ϸ�������
* <pszIov>, Ҫ���͵���������
* <dwCount>, Ҫ���͵���������ĸ���
*
* ����ֵ:
*		���ر��ε��÷��͵��ֽ�������
*/
int ME_OS_EXPORT SocketSendV(
	ME_HANDLE Handle,
	const iovec pszIov[],
	DWORD dwCount );

/**
* ��ȡ����
* 
* <Handle>, ���ĸ�socket����ȡ����
* <pszBuf>, ��ȡ���ݵĻ�����
* <dwLength>, ��ȡ���ݵĻ�������С
* <iFlag>, ���
*
* ����ֵ:
*		���ر�����ȡ���ֽ�������
*/
int ME_OS_EXPORT SocketRecv(
	ME_HANDLE Handle,
	char*& pszBuf,
	DWORD dwLength,
	int iFlag = 0 );

/**
* ��������
* 
* <Handle>, ���ĸ�socket����ȡ����
* <pszIov>, ��ȡ���ݻ���������
* <dwCount>, ��ȡ���ݻ���������ĸ���
*
* ����ֵ:
*		���ر�����ȡ���ֽ�������
*/
int ME_OS_EXPORT SocketRecvV(
	ME_HANDLE Handle,
	iovec szIov[],
	DWORD dwCount );

/**
* ��������, ������UDP
* 
* <Handle>, ���ĸ�socket�Ϸ�������
* <pszData>, Ҫ���͵�����
* <dwLength>, Ҫ�������ݵĳ���
* <pPeerAddress>, Ŀ���ַ
* <iFlag>, ���
*
* ����ֵ:
*		���ر��ε��÷��͵��ֽ�������
*/
int ME_OS_EXPORT SocketSendTo(
	ME_HANDLE Handle,
	const char* pszData,
	DWORD dwLength,
	const ME_SOCK_ADDR* pPeerAddress,
	int iFlag = 0 );

/**
* ��������, ������UDP
* 
* <Handle>, ���ĸ�socket�Ϸ�������
* <pszIov>, Ҫ���͵���������
* <dwCount>, Ҫ���͵���������ĸ���
* <pPeerAddress>, Ŀ���ַ
*
* ����ֵ:
*		���ر��ε��÷��͵��ֽ�������
*/
int ME_OS_EXPORT SocketSendVTo(
	ME_HANDLE Handle,
	const iovec pszIov[],
	DWORD dwCount,
	const ME_SOCK_ADDR* pPeerAddress );

/**
* ��ȡ����, ������UDP
* 
* <Handle>, ���ĸ�socket����ȡ����
* <pszBuf>, ��ȡ���ݵĻ�����
* <dwLength>, ��ȡ���ݵĻ�������С
* <hsaPeerAddress>, Ŀ���ַ
* <iFlag>, ���
*
* ����ֵ:
*		���ر�����ȡ���ֽ�������
*/
int ME_OS_EXPORT SocketRecvFrom(
	ME_HANDLE Handle,
	char*& pszBuf,
	DWORD dwLength,
	ME_SOCK_ADDR& hsaPeerAddress,
	int iFlag = 0 );

/**
* Ϊһ��socket�����һ�����ص�ַ
* 
* <Handle>, socket���
* <pLocalAddrYouWantToBind>, ��Ҫ�󶨵ĵ�ַ
*
* ����ֵ:
*		SOCKET_ERROR��ʧ��; ME_OK�ǳɹ�
*/
ME_Result ME_OS_EXPORT SocketBind(
	ME_HANDLE Handle,
	const ME_SOCK_ADDR* pLocalAddrYouWantToBind );

/**
* ����ĳ��socket��ĳЩ����
* 
* <Handle>, socket���
* <iFlag>, ��Ҫ���εĹ���
*
* ����ֵ:
*		SOCKET_ERROR��ʧ��; ME_OK�ǳɹ�
*/
ME_Result ME_OS_EXPORT SocketShutdown(
	ME_HANDLE Handle,
	int iFlag );

/**
* ����
* 
* <Handle>, socket���
* <pPeerAddress>, Ҫ���ӵ�����
*
* ����ֵ:
*		SOCKET_ERROR��ʧ��; ME_OK�ǳɹ�
*/
ME_Result ME_OS_EXPORT SocketConnect(
	ME_HANDLE Handle,
	const ME_SOCK_ADDR* pPeerAddress );


/**
* ����
* 
* <Handle>, socket���
*
* ����ֵ:
*		SOCKET_ERROR��ʧ��; ME_OK�ǳɹ�
*/
ME_Result ME_OS_EXPORT SocketListen( ME_HANDLE Handle );

/**
* ����
* 
* <Handle>, socket���
* <hsaPeerAddress>, Ҫ��ʲô��ַ���˿��ϼ���
*
* ����ֵ:
*		SOCKET_ERROR��ʧ��; ME_OK�ǳɹ�
*/
ME_HANDLE ME_OS_EXPORT SocketAccept(
	ME_HANDLE Handle,
	ME_SOCK_ADDR& hsaPeerAddress );

/**
* ��һ��4�ֽڵ�DWORDֵת���ɷ���IPV4�淶�ĵ�ַ
* 
* <dwIP>, IP��ַ��DWORDֵ
*
* ����ֵ:
*		�ַ�������Ϊ0��ʧ��; ��ME_OK�ǳɹ�
*/
CMEString ME_OS_EXPORT IpDigitalToString( DWORD dwIP );

/**
* ��һ������IPV4�淶�ĵ�ַת����4�ֽڵ�DWORDֵ
* 
* <pszIP>, IP��ַ
*
* ����ֵ:
*		0��ʧ��; ��ME_OK�ǳɹ�
*/
DWORD ME_OS_EXPORT IpStringToDigital( const char* pszIP );

/**
* ����socket��I/Oģʽ
* 
* <Handle>, ��Ҫ���õ�socket���
* <iFlag>, ��־
* <pArg>, ��Ӧ��<iFlag>��ֵ
*
* ����ֵ:
*		SOCKET_ERROR��ʧ��; ME_OK�ǳɹ�
*/
ME_Result ME_OS_EXPORT IOCtrl(
	ME_SOCKET Handle,
	int iFlag,
	void* pArg );

/**
* ��ȡһ���������������󶨵ı��ص�ַ
* 
* <Handle>, ��Ҫ���õ�socket���
* <hsaLocalAddress>, out, ���󶨵ĵ�ַ
*
* ����ֵ:
*		SOCKET_ERROR��ʧ��; ME_OK�ǳɹ�
*/
ME_Result ME_OS_EXPORT SocketGetLocalAddress(
	ME_HANDLE Handle,
	ME_SOCK_ADDR& hsaLocalAddress );

/**
* ��ȡһ���������������󶨵ĶԶ˵�ַ
* 
* <Handle>, ��Ҫ���õ�socket���
* <hsaRemoteAddress>, out, �Զ˵�ַ
*
* ����ֵ:
*		SOCKET_ERROR��ʧ��; ME_OK�ǳɹ�
*/
ME_Result ME_OS_EXPORT SocketGetRemoteAddress(
	ME_HANDLE Handle,
	ME_SOCK_ADDR& hsaRemoteAddress );

/**
* ����socket������
* 
* <Handle>, ��Ҫ���õ�socket���
* <iLevel>, Ҫ���õ�������Ա�������һ�����
* <iOption>, Ҫ���õ�����
* <pOptval>, ����<iOption>���Ե�ֵ
* <iOptlen>, ����ֵ<pOptval>�ĳ����Ƕ���
*
* ����ֵ:
*		SOCKET_ERROR��ʧ��; ME_OK�ǳɹ�
*/
ME_Result ME_OS_EXPORT SocketSetSocketOption(
	ME_HANDLE Handle,
	int iLevel, 
	int iOption, 
	const void* pOptval, 
	int iOptlen );

/**
* ��ȡsocket������
* 
* <Handle>, ��Ҫ���õ�socket���
* <iLevel>, Ҫ��ȡ��������Ա�������һ�����
* <iOption>, Ҫ��ȡ������
* <pOptval>, ����<iOption>���Ե�ֵ
* <iOptlen>, ����ֵ<pOptval>�ĳ����Ƕ���
*
* ����ֵ:
*		SOCKET_ERROR��ʧ��; ME_OK�ǳɹ�
*/
ME_Result ME_OS_EXPORT SocketGetSocketOption(
	ME_HANDLE Handle,
	int iLevel, 
	int iOption, 
	void* pOptval, 
	int* pOptlen );

NAME_SPACE_OS_END


#endif	// __OS_SOCKET_H_



