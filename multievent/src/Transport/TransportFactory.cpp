#include "TransportFactory.h"
#include "ConnectionInterface.h"
#include "TransportWrapper.h"
#include "TransportBuilder.h"
#include "TransportDesigner.h"

ME_NAME_SPACE_BEGIN

ME_Result CMETransportFactory::Create( 
	DWORD dwType, 
	ME_HANDLE hHandle, 
	IMETransport*& pTransport, 
	CMEInetAddress* pPeerAddress /* = NULL */ )
{
	ME_ASSERTE_RETURN( hHandle && (ME_INVALID_HANDLE != hHandle), ME_ERROR_INVALID_ARG );
	ME_ASSERTE_RETURN( ME_BIT_ENABLED(dwType, IMEConnectionManager::CONNECTION_TYPE_TCP) ||
						ME_BIT_ENABLED(dwType, IMEConnectionManager::CONNECTION_TYPE_UDP),
						ME_ERROR_NOT_FOUND );

	IMETransportBuilder* pBuilder = new CMETransportBuilder;
	IMETransportDesigner* pDesigner = new CMETransportDesigner(
		dwType,
		hHandle,
		pPeerAddress );

	ME_Result hResult = ME_OK;
	do 
	{
		hResult = pDesigner->Build( pBuilder );
		if ( ME_FAILED(hResult) )
		{
			break;
		}

		pTransport = pBuilder->GetResult();
		pTransport->AddReference();

	} while ( 0 );

	ME_DELETE( pBuilder );
	ME_DELETE( pDesigner );

	return hResult;
}

ME_Result CMETransportFactory::Create( 
	IMETransport* pSrcTransport, 
	IMETransport*& pTransport )
{
	ME_ASSERTE_RETURN( pSrcTransport, ME_ERROR_INVALID_ARG );

	IMETransportBuilder* pBuilder = new CMETransportBuilder;
	IMETransportDesigner* pDesigner = new CMETransportCrossThreadDesigner( pSrcTransport );

	ME_Result hResult = pDesigner->Build( pBuilder );
	pTransport = pBuilder->GetResult();
	pTransport->AddReference();

	ME_DELETE( pBuilder );
	ME_DELETE( pDesigner );

	return hResult;
}

ME_NAME_SPACE_END



