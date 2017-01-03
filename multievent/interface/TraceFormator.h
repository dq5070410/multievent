/**
* trace格式化工具类
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
 * 一个用于在打trace时, 对所需要打印的内容进行格式化的操作类
*/
class CMETraceFormator
{
public:
	/**
	* 写入内容的表现形式
	*
	* <HEX>, 16进制
	* <DECIMAL>, 10进制
	*/
	typedef enum tagOrdix
	{
		HEX     = 0,	// 16进制
		DECIMAL = 1		// 10进制
	} Ordix;

public:
	/**
	* 格式化trace内容
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
	* 把一个char写到<m_pszBuf>中
	*
	* 返回值:
	*		返回(*this)
	*/
	CMETraceFormator& operator << ( char cValue );

	/**
	* 把一个BYTE写到<m_pszBuf>中
	*
	* 返回值:
	*		返回(*this)
	*/
	CMETraceFormator& operator << ( BYTE cbValue );

	/**
	* 把一个short写到<m_pszBuf>中
	*
	* 返回值:
	*		返回(*this)
	*/
	CMETraceFormator& operator << ( short sValue );

	/**
	* 把一个WORD写到<m_pszBuf>中
	*
	* 返回值:
	*		返回(*this)
	*/
	CMETraceFormator& operator << ( WORD wValue );

	/**
	* 把一个int写到<m_pszBuf>中
	*
	* 返回值:
	*		返回(*this)
	*/
	CMETraceFormator& operator << ( int iValue );

	/**
	* 把一个UINT写到<m_pszBuf>中
	*
	* 返回值:
	*		返回(*this)
	*/
	CMETraceFormator& operator << ( UINT nValue );

	/**
	* 把一个long写到<m_pszBuf>中
	*
	* 返回值:
	*		返回(*this)
	*/
	CMETraceFormator& operator << ( long lValue );

	/**
	* 把一个ULONG写到<m_pszBuf>中
	*
	*
	* 返回值:
	*		返回(*this)
	*/
#ifdef ME_LINUX
	CMETraceFormator& operator << ( ULONG ulValue );
#endif	// ME_LINUX

	/**
	* 把一个long long写到<m_pszBuf>中
	*
	* 返回值:
	*		返回(*this)
	*/
	CMETraceFormator& operator << ( LONGLONG lValue );

	/**
	* 把一个DWORD写到<m_pszBuf>中
	*
	*
	* 返回值:
	*		返回(*this)
	*/
#ifdef ME_WIN
	CMETraceFormator& operator << ( DWORD dwValue );
#endif	// ME_WIN
	/**
	* 把一个float写到<m_pszBuf>中
	*
	* 返回值:
	*		返回(*this)
	*/
	CMETraceFormator& operator << ( float fValue );

	/**
	* 把一个double写到<m_pszBuf>中
	*
	* 返回值:
	*		返回(*this)
	*/
	CMETraceFormator& operator << ( double dValue );

	/**
	* 把一个字符串写到<m_pszBuf>中
	*
	* 返回值:
	*		返回(*this)
	*/
	CMETraceFormator& operator << ( const char* lpszValue );

	/**
	* 把一个指针地址写到<m_pszBuf>中
	*
	* 返回值:
	*		返回(*this)
	*/
	CMETraceFormator& operator << ( void* lpvValue );

	/**
	* 把一个指针地址写到<m_pszBuf>中
	*
	* 返回值:
	*		返回(*this)
	*/
	CMETraceFormator& operator << ( const void* lpvValue );

	/**
	* 把一个字符串写到<m_pszBuf>中
	*
	* 返回值:
	*		返回(*this)
	*/
	CMETraceFormator& operator << ( const CMEString& strValue );

	/**
	* 设置下一次写入内容是否转换成16进制显示
	*
	* <ordixValue>, 当<ordixValue>为HEX时为16进制; DECIMAL为10进制
	*
	* 返回值:
	*		返回(*this)
	*/
	CMETraceFormator& operator << ( Ordix ordixValue );

public:
	/**
	* 适配当调用函数的参数要求是char*时，formator能被编译器识别
	*/
	operator char*();

private :

	/**
	* 设置下一次写入内容是否转换成16进制显示
	*
	* <bHex>, 当<bHex>为TRUE时为16进制; FALSE为10进制
	*
	* 返回值:
	*		无
	*/
	void SetHexFlag( BOOL bHex );

	/**
	* 获取当前写入内容操作是否转换成16进制显示
	*
	* 返回值:
	*		TRUE, 为成功; FALSE, 为失败
	*/
	BOOL GetHexFlag();

	/**
	* 把一个字符串写入<m_pszBuf>, 并且减少<m_pszBuf>的可写长度
	*
	* <pszBuf>, 需要写入的字符串
	*
	* 返回值:
	*		TRUE, 为成功; FALSE, 为失败
	*/
	void Advance( const char* pszBuf );

private :
	char*						m_pszBuf;		// 存储需要写入的内容
	DWORD						m_dwBufSize;	// <m_pszBuf>的大小
	DWORD						m_dwPos;		// 当前写到<m_pszBuf>的什么位置
	BOOL						m_bHex;			// 下一次写入内容是否以16进制显示
};



ME_NAME_SPACE_END

#endif	// __TRACE_FORMATOR_H_




