#include "ConnectorFactory.h"
#include "ConnectionInterface.h"
#include "ConnectorImpl.h"
#include "ConnectorTcp.h"
#include "ConnectorUdp.h"
#include "ThreadManager.h"
#include "Thread.h"
#include "Reactor.h"
#include "ConnectorAsyncTcp.h"
#include "ConnectorAsyncUdp.h"

ME_NAME_SPACE_BEGIN


ME_Result CMEConnectorFactory::Create( 
	DWORD dwType,
	IMEConnectorImpl*& pConnector )
{

	CMEThread* pThread = (CMEThread*)CMEThreadManagerSingleton::Instance()->GetNetworkThread();
	if ( pThread && ME_BIT_ENABLED(pThread->GetReactor()->GetType(), CMEReactor::REACTOR_TYPE_SYNC) )
	{
		return CreateSync(
			dwType,
			pConnector );
	}
	else
	{
		return CreateAsync(
			dwType,
			pConnector );
	}
}

ME_Result CMEConnectorFactory::CreateSync( 
	DWORD dwType, 
	IMEConnectorImpl*& pConnector )
{
	/* TCP */
	if ( ME_BIT_ENABLED(dwType, IMEConnectionManager::CONNECTION_TYPE_TCP) )
	{
		pConnector = new CMEConnectorTcp;
	}

	/* UDP */
	else if ( ME_BIT_ENABLED(dwType, IMEConnectionManager::CONNECTION_TYPE_UDP) )
	{
		pConnector = new CMEConnectorUdp;
	}
	else
	{
		/* 只可能有这两种，其他的，通通asserte */
		ME_ASSERTE_RETURN( FALSE, ME_ERROR_INVALID_ARG );
	}

	return ME_OK;
}

ME_Result CMEConnectorFactory::CreateAsync( 
	DWORD dwType, 
	IMEConnectorImpl*& pConnector )
{
	/* TCP */
	if ( ME_BIT_ENABLED(dwType, IMEConnectionManager::CONNECTION_TYPE_TCP) )
	{
		pConnector = new CMEConnectorAsyncTcp;
	}

	/* UDP */
	else if ( ME_BIT_ENABLED(dwType, IMEConnectionManager::CONNECTION_TYPE_UDP) )
	{
		pConnector = new CMEConnectorAsyncUdp;
	}
	else
	{
		/* 只可能有这两种，其他的，通通asserte */
		ME_ASSERTE_RETURN( FALSE, ME_ERROR_INVALID_ARG );
	}

	return ME_OK;
}

ME_NAME_SPACE_END

