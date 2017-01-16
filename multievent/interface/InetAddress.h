/**
* �����ַ��
* 
*
* History
*	12/11/2016		Created
*/

#ifndef __INET_ADDRESS_H_
#define __INET_ADDRESS_H_

#include "Base.h"

ME_NAME_SPACE_BEGIN

/**
* �����ַ��
*
* Ŀǰֻ֧��IPv4
*/
class OS_EXPORT CMEInetAddress
{
public:
	CMEInetAddress();

	/**
	* ���캯��
	*
	* <pszHostNameAndPort>����ַ��˿ڣ�����"127.0.0.1:80"
	*/
	CMEInetAddress( const char* pszHostNameAndPort );

	/**
	* ���캯��
	*
	* <pszHostName>����ַ������"127.0.0.1"
	* <wPort>���˿ڣ�����ܳ���65535
	*/
	CMEInetAddress( 
		const char* pszHostName,
		WORD wPort );

	/* 2013.9.5 added by Τ�� */
	CMEInetAddress( const ME_SOCK_ADDR& hsaAddress );

public:
	/**
	* ��ȡ��һ�������ĵ�һ����ЧIP��ַ
	*
	* <hiaLocalAddress>����ȡ���ı��ص�ַ
	* <bIPv4>��IPv6��IPv4��ַ��Ĭ����IPv4���͵ĵ�ַ
	* <bLoopback>���Ƿ��ȡ��·��ַ
	*/
	static ME_Result GetLocalAddress( 
		CMEInetAddress& hiaLocalAddress,
		BOOL bIPv4 = TRUE,
		BOOL bLoopback = FALSE);

public:

	/**
	* ���õ�ַ�Ͷ˿�
	*
	* <pszHostName>, ��ַ��
	* <wPort>, �˿�
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��
	*/
	ME_Result Set( 
		const char* pszHostName,
		WORD wPort );

	/**
	* ͬʱ���õ�ַ�Ͷ˿�, ��"127.0.0.1:1224"
	* ʹ������������õ�ַʱ, ����ȷ���ַ����б�����ð��
	*
	* <pszHostNameAndPort>, ��ַ���Ͷ˿�, ��"127.0.0.1:1224"
	* <dwLength>, <pszHostNameAndPort>�ĳ���
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��
	*/
	ME_Result Set( const char* pszHostNameAndPort );

	/**
	* ���õ�ַ
	* 
	* <dwIP>, ��������IP��ַ, ʹ�������������ʱ��Ҫȷ��<dwIP>�Ѿ����иߵ��ֽ�ת��
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��	
	*/
	ME_Result Set( DWORD dwIP );

	ME_Result Set( const ME_SOCK_ADDR& hsaAddress );

	/**
	* ���ö˿�
	*
	* <wPort>, �˿�
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��
	*/
	ME_Result SetPort( WORD wPort );

	/**
	* ��ȡ�˿�
	*
	* ����ֵ:
	*		WORDֵ
	*/
	WORD GetPort() const;

	/**
	* ȡ�õ�ǰ��ַ
	*
	* ����ֵ:
	*		CMEStringֵ
	*/
	CMEString GetIP() const;

	/**
	* ȡ��ԭ�����ַ���ͽṹ��С
	*
	* ����ֵ:
	*		DWORDֵ
	*/
	DWORD GetSize() const;

	/**
	* ����ַ�Ƿ��ǻ�·��ַ
	*
	* WARNING��Ŀǰ�ú�����������IPv6
	*
	* ����ֵ:
	*		TRUEΪ�ɹ�������Ϊʧ��
	*/
	BOOL IsLocalHost() const;

	/**
	* ȡ��ԭ�����ַָ��
	*
	* ����ֵ:
	*		��NULLΪ�ɹ�; NULLΪʧ��
	*/
	const ME_SOCK_ADDR* GetRawAddress() const;

public:

	/**
	* == �����
	* �ж�����CMEInetAddress�����Ƿ����
	*/
	BOOL operator == ( const CMEInetAddress& DstAddress ) const;

	/**
	* < �����
	* �жϵ�ǰCMEInetAddress����С��Ŀ�����
	* ͨ������std::map�еĲ��Ҳ���
	*/
	BOOL operator < ( const CMEInetAddress& DstAddress ) const;

public:
	/**
	* ����hash_map
	*/
	class HashCompare
	{
	public:
		enum
		{	/** 
			* ��ϣ����Ҫ����������
			* ��Ϊ��_Hash������л�ȥtraits���ȶ��� - Ҳ����HashCompare��<bucket_size>��<min_buckets>
			* �������������������ֲ��ܸ�
			*/
			bucket_size = 4,
			min_buckets = 8
		};

	public:
		/** 
		* ��<Src>�еĳ�����ipֵ��һ���̶���ֵ�������
		*
		* <Src>, ��Ҫ���Ķ���
		*
		* ����ֵ:
		*		long����Χ֮�ڵ��κ�ֵ
		*/
		LONG GetHashValue( const CMEInetAddress& Src ) const;

		/**  
		* �Ա�����CMEInetAddress����, ����������CMEInetAddress��operator <
		*
		* <Src>, Դ����
		* <Dst>, Ŀ�����
		*
		* ����ֵ:
		*		SrcС��Dst, ����TRUE; ���򷵻�FALSE
		*/
		BOOL operator () (
			const CMEInetAddress& Src, 
			const CMEInetAddress& Dst ) const;

		/**  
		* hashĿ�����
		* �㷨copy��<hash_map>��<hash_compare>
		*
		* <Dst>, Ŀ�����
		*
		* ����ֵ:
		*		long����Χ֮�ڵ��κ�ֵ
		*/
		LONG operator() ( const CMEInetAddress& Dst ) const;
	};

private:
	/**
	* ���õ�ַ
	*
	* <pszIP>, ��ַ, �������˿�
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��
	*/
	ME_Result SetIP( const char* pszIP );

	/**
	* ���õ�ַ
	*
	* <dwIP>, �������͵�ַ
	* <bIsNetworkOrder>, <dwIP>�Ƿ��Ѿ���ת�������ֽ�����?
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��
	*/
	ME_Result SetIP( 
		DWORD dwIP, 
		BOOL bIsNetworkOrder = FALSE );

private:
	ME_SOCK_ADDR					m_SockAddr;	// ϵͳ��ַ

};

ME_NAME_SPACE_END

#endif	// __INET_ADDRESS_H_



