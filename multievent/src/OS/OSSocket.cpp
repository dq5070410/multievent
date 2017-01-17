#include "OSSocket.h"
#include <iostream>
using namespace std;

NAME_SPACE_OS_BEGIN

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result SocketOpen( 
	int iFamily, 
	int iType, 
	int iProtocol,
	ME_HANDLE& Handle )
{
	Handle = (ME_HANDLE)::socket(
		iFamily,
		iType,
		iProtocol );

	return ( ME_INVALID_HANDLE == Handle ? SOCKET_ERROR : ME_OK );
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result SocketClose( ME_HANDLE Handle )
{
	int iResult = 0;

#ifdef ME_WIN
	iResult = ::closesocket( (ME_SOCKET)Handle );

#elif defined( ME_LINUX )
	iResult = ::close( (ME_SOCKET)Handle );

#endif	// ME_WIN

	if ( SOCKET_ERROR == iResult ) 
	{
#ifdef ME_WIN
		errno = ::WSAGetLastError();

#endif	// ME_WIN
	}

	return iResult;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */int SocketSend(
	ME_HANDLE Handle,
	const char* pszData,
	DWORD dwLength,
	int iFlag /* = 0 */ )
{
	int iResult = ::send(
		(ME_SOCKET)Handle,
		pszData,
		dwLength,
		iFlag );

#ifdef ME_WIN
	if ( SOCKET_ERROR == iResult ) 
	{
		errno = ::WSAGetLastError();
	}

#endif	// ME_WIN

	return iResult;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */int SocketSendV(
	ME_HANDLE Handle,
	const iovec pszIov[],
	DWORD dwCount )
{
	int iResult = 0;

#ifdef ME_WIN
	DWORD dwBytesSend = 0;
	iResult = ::WSASend(
		(ME_SOCKET)Handle,
		(WSABUF*)pszIov,
		dwCount,
		&dwBytesSend,
		0,
		NULL,
		NULL );

	if ( SOCKET_ERROR == iResult ) 
	{
		errno = ::WSAGetLastError();
	}
	else 
	{
		iResult = (int)dwBytesSend;
	}

#elif defined( ME_LINUX )
	iResult = ::writev(
		Handle, 
		pszIov, 
		dwCount );

#endif // ME_WIN

	return iResult;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */int SocketRecv(
	ME_HANDLE Handle,
	char*& pszBuf,
	DWORD dwLength,
	int iFlag /* = 0 */ )
{
	int iResult = ::recv(
		(ME_SOCKET)Handle,
		pszBuf,
		dwLength,
		iFlag );

#ifdef ME_WIN
	if ( SOCKET_ERROR == iResult ) 
	{
		errno = ::WSAGetLastError();
	}

#endif	// ME_WIN

	return iResult;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */int SocketRecvV(
	ME_HANDLE Handle,
	iovec szIov[],
	DWORD dwCount )
{
	int iResult = 0;

#ifdef ME_WIN
	DWORD dwBytesRecv = 0;
	DWORD dwFlag = 0;

	iResult = ::WSARecv(
		(ME_SOCKET)Handle,
		(WSABUF*)szIov,
		dwCount,
		&dwBytesRecv,
		&dwFlag,
		NULL,
		NULL );

	if ( SOCKET_ERROR == iResult ) 
	{
		errno = ::WSAGetLastError();
	}
	else 
	{
		iResult = (int)dwBytesRecv;
	}

#elif defined( ME_LINUX )
	iResult = ::readv(
		Handle,
		szIov,
		dwCount );

#endif	// ME_WIN

	return iResult;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */int SocketSendTo(
	ME_HANDLE Handle,
	const char* pszData,
	DWORD dwLength,
	const ME_SOCK_ADDR* pPeerAddress,
	int iFlag /* = 0 */ )
{
	int iResult = ::sendto(
		(ME_SOCKET)Handle,
		pszData,
		dwLength,
		iFlag,
		reinterpret_cast<const sockaddr*>(pPeerAddress),
		(ME_SOCK_LEN)sizeof(ME_SOCK_ADDR) );

	if ( SOCKET_ERROR == iResult )
	{
#ifdef ME_WIN
		errno = ::WSAGetLastError();

#endif	// ME_WIN
	}

	return iResult;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */int SocketSendVTo(
	ME_HANDLE Handle,
	const iovec pszIov[],
	DWORD dwCount,
	const ME_SOCK_ADDR* pPeerAddress )
{
	int iResult = 0;

#ifdef ME_WIN
	DWORD dwBytesSend = 0;
	iResult = ::WSASendTo(
		(ME_SOCKET)Handle,
		(WSABUF*)pszIov,
		dwCount,
		&dwBytesSend,
		0,
		reinterpret_cast<const sockaddr*>(pPeerAddress),
		(ME_SOCK_LEN)sizeof(ME_SOCK_ADDR),
		NULL,
		NULL);

	if ( SOCKET_ERROR == iResult ) 
	{
		errno = ::WSAGetLastError();
	}
	else 
	{
		iResult = (int)dwBytesSend;
	}

#elif defined( ME_LINUX )
	msghdr msgData;
	msgData.msg_iov = (iovec*)pszIov;
	msgData.msg_iovlen = dwCount;	
	msgData.msg_name = (sockaddr*)pPeerAddress;
	msgData.msg_namelen = (ME_SOCK_LEN)sizeof(ME_SOCK_ADDR);
	msgData.msg_control = 0;
	msgData.msg_controllen = 0;
	msgData.msg_flags = 0;

	iResult = ::sendmsg(
		(ME_SOCKET)Handle,
		&msgData,
		0 );

#endif // ME_WIN

	return iResult;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */int SocketRecvFrom(
	ME_HANDLE Handle,
	char*& pszBuf,
	DWORD dwLength,
	ME_SOCK_ADDR& hsaPeerAddress,
	int iFlag /* = 0 */ )
{
	ME_SOCK_LEN iSize = (ME_SOCK_LEN)sizeof(ME_SOCK_ADDR);

	int iResult = ::recvfrom(
		(ME_SOCKET)Handle,
		pszBuf,
		dwLength,
		iFlag,
		reinterpret_cast<sockaddr*>(&hsaPeerAddress),
#ifdef ME_WIN
		&iSize

#elif defined( ME_LINUX )
	(ME_SOCK_LEN*)(&iSize)

#endif	// ME_WIN
		);

	if ( SOCKET_ERROR == iResult )
	{
#ifdef ME_WIN
		errno = ::WSAGetLastError();

#endif	// ME_WIN
	}

	return iResult;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result SocketBind(
	ME_HANDLE Handle,
	const ME_SOCK_ADDR* pLocalAddrYouWantToBind )
{
	int iResult = ::bind(
		(ME_SOCKET)Handle, 
		reinterpret_cast<const sockaddr*>(pLocalAddrYouWantToBind),
		(ME_SOCK_LEN)sizeof(ME_SOCK_ADDR) );

	if ( SOCKET_ERROR == iResult )
	{
#ifdef ME_WIN
		errno = ::WSAGetLastError();

#endif // CM_WIN32
	}

	return iResult;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result SocketShutdown(
	ME_HANDLE Handle,
	int iFlag )
{
	int iResult = ::shutdown(
		(ME_SOCKET)Handle,
		iFlag );

	if ( SOCKET_ERROR == iResult )
	{
#ifdef ME_WIN
		errno = ::WSAGetLastError();

#endif	// ME_WIN
	}

	return iResult;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result SocketConnect(
	ME_HANDLE Handle,
	const ME_SOCK_ADDR* pPeerAddress )
{
	int iResult = ::connect(
		(ME_SOCKET)Handle,
		reinterpret_cast<const sockaddr*>(pPeerAddress),
		(ME_SOCK_LEN)sizeof(ME_SOCK_ADDR) );

	if ( SOCKET_ERROR == iResult )
	{
#ifdef ME_WIN
		errno = ::WSAGetLastError();

#else
		if ( EINPROGRESS == errno )
		{
			errno = EWOULDBLOCK;
		}
		else if ( EINTR == errno )
		{
			cout << " ++++++++++++++++++++++ FUCK +++++++++++++++++++++++ " << endl;

			return ME_OK;
		}

#endif	// ME_WIN
	}

	return iResult;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result SocketListen( ME_HANDLE Handle )
{
	int iResult = ::listen(
		(ME_SOCKET)Handle,
		SOMAXCONN );

	if ( SOCKET_ERROR == iResult )
	{
#ifdef ME_WIN
		errno = ::WSAGetLastError();

#else
		if ( EINPROGRESS == errno )
		{
			errno = EWOULDBLOCK;
		}

#endif	// ME_WIN
	}

	return iResult;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_HANDLE SocketAccept(
	ME_HANDLE Handle,
	ME_SOCK_ADDR& hsaPeerAddress )
{
	ME_SOCK_LEN iSize = (ME_SOCK_LEN)sizeof(ME_SOCK_ADDR);
	ME_HANDLE hhHandle = (ME_HANDLE)::accept(
		(ME_SOCKET)Handle,
		reinterpret_cast<sockaddr*>(&hsaPeerAddress),
#ifdef ME_WIN
		&iSize

#elif defined( ME_LINUX )
		(ME_SOCK_LEN*)(&iSize)

#endif	// ME_WIN
		);

	if ( ME_INVALID_HANDLE == hhHandle )
	{
#ifdef ME_WIN
		errno = ::WSAGetLastError();

#else
		if ( EINPROGRESS == errno )
		{
			errno = EWOULDBLOCK;
		}

#endif	// ME_WIN
	}

	return hhHandle;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */CMEString IpDigitalToString( DWORD dwIP )
{
	struct in_addr AddrIn;
	AddrIn.s_addr = dwIP;
	const char* pAddr = ::inet_ntoa( AddrIn );	// 用inet_pton

	return CMEString( pAddr );
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */DWORD IpStringToDigital( const char* pszIP )
{
	DWORD dwIP = INADDR_ANY;
#ifdef ME_WIN
	dwIP = ::inet_addr( pszIP );

#elif defined( ME_LINUX )
	::inet_pton( 
		AF_INET,
		pszIP,
		&dwIP );

#endif	// ME_WIN

	return dwIP;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result IOCtrl(
	ME_SOCKET Handle,
	int iFlag,
	void* pArg )
{
#ifdef ME_WIN
	return ::ioctlsocket(
		Handle, 
		iFlag, 
		reinterpret_cast<DWORD*>(pArg) );

#elif defined( ME_LINUX )
	return ::ioctl(
		Handle, 
		iFlag, 
		pArg );

#endif	// ME_WIN
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result SocketGetLocalAddress(
	ME_HANDLE Handle,
	ME_SOCK_ADDR& hsaLocalAddress )
{
	ME_SOCK_LEN iSize = (ME_SOCK_LEN)sizeof(ME_SOCK_ADDR);
	int iResult = ::getsockname(
		(ME_SOCKET)Handle,
		reinterpret_cast<sockaddr*>(&hsaLocalAddress),

#ifdef ME_WIN
		&iSize

#elif defined( ME_LINUX )
		(ME_SOCK_LEN*)(&iSize)

#endif	// ME_WIN
		);

	if ( SOCKET_ERROR == iResult ) 
	{
#ifdef ME_WIN
		errno = ::WSAGetLastError();

#endif	// ME_WIN
	}

	return iResult;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result SocketGetRemoteAddress(
	ME_HANDLE Handle,
	ME_SOCK_ADDR& hsaRemoteAddress )
{
	ME_SOCK_LEN iSize = (ME_SOCK_LEN)sizeof(ME_SOCK_ADDR);
	int iResult = ::getpeername(
		(ME_SOCKET)Handle,
		reinterpret_cast<sockaddr*>(&hsaRemoteAddress),
#ifdef ME_WIN
		&iSize
#elif defined( ME_LINUX )
		(ME_SOCK_LEN*)(&iSize)
#endif	// ME_WIN
		);

	if ( SOCKET_ERROR == iResult ) 
	{
#ifdef ME_WIN
		errno = ::WSAGetLastError();

#endif	// ME_WIN
	}

	return iResult;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result SocketSetSocketOption(
	ME_HANDLE Handle,
	int iLevel, 
	int iOption, 
	const void* pOptval, 
	int iOptlen )
{
	int iResult = ::setsockopt(
		(ME_SOCKET)Handle, 
		iLevel, 
		iOption, 
#ifdef ME_WIN
		static_cast<const char*>(pOptval), 

#elif defined( ME_LINUX )
		pOptval,

#endif // ME_WIN
		(ME_SOCK_LEN)iOptlen );

	if ( SOCKET_ERROR == iResult ) 
	{
#ifdef ME_WIN
		errno = ::WSAGetLastError();

#endif	// ME_WIN
	}

	return iResult;
}

/* 2013.4.12 让gcc能够正确识别 */
/*ME_INLINE */ME_Result SocketGetSocketOption(
	ME_HANDLE Handle,
	int iLevel, 
	int iOption, 
	void* pOptval, 
	int* pOptlen )
{
	int iResult = ::getsockopt(
		(ME_SOCKET)Handle, 
		iLevel, 
		iOption, 

#ifdef ME_WIN
		static_cast<char*>(pOptval), 
		pOptlen

#elif defined( ME_LINUX )
		pOptval,	
		(ME_SOCK_LEN*)(pOptlen)

#endif // ME_WIN
		);

	if ( SOCKET_ERROR == iResult ) 
	{
#ifdef ME_WIN
		errno = ::WSAGetLastError();

#endif	// ME_WIN
	}

	return iResult;
}

NAME_SPACE_OS_END





