#include "IPBase.h"
#include "OSSocket.h"

ME_NAME_SPACE_BEGIN

CMEIPBase::CMEIPBase()
	: m_hHandle( ME_INVALID_HANDLE )
{

}

CMEIPBase::CMEIPBase( ME_HANDLE hHandle )
	: m_hHandle( hHandle )
{

}

CMEIPBase::~CMEIPBase()
{

}

void CMEIPBase::SetHandle( ME_HANDLE hHandle )
{
	m_hHandle = hHandle;
}

ME_HANDLE CMEIPBase::GetHandle() const
{
	return m_hHandle;
}

ME_Result CMEIPBase::Enable( int iValue )
{
	int iResult = ME_ERROR_FAILURE;
	switch ( iValue )
	{
	case NON_BLOCK:
		{
#ifdef ME_WIN
			DWORD dwNonBlock = 1;
			iResult = ::ioctlsocket(
				(ME_SOCKET)m_hHandle,
				FIONBIO,
				&dwNonBlock );

			if ( SOCKET_ERROR == iResult )
			{
				errno = ::WSAGetLastError();
			}

			return iResult;

#elif defined( ME_LINUX )
			iResult = ::fcntl(
				m_hHandle,
				F_GETFL,
				0 );

			if ( SOCKET_ERROR == iResult )
			{
				return SOCKET_ERROR;
			}

			ME_SET_BITS( iResult, O_NONBLOCK );

			iResult = ::fcntl(
				m_hHandle,
				F_SETFL,
				iResult );

			if ( SOCKET_ERROR == iResult )
			{
				return SOCKET_ERROR;
			}

			return iResult;

#endif	// ME_WIN
		}

		/**
		* 其他option接着往下写
		*
		* 目前，有NON_BLOCK就够了，其他以后再补
		*/

	default:
		{
			ME_ERROR_TRACE_THIS( "unknow value = " << iValue );

			return SOCKET_ERROR;
		}
	} 
}

ME_Result CMEIPBase::Disable( int iValue )
{
	int iResult = ME_OK;
	switch ( iValue )
	{
	case NON_BLOCK:
		{
#ifdef ME_WIN
			DWORD dwNonBlock = 0;
			iResult = ::ioctlsocket(
				(ME_SOCKET)m_hHandle,
				FIONBIO,
				&dwNonBlock );

			if ( SOCKET_ERROR == iResult )
			{
				errno = ::WSAGetLastError();
			}

			return iResult;

#elif defined( ME_LINUX )
			iResult = ::fcntl(
				m_hHandle,
				F_GETFL,
				0 );

			if ( SOCKET_ERROR == iResult )
			{
				return SOCKET_ERROR;
			}

			ME_CLR_BITS( iResult, O_NONBLOCK );

			iResult = ::fcntl(
				m_hHandle,
				F_SETFL,
				iResult );

			if ( SOCKET_ERROR == iResult )
			{
				return SOCKET_ERROR;
			}

			return iResult;

#endif	// ME_WIN
		}

		/**
		* 其他option接着往下写
		*
		* 目前，有NON_BLOCK就够了，其他以后再补
		*/

	default:
		{
			ME_ERROR_TRACE_THIS( "unknow value = " << iValue );

			return SOCKET_ERROR;
		}
	}
}

ME_Result CMEIPBase::Control( 
	int iCmd, 
	void* pArg )
{
	ME_ASSERTE_RETURN( pArg, ME_ERROR_NULL_POINTER );

	return OS::IOCtrl(
		(ME_SOCKET)m_hHandle,
		iCmd,
		pArg );
}

ME_NAME_SPACE_END


