#include "TransportDesigner.h"
#include "TransportBuilder.h"
#include "TransportOperatorPackage.h"
#include "TransportOperatorKeepAlive.h"
#include "ConnectionInterface.h"
#include "InetAddress.h"
#include "TransportOperatorSSL.h"

ME_NAME_SPACE_BEGIN

CMETransportDesigner::CMETransportDesigner( 
	DWORD dwType, 
	ME_HANDLE hHandle, 
	CMEInetAddress* pPeerAddress /* = NULL */ )
	: m_dwType( dwType )
	, m_hHandle( hHandle )
	, m_pPeerAddress( pPeerAddress )
{

}

CMETransportDesigner::~CMETransportDesigner()
{

}

ME_Result CMETransportDesigner::Build( IMETransportBuilder* pBuilder )
{
	ME_ASSERTE_RETURN( pBuilder, ME_ERROR_INVALID_ARG );

	ME_Result hResult = pBuilder->Build(
		m_dwType,
		m_hHandle );

	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	do 
	{
		/* 如果不需要任何处理器的 */
		if ( ME_BIT_DISABLED_REVERSE(m_dwType, IMEConnectionManager::CONNECTION_TYPE_TCP | IMEConnectionManager::CONNECTION_TYPE_UDP) )
		{
			break;
		}

        /* Operator按照注册顺序，注册顺序越靠前，接收时越先调用，发送时越后调用 */
		IMETransportOperator* pOperator = NULL;

        /* 如果要启用SSL，那么TransportOperatorSSL必须首先build */
        if ( ME_BIT_ENABLED(m_dwType, IMEConnectionManager::CONNECTION_TYPE_SSL) )
        {
            pOperator = new CMETransportOperatorSSL();
            hResult = pBuilder->Build( pOperator );
            if ( ME_FAILED(hResult) )
            {
                ME_DELETE( pOperator );
            }
        }

		/* 如果启用数据整理，那么TransportOperatorPackage在SSL之后就要build */
		if ( ME_BIT_ENABLED(m_dwType, IMEConnectionManager::CONNECTION_TYPE_LENGTH) )
		{
			pOperator = new CMETransportOperatorPackage;
			hResult = pBuilder->Build( pOperator );
			if ( ME_FAILED(hResult) )
			{
				ME_DELETE( pOperator );
			}
		}

		/* 继续在这里往下写，或者继承这个Designer，设计一个你自己的设计器 */
        if ( ME_BIT_ENABLED(m_dwType, IMEConnectionManager::CONNECTION_TYPE_KEEP_ALIVE) )
        {
            pOperator = new CMETransportOperatorKeepAlive( pBuilder->GetResult() );
            hResult = pBuilder->Build( pOperator );
            if ( ME_FAILED(hResult) )
            {
                ME_DELETE( pOperator );
            }
        }

	} while ( 0 );

	if ( m_pPeerAddress )
	{
		hResult = pBuilder->Build( *m_pPeerAddress );

		ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );
	}

	return hResult;
}

CMETransportCrossThreadDesigner::CMETransportCrossThreadDesigner( IMETransport* pSrcTransport )
	: m_pSrcTransport( pSrcTransport )
{

}

CMETransportCrossThreadDesigner::~CMETransportCrossThreadDesigner()
{

}

ME_Result CMETransportCrossThreadDesigner::Build( IMETransportBuilder* pBuilder )
{
	ME_ASSERTE_RETURN( pBuilder, ME_ERROR_INVALID_ARG );

	return pBuilder->Build( m_pSrcTransport );
}

ME_NAME_SPACE_END


