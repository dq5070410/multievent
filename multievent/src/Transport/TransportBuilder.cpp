#include "TransportBuilder.h"
#include "ConnectionInterface.h"
#include "TransportWrapper.h"
#include "TransportOperator.h"
#include "TransportCrossThread.h"

ME_NAME_SPACE_BEGIN

CMETransportBuilder::~CMETransportBuilder()
{

}

ME_Result CMETransportBuilder::Build( 
	DWORD dwType,
	ME_HANDLE hHandle )
{
	if ( m_pTransport )
	{
		ME_ERROR_TRACE_THIS( "transport has been already initialize, transport = " << m_pTransport );

		return ME_ERROR_ALREADY_INITIALIZED;
	}

	ME_Result hResult = ME_ERROR_NOT_FOUND;

	if ( ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_TCP) ||
		 ME_BIT_ENABLED(dwType, IConnectionManager::CONNECTION_TYPE_UDP) )
	{
		CMETransportWrapper* pWrapper = new CMETransportWrapper;
		hResult = pWrapper->Initialize(
			dwType, 
			hHandle );

		if ( ME_FAILED(hResult) )
		{
			ME_DELETE( pWrapper );

			return hResult;
		}

		m_pTransport = pWrapper;
	}

	return hResult;
}

ME_Result CMETransportBuilder::Build( ITransport* pTransport )
{
	if ( m_pTransport )
	{
		ME_ERROR_TRACE_THIS( "transport has been already initialize, transport = " << m_pTransport );

		return ME_ERROR_ALREADY_INITIALIZED;
	}

	CMETransportCrossThread* pTransportCrossThread = new CMETransportCrossThread( pTransport );
	m_pTransport = pTransportCrossThread;

	return ME_OK;
}

ME_Result CMETransportBuilder::Build( const CMEInetAddress& peerAddress )
{
	ME_ASSERTE_RETURN( m_pTransport, ME_ERROR_NULL_POINTER );

	return m_pTransport->SetOption(
		ITransport::OPTION_TYPE_PEER_ADDRESS,
		(void*)&peerAddress );
}

ME_Result CMETransportBuilder::Build( IMETransportOperator* pOperator )
{
	ME_ASSERTE_RETURN( pOperator && m_pTransport, ME_ERROR_NULL_POINTER );

	return ((CMETransportWrapper*)m_pTransport.ParaIn())->RegisterOperator( pOperator );
}

ITransport* CMETransportBuilder::GetResult()
{
	return m_pTransport.ParaIn();
}

ME_NAME_SPACE_END












