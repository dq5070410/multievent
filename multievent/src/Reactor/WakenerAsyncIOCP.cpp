#include "WakenerAsyncIOCP.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

CMEWakenerAsyncIOCP::CMEWakenerAsyncIOCP( ME_HANDLE hInterface )
	: m_hIOCP( hInterface )
{

}

CMEWakenerAsyncIOCP::~CMEWakenerAsyncIOCP()
{
	::CloseHandle( m_hIOCP );
}

void CMEWakenerAsyncIOCP::SetHandle( ME_HANDLE hHandle )
{

}

ME_HANDLE CMEWakenerAsyncIOCP::GetHandle() const
{
	return NULL;
}

ME_Result CMEWakenerAsyncIOCP::Notify()
{
	return m_asyncNotify.Post(
		this,
		m_hIOCP );
}

ME_Result CMEWakenerAsyncIOCP::Initialize()
{
	return ME_OK;
}

ME_Result CMEWakenerAsyncIOCP::Finalize()
{
	return ME_OK;
}

void CMEWakenerAsyncIOCP::OnInput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	
}

void CMEWakenerAsyncIOCP::OnOutput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	ME_ASSERTE( FALSE );
}

void CMEWakenerAsyncIOCP::OnClose( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	ME_ASSERTE( FALSE );
}


ME_NAME_SPACE_END

#endif	// ME_WIN

