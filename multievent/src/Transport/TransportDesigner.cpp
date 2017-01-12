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
		/* �������Ҫ�κδ������� */
		if ( ME_BIT_DISABLED_REVERSE(m_dwType, IMEConnectionManager::CONNECTION_TYPE_TCP | IMEConnectionManager::CONNECTION_TYPE_UDP) )
		{
			break;
		}

        /* Operator����ע��˳��ע��˳��Խ��ǰ������ʱԽ�ȵ��ã�����ʱԽ����� */
		IMETransportOperator* pOperator = NULL;

        /* ���Ҫ����SSL����ôTransportOperatorSSL��������build */
        if ( ME_BIT_ENABLED(m_dwType, IMEConnectionManager::CONNECTION_TYPE_SSL) )
        {
            pOperator = new CMETransportOperatorSSL();
            hResult = pBuilder->Build( pOperator );
            if ( ME_FAILED(hResult) )
            {
                ME_DELETE( pOperator );
            }
        }

		/* �����������������ôTransportOperatorPackage��SSL֮���Ҫbuild */
		if ( ME_BIT_ENABLED(m_dwType, IMEConnectionManager::CONNECTION_TYPE_LENGTH) )
		{
			pOperator = new CMETransportOperatorPackage;
			hResult = pBuilder->Build( pOperator );
			if ( ME_FAILED(hResult) )
			{
				ME_DELETE( pOperator );
			}
		}

		/* ��������������д�����߼̳����Designer�����һ�����Լ�������� */
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


