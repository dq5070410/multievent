/**
* udp socket�Ļ�װ��
* 
*
* History
*	12/12/2016		Created
*/

#ifndef __SOCKET_UDP_H_
#define __SOCKET_UDP_H_

#include "Base.h"
#include "SocketBase.h"

ME_NAME_SPACE_BEGIN

/**
* UDP socket�ķ�װ��
* 
* ע�⣬�������̰߳�ȫ��
*/
class ME_OS_EXPORT CMESocketUdp : public CMESocketBase
{
public:
	CMESocketUdp();

	CMESocketUdp( ME_HANDLE hHandle );

	virtual ~CMESocketUdp();

public:
	/** 
	* ��������
	* 
	* <pszData>, Ҫ���͵�����
	* <dwLength>, Ҫ�������ݵĳ���
	* <hiaPeerAddress>, Ŀ���ַ
	* <iFlag>, ���
	*
	* ����ֵ:
	*		SOCKET_ERROR��ʧ��; 0�ǳɹ�
	*/
	int SendTo(
		const char* pszData, 
		DWORD dwLength,
		const CMEInetAddress& hiaPeerAddress,
		int iFlag = 0 );

	/** 
	* ��������
	* 
	* <pszIov>, Ҫ���͵���������
	* <dwCount>, Ҫ���͵���������ĸ���
	* <hiaPeerAddress>, Ŀ���ַ
	*
	* ����ֵ:
	*		SOCKET_ERROR��ʧ��; 0�ǳɹ�
	*/
	int SendVTo(
		const iovec pszIov[], 
		DWORD dwCount,
		const CMEInetAddress& hiaPeerAddress );

	/** 
	* ��ȡ����
	* 
	* <pszBuf>, ��ȡ���ݵĻ�����
	* <dwLength>, ��ȡ���ݵĻ�������С
	* <hiaPeerAddress>, Ŀ���ַ
	* <iFlag>, ���
	*
	* ����ֵ:
	*		SOCKET_ERROR��ʧ��; 0�ǳɹ�
	*/
	int RecvFrom(
		char*& pszData, 
		DWORD dwLength,
		CMEInetAddress& hiaPeerAddress,
		int iFlag = 0 );
protected:
	virtual ME_Result OpenImpl();

	virtual ME_Result CloseImpl();
};

ME_NAME_SPACE_END

#endif	// __SOCKET_UDP_H_




















