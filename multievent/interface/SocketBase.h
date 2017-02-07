/**
* �ײ�socket�Ļ�����װ��
* ���е�socket�඼Ӧ�ô�����̳�
* 
*
* History
*	12/12/2016		Created
*/

#ifndef __SOCKET_BASE_H_
#define __SOCKET_BASE_H_

#include "Base.h"
#include "IPBase.h"
#include "SocketErrorAutoSave.h"

ME_NAME_SPACE_BEGIN
class CMEInetAddress;
ME_NAME_SPACE_END

ME_NAME_SPACE_BEGIN

/**
* �ײ�socket�Ļ�����װ��
* ���е�socket�඼Ӧ�ô�����̳�
*/
class ME_OS_EXPORT CMESocketBase : public CIPBase
{
public:
	virtual ~CMESocketBase();

public:
	/** 
	* ���Դ�һ��socket
	* 
	* <bReuse>, �Ƿ������ַ����
	*
	* ����ֵ:
	*		SOCKET_ERROR��ʧ��; ME_OK�ǳɹ�
	*/
	ME_Result Open( BOOL bReuse = FALSE );

	/** 
	* ���Դ�һ��socket, ���󶨵�һ�����ص�ַ
	* 
	* <hiaLocalAddr>, ��Ҫ�󶨵ĵ�ַ
	* <bReuse>, �Ƿ������ַ����
	*
	* ����ֵ:
	*		ME_OK�ǳɹ�, ����Ϊʧ��
	*/
	ME_Result Open( 
		const CMEInetAddress& hiaLocalAddr,
		BOOL bReuse = FALSE );

	/** 
	* �������״̬
	* 
	* <hiaLocalAddr>, ��Ҫ�����ĵ�ַ
	*
	* ����ֵ:
	*		ME_OK�ǳɹ�, ����Ϊʧ��
	*/
	ME_Result Listen( const CMEInetAddress& hiaLocalAddr );

	/** 
	* ���ܶԶ˽���
	*
	* <hHandle>, �Զ����Ӿ��
	* <hiaPeerAddr>, �Զ˵ĵ�ַ
	*
	* ����ֵ:
	*		ME_OK�ǳɹ�, ����Ϊʧ��
	*/
	ME_Result Accept(
		ME_HANDLE& hHandle,
		CMEInetAddress& hiaPeerAddr );

	/**
	* ���Թر�һ��socket����<m_hhHandle>��ΪME_INVALID_HANDLE
	*
	* ����ֵ:
	*		ME_OK�ǳɹ�, ����Ϊʧ��
	*/
	ME_Result Close();

	/** 
	* ��������
	* 
	* <pszData>, Ҫ���͵�����
	* <dwLength>, Ҫ�������ݵĳ���
	* <iFlag>, ���
	*
	* ����ֵ:
	*		���ر��ε��÷��͵��ֽ�������
	*/
	int Send(
		const char* pszData,
		DWORD dwLength,
		int iFlag = 0 );

	/** 
	* ��������
	* 
	* <pszIov>, Ҫ���͵���������
	* <dwCount>, Ҫ���͵���������ĸ���
	*
	* ����ֵ:
	*		���ر��ε��÷��͵��ֽ�������
	*/
	int SendV(
		const iovec pszIov[],
		DWORD dwCount );

	/** 
	* ��ȡ����
	* 
	* <pszBuf>, ��ȡ���ݵĻ�����
	* <dwLength>, ��ȡ���ݵĻ�������С
	* <iFlag>, ���
	*
	* ����ֵ:
	*		���ر�����ȡ���ֽ�������
	*/
	int Recv(
		char*& pszBuf,
		DWORD dwLength,
		int iFlag = 0 );

	/** 
	* ��������
	* 
	* <pszIov>, ��ȡ���ݻ���������
	* <dwCount>, ��ȡ���ݻ���������ĸ���
	*
	* ����ֵ:
	*		���ر�����ȡ���ֽ�������
	*/
	int RecvV(
		iovec szIov[],
		DWORD dwCount );

	/** 
	* ����socket������
	* 
	* <iLevel>, Ҫ���õ�������Ա�������һ�����
	* <iOption>, Ҫ���õ�����
	* <pOptval>, ����<iOption>���Ե�ֵ
	* <iOptlen>, ����ֵ<pOptval>�ĳ����Ƕ���
	*
	* ����ֵ:
	*		SOCKET_ERROR��ʧ��; ME_OK�ǳɹ�
	*/
	ME_Result SetOption(
		int iLevel, 
		int iOption, 
		const void* pOptval, 
		int iOptlen );

	/** 
	* ��ȡsocket������
	* 
	* <iLevel>, Ҫ��ȡ��������Ա�������һ�����
	* <iOption>, Ҫ��ȡ������
	* <pOptval>, ����<iOption>���Ե�ֵ
	* <iOptlen>, ����ֵ<pOptval>�ĳ����Ƕ���
	*
	* ����ֵ:
	*		SOCKET_ERROR��ʧ��; ME_OK�ǳɹ�
	*/
	ME_Result GetOption(
		int iLevel, 
		int iOption, 
		void* pOptval, 
		int* pOptlen );

	/** 
	* ��ȡһ���������������󶨵ı��ص�ַ
	* 
	* <hiaLocalAddr>, out, ���󶨵ĵ�ַ
	*
	* ����ֵ:
	*		SOCKET_ERROR��ʧ��; ME_OK�ǳɹ�
	*/
	ME_Result GetLoacalAddress( CMEInetAddress& hiaLocalAddr );

	/** 
	* ��ȡһ���������������󶨵ĶԶ˵�ַ
	* 
	* <hiaRemoteAddress>, out, �Զ˵�ַ
	*
	* ����ֵ:
	*		SOCKET_ERROR��ʧ��; ME_OK�ǳɹ�
	*/
	ME_Result GetRemoteAddress( CMEInetAddress& hiaRemoteAddress );

protected:
	CMESocketBase();

	CMESocketBase( ME_HANDLE hHandle );

protected:
	/**
	* �ڻ������еĶ���֮�⣬��(�̳���)����Ҫ��ʲô
	*/
	virtual ME_Result OpenImpl() = 0;

	/**
	* �ڻ������еĶ���֮�⣬��(�̳���)����Ҫ��ʲô
	*/
	virtual ME_Result CloseImpl() = 0;

protected:
	/**
	* ��ӡ��ǰ��socket����, ���ĸ������﷢���Ĵ���, thisָ��, �Լ�����errno
	*/
	void ShowSocketError( int iResult );
};

ME_NAME_SPACE_END

#endif	// __ME_SOCKET_BASE_H_








