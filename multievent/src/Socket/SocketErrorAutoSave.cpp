#include "SocketErrorAutoSave.h"

CMESocketErrorAutoSave::CMESocketErrorAutoSave()
#ifdef ME_WIN
	: m_iError( ::WSAGetLastError() )
#else
	: m_iError( errno )
#endif	// ME_WIN
{
}

CMESocketErrorAutoSave::~CMESocketErrorAutoSave()
{
	errno = m_iError;
}

int CMESocketErrorAutoSave::GetLastError() const
{
	return m_iError;
}


