#include "TraceFormator.h"
#include "TraceDefines.h"

ME_NAME_SPACE_BEGIN

CMETraceFormator::CMETraceFormator( 
	char* pszBuf, 
	DWORD dwBufSize )
	: m_pszBuf( pszBuf )
	, m_dwBufSize( dwBufSize )
	, m_dwPos( 0 )
	, m_bHex( FALSE )
{

}

void CMETraceFormator::SetHexFlag( BOOL bHex )
{
	m_bHex = bHex;
}

BOOL CMETraceFormator::GetHexFlag()
{
	return m_bHex;
}

void CMETraceFormator::Advance( const char* pszBuf )
{
	if( pszBuf )
	{
		DWORD dwLength = ::strnlen( 
			pszBuf, 
			m_dwBufSize );

		if( dwLength > (m_dwBufSize - m_dwPos - ME_TRACE_RESERVE_DATA_LEN) )
		{
			dwLength = m_dwBufSize - m_dwPos - ME_TRACE_RESERVE_DATA_LEN;
		}

		if( dwLength > 0 )
		{
#ifdef ME_WIN
			::memcpy_s(
				m_pszBuf + m_dwPos * sizeof(char),
				m_dwBufSize - m_dwPos * sizeof(char), 
				(void*)pszBuf,
				dwLength * sizeof(char) );

#elif defined( ME_LINUX )
			::memcpy(
				m_pszBuf + m_dwPos * sizeof(char),
				(void*)pszBuf,
				dwLength * sizeof(char) );

#endif	// ME_WIN
			m_dwPos += dwLength;
		}
	}
}

int CMETraceFormator::Printf( 
	char* pszValue, 
	UINT nLength, 
	const char* pszFormat, 
	... )
{

	::memset(
		pszValue,
		0,
		nLength );

	va_list vlList;
	va_start(
		vlList, 
		pszFormat );

	int iResult = 0;
	
#ifdef ME_WIN
	iResult = ::_vsprintf_s_l(
		pszValue, 
		nLength, 
		pszFormat, 
		NULL, 
		vlList );

#else
	iResult = ::vsprintf(
		pszValue,
		pszFormat,
		vlList );

#endif	// ME_WIN

	va_end( vlList );

	return iResult;
}

CMETraceFormator::operator char*()
{
	return m_pszBuf;
}

CMETraceFormator& CMETraceFormator::operator << ( int iValue )
{
	char szValue[64];
	
	Printf(
		szValue,
		64,
		FALSE == GetHexFlag() ? "%d" : "%08x",
		iValue );

	Advance( szValue );

	SetHexFlag( FALSE );

	return (*this);
}

CMETraceFormator& CMETraceFormator::operator << ( UINT nValue )
{
	char szValue[64];
	
	Printf(
		szValue,
		64,
		FALSE == GetHexFlag() ? "%u" : "%08x",
		nValue );

	Advance( szValue );

	SetHexFlag( FALSE );

	return (*this);
}

CMETraceFormator& CMETraceFormator::operator << ( char cValue )
{
	return (*this << (int)cValue);
}

CMETraceFormator& CMETraceFormator::operator << ( BYTE cbValue )
{
	return (*this << (int)cbValue);
}

CMETraceFormator& CMETraceFormator::operator << ( short sValue )
{
	return (*this << (int)sValue);
}

CMETraceFormator& CMETraceFormator::operator << ( WORD wValue )
{
	return (*this << (int)wValue);
}

CMETraceFormator& CMETraceFormator::operator << ( long lValue )
{
	char szValue[64];
	
	Printf(
		szValue,
		64,
		FALSE == GetHexFlag() ? "%ld" : "%08lx",
		lValue );

	Advance( szValue );

	SetHexFlag( FALSE );

	return (*this);
}

#ifdef ME_LINUX
CMETraceFormator& CMETraceFormator::operator << ( ULONG ulValue )
{
	return (*this) << (LONG)ulValue;
}

#endif	// ME_LINUX

CMETraceFormator& CMETraceFormator::operator << ( LONGLONG lValue )
{
	char szValue[100];
	
	Printf(
		szValue,
		100,
		FALSE == GetHexFlag() ? "%lld" : "%08lx",
		lValue );

	Advance( szValue );

	SetHexFlag( FALSE );

	return (*this);
}

#ifdef ME_WIN
CMETraceFormator& CMETraceFormator::operator << ( DWORD dwValue )
{
	char szValue[64];
	
	Printf(
		szValue,
		64,
		FALSE == GetHexFlag() ? "%lu" : "%08lx",
		dwValue );

	Advance( szValue );

	SetHexFlag( FALSE );

	return (*this);
}

#endif	// ME_WIN

CMETraceFormator& CMETraceFormator::operator << ( float fValue )
{
	char szValue[64];
	
	Printf(
		szValue,
		64,
		"%f",
		fValue );

	Advance( szValue );

	SetHexFlag( FALSE );

	return (*this);
}

CMETraceFormator& CMETraceFormator::operator << ( double dValue )
{
	char szValue[64];

	Printf(
		szValue,
		64,
		"%f",
		dValue );

	Advance( szValue );

	SetHexFlag( FALSE );

	return (*this);
}

CMETraceFormator& CMETraceFormator::operator << ( const char* lpszValue )
{
	Advance( lpszValue );

	return (*this);
}

CMETraceFormator& CMETraceFormator::operator << ( const CMEString& strValue )
{
	return (*this << strValue.c_str());
}

CMETraceFormator& CMETraceFormator::operator << ( Ordix ordixValue )
{
	switch( ordixValue )
	{
	case HEX:
		{
			SetHexFlag( TRUE );

			break;
		}

	case DECIMAL:
		{
			SetHexFlag( FALSE );

			break;
		}

	default:
		{
			break;
		}
	}

	return (*this);
}

CMETraceFormator& CMETraceFormator::operator << ( void* lpvValue )
{
#ifdef ME_WIN
	return (*this << "0x" << HEX << reinterpret_cast<DWORD>(lpvValue));

#elif defined( ME_LINUX )
	return (*this << "0x" << HEX << reinterpret_cast<ULONG>(lpvValue));

#endif	// ME_WIN
}

CMETraceFormator& CMETraceFormator::operator << ( const void* lpvValue )
{
	return (*this << (void*)(lpvValue));
}

ME_NAME_SPACE_END











