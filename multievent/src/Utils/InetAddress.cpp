#include "InetAddress.h"
#include "OSSocket.h"

#ifndef ME_HASH_SEED
	#define ME_HASH_SEED 0xabcdabcd
#endif	// HASH_SEED

#ifndef ME_LONG_MAX
	#define ME_LONG_MAX 2147483647L
#endif

#ifndef ME_RANDOM_DENOMINATOR
	#define ME_RANDOM_DENOMINATOR 781224L
#endif

#ifndef ME_RANDOM_QUOT
	#define ME_RANDOM_QUOT 9050L
#endif

#ifndef ME_RANDOM_QREM
	#define ME_RANDOM_QREM 19819L
#endif

/* IP地址与端口(这个字符串)最大长度为256 */
const DWORD MAX_IP_AND_PORT_LENGTH = 256;

/* 端口最大值不能超过65536 */
const WORD MAX_PORT_NUM = 0xffff;

ME_NAME_SPACE_BEGIN

CInetAddress::CInetAddress()
{
	Set(
		NULL,
		0 );
}

CInetAddress::CInetAddress( 
	const char* pszHostName, 
	WORD wPort )
{
	Set(
		pszHostName,
		wPort );
}

CInetAddress::CInetAddress( const char* pszHostNameAndPort )
{
	Set( pszHostNameAndPort );
}

CInetAddress::CInetAddress( const ME_SOCK_ADDR& hsaAddress )
{
	Set( hsaAddress );
}

ME_Result CInetAddress::GetLocalAddress(
	CInetAddress& hiaLocalAddress, 
	BOOL bIPv4 /* = TRUE */, 
	BOOL bLoopback /* = FALSE */)
{
	if ( (TRUE == bIPv4) && (TRUE == bLoopback) )
	{
		return hiaLocalAddress.SetIP( INADDR_LOOPBACK );
	}

	ME_Result hResult = ME_ERROR_NOT_AVAILABLE;
	struct addrinfo* pAddrInfo = NULL;

	do 
	{
		char szBuf[MAX_IP_AND_PORT_LENGTH];
		::memset( szBuf, 0, MAX_IP_AND_PORT_LENGTH );

		hResult = ::gethostname( 
			szBuf, 
			MAX_IP_AND_PORT_LENGTH );

		ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

		struct addrinfo addrInfo;
		::memset( 
			&addrInfo, 
			0, 
			sizeof(addrinfo) );

		addrInfo.ai_family = TRUE == bIPv4 ? AF_INET : AF_INET6;

		hResult = ::getaddrinfo(
			szBuf, 
			NULL, 
			&addrInfo, 
			&pAddrInfo );

		if ( ME_FAILED(hResult) )
		{
			break;
		}

		hResult = hiaLocalAddress.Set( *(ME_SOCK_ADDR*)pAddrInfo->ai_addr );

	} while ( 0 );

	::freeaddrinfo( pAddrInfo );

	return hResult;
}

WORD CInetAddress::GetPort() const
{
	return ::ntohs( m_SockAddr.sin_port );
}

CMEString CInetAddress::GetIP() const
{
	return OS::IpDigitalToString( m_SockAddr.sin_addr.s_addr );
}

DWORD CInetAddress::GetSize() const
{
	return sizeof(ME_SOCK_ADDR);
}

BOOL CInetAddress::IsLocalHost() const
{
	return (::htonl(INADDR_LOOPBACK) == m_SockAddr.sin_addr.s_addr);
}

const ME_SOCK_ADDR* CInetAddress::GetRawAddress() const
{
	return &m_SockAddr;
}

ME_Result CInetAddress::SetIP(
	DWORD dwIP, 
	BOOL bIsNetworkOrder /* = FALSE  */ )
{
	if ( FALSE == bIsNetworkOrder )
	{
		m_SockAddr.sin_addr.s_addr = ::htonl( dwIP );
	}
	else
	{
		m_SockAddr.sin_addr.s_addr = dwIP;
	}

	return 0;
}

ME_Result CInetAddress::SetIP( const char* pszIP )
{
	DWORD dwIP = OS::IpStringToDigital( pszIP );

	if ( INADDR_NONE != dwIP )
	{
		return SetIP( dwIP, TRUE );
	}

	return ME_ERROR_FAILURE;
}

ME_Result CInetAddress::Set( 
	const char *pszHostName, 
	WORD wPort )
{
	::memset(
		&m_SockAddr,
		0,
		sizeof(m_SockAddr) );

	m_SockAddr.sin_family = AF_INET;

	/**
	* 很有可能是变量刚刚声明出来, 构造函数中调用的Set
	* 所以不用asserte出来, 直接return就好了
	*/
	if ( NULL == pszHostName )
	{
		return ME_OK;
	}

	m_SockAddr.sin_port = ::htons( wPort );

	return SetIP( pszHostName );
}

ME_Result CInetAddress::Set( const char* pszHostNameAndPort )
{
	if ( NULL == pszHostNameAndPort )
	{
		return ME_ERROR_NULL_POINTER;
	}

#ifdef ME_DEBUG
	ME_ASSERTE_RETURN( 
		::strnlen(pszHostNameAndPort, MAX_IP_AND_PORT_LENGTH) < MAX_IP_AND_PORT_LENGTH,
		ME_ERROR_NOT_AVAILABLE );

#endif	// ME_DEBUG

	DWORD dwLength = ::strnlen(
		pszHostNameAndPort, 
		MAX_IP_AND_PORT_LENGTH);

	WORD wPort = 0;

	/* 查找":", ":"之前都算是ip */
	char *szFind = const_cast<char*>(::strchr(pszHostNameAndPort, ':'));
	if ( NULL == szFind) 
	{
		ME_ERROR_TRACE( "unknow address = " << pszHostNameAndPort );

		szFind = const_cast<char*>(pszHostNameAndPort) + dwLength;

		return ME_ERROR_FAILURE;
	}
	else 
	{
		/* ":"之后的都算是端口 */
		if ( (MAX_PORT_NUM >= static_cast<WORD>(::atoi(szFind + 1))) &&
			(0 <= static_cast<WORD>(::atoi(szFind + 1))) )
		{
			wPort = static_cast<WORD>(::atoi(szFind + 1));
		}
		else
		{
			return ME_ERROR_FAILURE;
		}
	}

	/* 256个字节的buf足矣, 从来没有碰到过域名或ip加上端口能超过128字节的 */
	char szBuf[MAX_IP_AND_PORT_LENGTH];
	::memset(
		szBuf,
		0,
		MAX_IP_AND_PORT_LENGTH );

	int iAddrLen = szFind - pszHostNameAndPort;
	ME_ASSERTE_RETURN( ((DWORD)iAddrLen < sizeof(szBuf)), ME_ERROR_NOT_AVAILABLE );

	::memcpy(
		szBuf,
		pszHostNameAndPort,
		iAddrLen );

	return Set(
		szBuf, 
		wPort );
}

ME_Result CInetAddress::Set( const ME_SOCK_ADDR& hsaAddress )
{
	::memcpy( &m_SockAddr, &hsaAddress, sizeof(ME_SOCK_ADDR) );

	return ME_OK;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */BOOL CInetAddress::operator == ( const CInetAddress &DstAddress ) const
{
	/** 地址对比不包括sin_zero是因为, 在使用了类似getpeername()或getsockname()
	* 之后, 函数会填充sin_zero这个数组, 而且对比这个数组也没什么意义
	*/
	return ( 0 == ::memcmp(
		&m_SockAddr,
		&DstAddress.m_SockAddr,
		sizeof(m_SockAddr) - sizeof(m_SockAddr.sin_zero)) ? TRUE : FALSE );
}

ME_INLINE BOOL CInetAddress::operator < ( const CInetAddress &DstAddress ) const
{
	/* < 操作, 先对比地址, 如果地址相同再对比端口 */
	return ( m_SockAddr.sin_addr.s_addr < DstAddress.m_SockAddr.sin_addr.s_addr || 
		((m_SockAddr.sin_addr.s_addr == DstAddress.m_SockAddr.sin_addr.s_addr) && (m_SockAddr.sin_port < DstAddress.m_SockAddr.sin_port)) );
}

LONG CInetAddress::HashCompare::GetHashValue( const CInetAddress& Src ) const
{
	return (LONG)(Src.m_SockAddr.sin_addr.s_addr ^ ME_HASH_SEED);
}

BOOL CInetAddress::HashCompare::operator () ( 
	const CInetAddress& Src, 
	const CInetAddress& Dst ) const
{
	return (Src < Dst);
}

LONG CInetAddress::HashCompare::operator () ( const CInetAddress& Dst ) const
{
	/* 把<lQuot>限制在一个long的最大值以内 */
	long lQuot = GetHashValue(Dst) & ME_LONG_MAX;
	ldiv_t lQrem = ::ldiv( lQuot, ME_RANDOM_DENOMINATOR );

	/* 随便赋值 */
	lQrem.rem = ME_RANDOM_QREM * lQrem.rem - ME_RANDOM_QUOT * lQrem.quot;

	/* 如果变成负数 */
	if ( 0 > lQrem.rem )
	{
		lQrem.rem += ME_LONG_MAX;
	}

	return ((LONG)lQrem.rem);
}

ME_NAME_SPACE_END











