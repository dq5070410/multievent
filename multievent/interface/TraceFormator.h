/**
* trace��ʽ��������
* 
*
* History
*	03/20/2016		Created
*/

#ifndef __TRACE_FORMATOR_H_
#define __TRACE_FORMATOR_H_

#include "TypeDefines.h"
#include "UtilDefines.h"


ME_NAME_SPACE_BEGIN

/*
 * һ�������ڴ�traceʱ, ������Ҫ��ӡ�����ݽ��и�ʽ���Ĳ�����
*/
class CMETraceFormator
{
public:
	/**
	* д�����ݵı�����ʽ
	*
	* <HEX>, 16����
	* <DECIMAL>, 10����
	*/
	typedef enum tagOrdix
	{
		HEX     = 0,	// 16����
		DECIMAL = 1		// 10����
	} Ordix;

public:
	/**
	* ��ʽ��trace����
	*/
	static int Printf(
		char* pszValue,
		UINT nLength,
		const char* pszFormat, 
		... );

public :
	CMETraceFormator( 
		char* pszBuf, 
		DWORD dwBufSize );

public:
	/**
	* ��һ��charд��<m_pszBuf>��
	*
	* ����ֵ:
	*		����(*this)
	*/
	CMETraceFormator& operator << ( char cValue );

	/**
	* ��һ��BYTEд��<m_pszBuf>��
	*
	* ����ֵ:
	*		����(*this)
	*/
	CMETraceFormator& operator << ( BYTE cbValue );

	/**
	* ��һ��shortд��<m_pszBuf>��
	*
	* ����ֵ:
	*		����(*this)
	*/
	CMETraceFormator& operator << ( short sValue );

	/**
	* ��һ��WORDд��<m_pszBuf>��
	*
	* ����ֵ:
	*		����(*this)
	*/
	CMETraceFormator& operator << ( WORD wValue );

	/**
	* ��һ��intд��<m_pszBuf>��
	*
	* ����ֵ:
	*		����(*this)
	*/
	CMETraceFormator& operator << ( int iValue );

	/**
	* ��һ��UINTд��<m_pszBuf>��
	*
	* ����ֵ:
	*		����(*this)
	*/
	CMETraceFormator& operator << ( UINT nValue );

	/**
	* ��һ��longд��<m_pszBuf>��
	*
	* ����ֵ:
	*		����(*this)
	*/
	CMETraceFormator& operator << ( long lValue );

	/**
	* ��һ��ULONGд��<m_pszBuf>��
	*
	*
	* ����ֵ:
	*		����(*this)
	*/
#ifdef ME_LINUX
	CMETraceFormator& operator << ( ULONG ulValue );
#endif	// ME_LINUX

	/**
	* ��һ��long longд��<m_pszBuf>��
	*
	* ����ֵ:
	*		����(*this)
	*/
	CMETraceFormator& operator << ( LONGLONG lValue );

	/**
	* ��һ��DWORDд��<m_pszBuf>��
	*
	*
	* ����ֵ:
	*		����(*this)
	*/
#ifdef ME_WIN
	CMETraceFormator& operator << ( DWORD dwValue );
#endif	// ME_WIN
	/**
	* ��һ��floatд��<m_pszBuf>��
	*
	* ����ֵ:
	*		����(*this)
	*/
	CMETraceFormator& operator << ( float fValue );

	/**
	* ��һ��doubleд��<m_pszBuf>��
	*
	* ����ֵ:
	*		����(*this)
	*/
	CMETraceFormator& operator << ( double dValue );

	/**
	* ��һ���ַ���д��<m_pszBuf>��
	*
	* ����ֵ:
	*		����(*this)
	*/
	CMETraceFormator& operator << ( const char* lpszValue );

	/**
	* ��һ��ָ���ַд��<m_pszBuf>��
	*
	* ����ֵ:
	*		����(*this)
	*/
	CMETraceFormator& operator << ( void* lpvValue );

	/**
	* ��һ��ָ���ַд��<m_pszBuf>��
	*
	* ����ֵ:
	*		����(*this)
	*/
	CMETraceFormator& operator << ( const void* lpvValue );

	/**
	* ��һ���ַ���д��<m_pszBuf>��
	*
	* ����ֵ:
	*		����(*this)
	*/
	CMETraceFormator& operator << ( const CMEString& strValue );

	/**
	* ������һ��д�������Ƿ�ת����16������ʾ
	*
	* <ordixValue>, ��<ordixValue>ΪHEXʱΪ16����; DECIMALΪ10����
	*
	* ����ֵ:
	*		����(*this)
	*/
	CMETraceFormator& operator << ( Ordix ordixValue );

public:
	/**
	* ���䵱���ú����Ĳ���Ҫ����char*ʱ��formator�ܱ�������ʶ��
	*/
	operator char*();

private :

	/**
	* ������һ��д�������Ƿ�ת����16������ʾ
	*
	* <bHex>, ��<bHex>ΪTRUEʱΪ16����; FALSEΪ10����
	*
	* ����ֵ:
	*		��
	*/
	void SetHexFlag( BOOL bHex );

	/**
	* ��ȡ��ǰд�����ݲ����Ƿ�ת����16������ʾ
	*
	* ����ֵ:
	*		TRUE, Ϊ�ɹ�; FALSE, Ϊʧ��
	*/
	BOOL GetHexFlag();

	/**
	* ��һ���ַ���д��<m_pszBuf>, ���Ҽ���<m_pszBuf>�Ŀ�д����
	*
	* <pszBuf>, ��Ҫд����ַ���
	*
	* ����ֵ:
	*		TRUE, Ϊ�ɹ�; FALSE, Ϊʧ��
	*/
	void Advance( const char* pszBuf );

private :
	char*						m_pszBuf;		// �洢��Ҫд�������
	DWORD						m_dwBufSize;	// <m_pszBuf>�Ĵ�С
	DWORD						m_dwPos;		// ��ǰд��<m_pszBuf>��ʲôλ��
	BOOL						m_bHex;			// ��һ��д�������Ƿ���16������ʾ
};



ME_NAME_SPACE_END

#endif	// __TRACE_FORMATOR_H_




