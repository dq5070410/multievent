#include "ConnectorWrapper.h"
#include "AutoPtrT.h"
#include "TransportFactory.h"
#include "ConnectorFactory.h"
#include "TimeValue.h"
#include "OSSocket.h"
#include "ThreadManager.h"
#include "Thread.h"
#include "OSSocket.h"

ME_NAME_SPACE_BEGIN

CMEConnectorWrapper::CMEConnectorWrapper()
	: m_dwType( IConnectionManager::CONNECTION_TYPE_NONE )
	, m_pConnector( NULL )
	, m_pSink( NULL )
	, m_bConnected( FALSE )
	, m_hHandle( NULL )
	, m_pTimerThread( NULL )
{

}

CMEConnectorWrapper::~CMEConnectorWrapper()
{
	//Close();
}

ME_Result CMEConnectorWrapper::Initialize( DWORD dwType )
{
	AddReference();

	IMEConnectorImpl* pConnector = NULL;
	ME_Result hResult = CMEConnectorFactorySingleton::Instance()->Create(
		dwType,
		pConnector );

	if ( ME_FAILED(hResult) )
	{
		ME_DELETE( pConnector );

		return hResult;
	}

	m_dwType = dwType;
	m_pConnector = pConnector;

	RemoveReference();

	return ME_OK;
}

ME_Result CMEConnectorWrapper::Open( 
	IConnectorSink* pSink, 
	const CMEInetAddress& peerAddress, 
	CTimeValue* pTimeout /* = NULL */, 
	CMEInetAddress* pLocalAddress /* = NULL */ )
{
	ME_ASSERTE_RETURN( pSink, ME_ERROR_INVALID_ARG );

	AddReference();

	/**
	* 先保存，以便在OnTimer或OnConnect发生时能够通知上层
	* 如果这里<pSink>是NULL，那么上层就无法得知结果，
	* 因为无论是OnTimer或OnConnect都需要一个非NULL的回调对象
	*/
	m_pSink = pSink;

	ME_Result hResult = ME_ERROR_NOT_INITIALIZED;
	do 
	{
		if ( !m_pConnector )
		{
			break;
		}

		CMEThread* pThread = (CMEThread*)CMEThreadManager::Instance()->GetNetworkThread();
		if ( NULL == pThread )
		{
			break;
		}

		ME_ASSERTE( !m_hHandle );

		hResult = m_pConnector->Open(
			this,
			peerAddress,
			pThread->GetReactor(),
			m_hHandle,
			pLocalAddress );

	} while ( 0 );

	/* 要么是EWOULDBLOCK要么是成功 */
	if ( ME_SUCCEEDED(hResult) )
	{
		m_bConnected = TRUE;
		if ( pTimeout )
		{
			m_Timer.Schedule(
				this,
				CTimeValue(),
				*pTimeout,
				1 );

			/* 由于是在多线程环境下投递时钟，需要保存投递时钟线程，以便后续取消时钟 */
			m_pTimerThread = dynamic_cast<CMEThread*>( IMEThreadManager::Instance()->GetCurrentThread() );
		}

		m_hiaPeerAddress = peerAddress;	// 打开成功再保存对端地址

		RemoveReference();

		/* 没有设置超时时间，那么就死等，直到OnConnect的发生 */
		return ME_OK;
	}
	/* 要么连接失败，要么输入参数非法 */
	else
	{
		OS::SocketClose( m_hHandle );

		m_bConnected = FALSE;

		/* 这里仅仅为了避免在Open()里call OnConnect() */
		m_Timer.Schedule(
			this,
			CTimeValue(),
			1 );
	}

	RemoveReference();

	return hResult;
}

void CMEConnectorWrapper::Close()
{
	AddReference();

	if ( m_pTimerThread )
	{
		m_pTimerThread->CancelTimer( &m_Timer );
	}

	if ( m_pConnector )
	{
		m_pConnector->Close();
		//m_pConnector = NULL;
	}

	m_pSink = NULL;

	RemoveReference();
}

ME_Result CMEConnectorWrapper::RegisterOperator( IMEConnectionOperator* pOperator )
{
    ME_ASSERTE_RETURN( (NULL != pOperator), ME_ERROR_INVALID_ARG );

    OperatorDequeType::iterator iter = m_dequeOperator.begin();
    for ( ; iter != m_dequeOperator.end(); ++iter )
    {
        if ( (*iter)->GetType() == pOperator->GetType() )
        {
            return ME_ERROR_FOUND;
        }
    }

    m_dequeOperator.push_back( IMEConnectionOperatorAutoPtr(pOperator) );

    return ME_OK;
}

ME_Result CMEConnectorWrapper::UnregisterOperator( IMEConnectionOperator* pOperator )
{
    ME_ASSERTE_RETURN( (NULL != pOperator), ME_ERROR_INVALID_ARG );

    OperatorDequeType::iterator iter = m_dequeOperator.begin();
    for ( ; iter != m_dequeOperator.end(); ++iter )
    {
        if ( (*iter) == pOperator )
        {
            m_dequeOperator.erase( iter );

            break;
        }
    }

    return ME_OK;
}

ME_Result CMEConnectorWrapper::SetOption( DWORD dwOptionType, void* pOptionValue )
{
    ME_Result hResult = ME_ERROR_FAILURE;

    switch ( dwOptionType )
    {
    case CONNECTION_OPTION_TYPE_SSL_TYPE:
    case CONNECTION_OPTION_TYPE_CERTIFICATE_FILE:
    case CONNECTION_OPTION_TYPE_PRIVATEKEY_FILE:
    case CONNECTION_OPTION_TYPE_PRIVATEKEY_PASSWORD:
        {
            OperatorDequeType::iterator iter = m_dequeOperator.begin();
            for ( ; iter != m_dequeOperator.end(); ++iter )
            {
                if ( (*iter)->GetType() == IMEConnectionManager::CONNECTION_TYPE_SSL )
                {
                    return (*iter)->SetOption(
                        dwOptionType,
                        pOptionValue);
                }
            }

            break;
        }

    default:
        {
            break;
        }
    }

    return hResult;
}

ME_Result CMEConnectorWrapper::GetOption( WORD dwOptionType, void*& pOptionValue )
{
    return ME_ERROR_FAILURE;
}

DWORD CMEConnectorWrapper::AddReference()
{
	return CMEReferenceControlT<MutexType>::AddReference();
}

DWORD CMEConnectorWrapper::RemoveReference()
{
	return CMEReferenceControlT<MutexType>::RemoveReference();
}

void CMEConnectorWrapper::CloseHandle()
{
	if ( m_hHandle && (ME_INVALID_HANDLE != m_hHandle) )
	{
		OS::SocketClose( m_hHandle );
		m_hHandle = NULL;
	}
}

void CMEConnectorWrapper::OnConnect( 
	ME_Result hResult, 
	ME_HANDLE hHandle )
{
	ME_ASSERTE_RETURN_VOID( m_pSink );

	AddReference();

	IMEConnectorSink* pTmpSink = m_pSink;

	CMEAutoPtrT<IMETransport> pTransport;
	Close();

	/* 连接成功时不需要CloseHandle，因为handle会交付transport管理 */
	if ( ME_SUCCEEDED(hResult) )
	{	
#ifdef ME_DEBUG
		ME_ASSERTE( m_hHandle == hHandle );
#endif	// ME_DEBUG

		/* connector放弃句柄 */
		m_hHandle = NULL;

		/* 交由transport管理 */
		hResult = CMETransportFactorySingleton::Instance()->Create( 
			m_dwType,
			hHandle, 
			pTransport.ParaOut(),
			ME_BIT_ENABLED(m_dwType, IMEConnectionManager::CONNECTION_TYPE_UDP) ? &m_hiaPeerAddress : NULL );

		ME_ASSERTE( ME_SUCCEEDED(hResult) );

        hResult = PreOnConnect(
            hHandle,
            pTransport );

        if ( ME_FAILED(hResult) )
        {
            pTransport = NULL;
        }

		pTmpSink->OnConnect(
			pTransport,
			hResult );
	}
	else
	{
		/* 失败时，因为不会产生transport，handle无人托管，所以就必须在这里close，不然会有句柄泄漏 */
		CloseHandle();

		pTmpSink->OnConnect(
			pTransport,
			ME_ERROR_NETWORK_CONNECT_ERROR );
	}

	RemoveReference();
}

void CMEConnectorWrapper::OnTimer( CMETimer* pTimerID )
{
	/**
	* OnTimer的发生，要么是Open()函数是成功的，但是在超时时间之内OnConnect()没有回来
	* 要么就是，Open()直接就失败了
	*/

	AddReference();

	IConnectorSink* pSink = m_pSink;
	Close();

	/* 同样，因为不会产生transport，handle无人托管，所以就必须在这里close，不然会有句柄泄漏 */
	CloseHandle();

	if ( pSink )
	{
		CMEAutoPtrT<IMETransport> pTransport;
		pSink->OnConnect(
			pTransport,
			m_bConnected ? ME_ERROR_TIMEOUT : ME_ERROR_NETWORK_CONNECT_ERROR );
	}

	RemoveReference();
}

ME_Result CMEConnectorWrapper::PreOnConnect(
    ME_HANDLE hSocket,
    CMETransportAutoPtr& pTransportAutoPtr )
{
    ME_Result hResult = ME_OK;

    OperatorDequeType::reverse_iterator iter = m_dequeOperator.rbegin();
    for (; iter != m_dequeOperator.rend(); ++iter )
    {
        ME_ASSERTE_RETURN( ((*iter) != NULL), ME_ERROR_NULL_POINTER );

        hResult = (*iter)->PreOnConnect(
            hSocket,
            pTransportAutoPtr );

        if ( ME_FAILED(hResult) )
        {
            break;
        }
    }

    return hResult;
}


ME_NAME_SPACE_END



