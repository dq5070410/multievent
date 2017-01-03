/**
* ¥Ú”°logµƒtrace¿‡
* 
*
* History
*	03/20/2016		Created
*/

#ifndef __TRACE_H_
#define __TRACE_H_

#include "Platform.h"
#include "UtilDefines.h"

ME_NAME_SPACE_BEGIN

class CMELockThreadMutex;

template <class T> class CMESingletonT;

class CMETrace
{
public:
	static CMETrace* Instance();

public:

#ifdef ME_WIN
	void Output( 
		int iLevel, 
		const char* pszData );

#endif	// ME_WIN

	void WriteLog(
		int iLevel, 
		const char* pszData );

private:
	void FormatStr( 
		char* pBuffer, 
		int iBufferLength,
		int iLevel,
		const char* pszData );

private:
	CMETrace();
	~CMETrace();

private:
	CMELockThreadMutex*			m_pLock;


private:
	void operator = ( const CMETrace& );
	CMETrace( const CMETrace& );

private:
	friend class CMESingletonT<CMETrace>;
};

ME_NAME_SPACE_END

#endif	// __TRACE_H_







