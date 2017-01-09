#include "AcceptorFactory.h"
#include "ConnectionInterface.h"
#include "AcceptorImpl.h"
#include "AcceptorTcp.h"
#include "AcceptorUdp.h"
#include "ThreadManager.h"
#include "Thread.h"
#include "Reactor.h"
#include "WakenerAsyncIOCP.h"
#include "AcceptorAsyncTcp.h"
#include "AcceptorAsyncUdp.h"

ME_NAME_SPACE_BEGIN

ME_Result CMEAcceptorFactory::Create( 
	DWORD dwType, 
	IMEAcceptorImpl*& pAcceptor )
{
	CMEThread* pThread = (CMEThread*)CMEThreadManagerSingleton::Instance()->GetNetworkThread();
	if ( pThread && ME_BIT_ENABLED(pThread->GetReactor()->GetType(), CMEReactor::REACTOR_TYPE_SYNC) )
	{
		return CreateSync(
			dwType,
			pAcceptor );
	}
	else
	{
		return CreateAsync(
			dwType,
			pAcceptor );
	}
}

ME_Result CMEAcceptorFactory::CreateSync( 
	DWORD dwType, 
	IMEAcceptorImpl*& pAcceptor )
{
	/* TCP */
	if ( ME_BIT_ENABLED(dwType, IMEConnectionManager::CONNECTION_TYPE_TCP) )
	{
		pAcceptor = new CMEAcceptorTcp;
	}

	/* UDP */
	else if ( ME_BIT_ENABLED(dwType, IMEConnectionManager::CONNECTION_TYPE_UDP) )
	{
		pAcceptor = new CMEAcceptorUdp;
	}
	else
	{
		/* 只可能有这两种，其他的，通通asserte */
		ME_ASSERTE_RETURN( FALSE, ME_ERROR_INVALID_ARG );
	}

	return ME_OK;
}

ME_Result CMEAcceptorFactory::CreateAsync( 
	DWORD dwType, 
	IMEAcceptorImpl*& pAcceptor )
{
	/* TCP */
	if ( ME_BIT_ENABLED(dwType, IMEConnectionManager::CONNECTION_TYPE_TCP) )
	{
		pAcceptor = new CMEAcceptorAsyncTcp;
	}

	/* UDP */
	else if ( ME_BIT_ENABLED(dwType, IMEConnectionManager::CONNECTION_TYPE_UDP) )
	{
		pAcceptor = new CMEAcceptorAsyncUdp;
	}
	else
	{
		/* 只可能有这两种，其他的，通通asserte */
		ME_ASSERTE_RETURN( FALSE, ME_ERROR_INVALID_ARG );
	}

	return ME_OK;
}

ME_NAME_SPACE_END




