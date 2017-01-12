#include "TransportWrapper.h"
#include "TransportFactory.h"
#include "TransportTcp.h"
#include "TransportUdp.h"
#include "TransportOperator.h"
#include "ThreadManager.h"
#include "Thread.h"
#include "Reactor.h"
#include "MessageBlock.h"
#include "EventTransportT.h"
#include "TransportUdpManager.h"
#include "TransportOperatorKeepAlive.h"

#include <iostream>
using namespace std;

#include "HikTransportAsyncTcp.h"
#include "HikTransportAsyncUdp.h"

ME_NAME_SPACE_BEGIN

CMETransportWrapper::CMETransportWrapper()
	: m_dwType( IMEConnectionManager::CONNECTION_TYPE_NONE )
	, m_pTransport( NULL )
	, m_pSink( NULL )
	//, m_bClosed( FALSE )
{

}

CMETransportWrapper::~CMETransportWrapper()
{
	m_UdpDataBuf.clear();
	m_dequeOperator.clear();
}

ME_Result CMETransportWrapper::Initialize( 
	DWORD dwType, 
	ME_HANDLE hHandle )
{
	ME_Result hResult = ME_ERROR_FAILURE;

	CMEThread* pThread = (CMEThread*)CMEThreadManagerSingleton::Instance()->GetNetworkThread();
	if ( pThread && ME_BIT_ENABLED(pThread->GetReactor()->GetType(), CMEReactor::REACTOR_TYPE_SYNC) )
	{
		hResult = CreateSync( 
			dwType,
			hHandle );
	}
	else
	{
		hResult = CreateAsync( 
			dwType,
			hHandle );
	}

	if ( ME_SUCCEEDED(hResult) )
	{
		m_dwType = dwType;
	}

	return hResult;
}

ME_Result CMETransportWrapper::RegisterOperator( IMETransportOperator* pOperator )
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

    m_dequeOperator.push_back( IMETransportOperatorAutoPtr(pOperator) );

    return ME_OK;
}

ME_Result CMETransportWrapper::UnRegisterOperator( IMETransportOperator* pOperator )
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

ME_Result CMETransportWrapper::Open( IMETransportSink* pSink )
{
	ME_ASSERTE_RETURN( m_pTransport && pSink, ME_ERROR_NULL_POINTER );
	
	//AddReference();

	/** 
	* 如果<m_pSink>不为空, 说明Open()已经被调用过
	* 再一次的Open()只是, 也只可以更换sink而已
	*/
	if ( m_pSink )
	{
		m_pSink = pSink;

#ifdef ME_DEBUG
		//ME_ASSERTE( TRUE == m_UdpDataBuf.empty() );	// 2013.9.16 edited by 韦珂
#endif	// ME_DEBUG

		//RemoveReference();

		return ME_OK;
	}

    /* 2014.7.7 modify by 方宇 fix 在Sink未完成设置的情况下，Transport回调OnReceive失败的问题*/
    m_pSink = pSink;

	CMEThread* pThread = (CMEThread*)CMEThreadManager::Instance()->GetNetworkThread();
	ME_Result hResult = m_pTransport->Open(
		pThread->GetReactor(),
		this );
	
	if ( ME_FAILED(hResult) )
	{
        m_pSink = NULL;

		m_pTransport->Close( 
			pThread->GetReactor(),
			hResult );
	}
// 	else
// 	{
// 		m_pSink = pSink;
// 	}

	{
		CMELockGuardT<CMELockThreadMutex> Guard( &m_DataBufLock );

		if ( FALSE == m_UdpDataBuf.empty() )
		{
			ME_ASSERTE( ME_BIT_ENABLED(m_dwType, IMEConnectionManager::CONNECTION_TYPE_UDP) );

			DeliveryDataEvent* pEvent = new DeliveryDataEvent( this );

			pEvent->Run( pThread );
		}
	}

	//RemoveReference();

	return hResult;
}

ME_Result CMETransportWrapper::Close( ME_Result hReason /* = ME_OK */ )
{
	/**
	* 这里原本是需要把operator结合进来，由于忙不来，又为了不block进度
	* 先把原生类直接放这里，不管operator，等后边写完，再补
	*/

	//AddReference();

	/* 2013.9.6 added by 韦珂 */
	if ( m_bClosed++ != FALSE )
	{
		//RemoveReference();

		return ME_OK;
	}
	
	// TODO
    //m_dequeOperator.clear();

	m_pSink = NULL;
	ME_Result hResult = ME_OK;

	if ( m_pTransport )
	{
		if ( ME_BIT_ENABLED(m_dwType, IMEConnectionManager::CONNECTION_TYPE_UDP) )
		{
			CMEInetAddress peerAddress;
			CMEInetAddress* pTmp = &peerAddress;
			m_pTransport->GetOption(
				IMETransport::OPTION_TYPE_PEER_ADDRESS,
				(void*&)pTmp );

			CMETransportUdpManagerSingleton::Instance()->UnRegisterTransport(
				peerAddress,
				this );
		}

		CMEThread* pThread = (CMEThread*)CMEThreadManager::Instance()->GetNetworkThread();
		hResult = m_pTransport->Close( 
			pThread->GetReactor(),
			hReason );

		//ME_DELETE( m_pTransport );
		//m_pTransport = NULL;
	}

	//RemoveReference();

	return hResult;
}

ME_Result CMETransportWrapper::SendData( CMEMessageBlock* pmbBlock )
{
	ME_ASSERTE_RETURN( m_pTransport && pmbBlock, ME_ERROR_NULL_POINTER );

	//AddReference();

    /**
	* 这里原本是需要把operator结合进来，由于忙不来，又为了不block进度
	* 先把原生类直接放这里，不管operator，等后边写完，再补
	*/

    /* 发送数据 */
    CMEThread* pThread = (CMEThread*)CMEThreadManager::Instance()->GetNetworkThread();
    ME_ASSERTE( pThread );

	ME_Result hResultOperator = ME_OK;
	ME_Result hResultSend = ME_OK;

	do 
	{
		hResultOperator = OperatorPreSend( pmbBlock );

		hResultSend = m_pTransport->SendData( 
			pThread->GetReactor(),
			pmbBlock );

		OperatorPostSend( pmbBlock );

		if( ME_FAILED(hResultSend) )
		{
			//return hResultSend;
			break;
		}

	} while ( ME_BIT_ENABLED(hResultOperator, IMETransportOperator::RETURN_VALUE_AGAIN) );

	//RemoveReference();

	return hResultSend;
}

ME_Result CMETransportWrapper::SetOption( 
	DWORD dwOptionType, 
	void* pOptionValue )
{
	ME_ASSERTE_RETURN( m_pTransport, ME_ERROR_NOT_INITIALIZED );

	//AddReference();

	ME_Result hResult = ME_ERROR_FAILURE;

// 	if ( IMETransport::OPTION_TYPE_SET_KEEPALIVE_INTERVEL == dwOptionType || 
// 		 IMETransport::OPTION_TYPE_SET_KEEPALIVE_CHECKNUM == dwOptionType )
// 	{
// 		/* 设置心跳处理器的选项 */
// 		MapType::iterator iter = m_mapOperator.find( IMEConnectionManager::CONNECTION_TYPE_KEEP_ALIVE );
// 		//ME_ASSERTE_RETURN( iter != m_mapOperator.end() && iter->second, ME_ERROR_NOT_FOUND ); // 2013.9.9 edited by 韦珂
// 		ME_ASSERTE( iter != m_mapOperator.end() && iter->second );
// 
// 		hResult = iter->second->SetOption( 
// 					dwOptionType, 
// 					pOptionValue );
// 	}
// 	else
// 	{		
// 		hResult = m_pTransport->SetOption(
// 					dwOptionType,
// 					pOptionValue );
// 	}

    switch ( dwOptionType )
    {
    case OPTION_TYPE_SET_KEEPALIVE_INTERVEL:
    case OPTION_TYPE_SET_KEEPALIVE_CHECKNUM:
        {
            OperatorDequeType::iterator iter = m_dequeOperator.begin();
            for ( ; iter != m_dequeOperator.end(); ++iter )
            {
                if ( (*iter)->GetType() == IMEConnectionManager::CONNECTION_TYPE_KEEP_ALIVE )
                {
                    hResult = (*iter)->SetOption(
                        dwOptionType,
                        pOptionValue );

                    break;
                }
            }

    	    break;
        }

    case OPTION_TYPE_SET_SSL:
        {
            OperatorDequeType::iterator iter = m_dequeOperator.begin();
            for ( ; iter != m_dequeOperator.end(); ++iter )
            {
                if ( (*iter)->GetType() == IMEConnectionManager::CONNECTION_TYPE_SSL )
                {
                    hResult = (*iter)->SetOption(
                        dwOptionType,
                        pOptionValue );

                    break;
                }
            }

            break;
        }

    default:
        {
            hResult = m_pTransport->SetOption(
                dwOptionType,
                pOptionValue );
        }
    }

	//RemoveReference();

	return hResult;
}

ME_Result CMETransportWrapper::GetOption( 
	DWORD dwOptionType, 
	void*& pOptionValue )
{
	//AddReference();

	ME_Result hResult = ME_OK;

	if ( IMETransport::OPTION_TYPE_TRANSPORT_TYPE == dwOptionType )
	{
		DWORD* iTransportType = static_cast<DWORD*>( pOptionValue );
		ME_ASSERTE( (NULL != iTransportType) );

		*iTransportType = m_dwType;
	}
	else
	{
		//ME_ASSERTE_RETURN( m_pTransport, ME_ERROR_NOT_INITIALIZED ); // 2013.9.9 edited by 韦珂
		if ( NULL == m_pTransport )
		{
			//RemoveReference();

			return ME_ERROR_NOT_INITIALIZED;
		}

		hResult = m_pTransport->GetOption(
			dwOptionType,
			pOptionValue );
	}

	//RemoveReference();

	return hResult;
}

IMETransportSink* CMETransportWrapper::GetSink() const
{
	return m_pSink;
}

DWORD CMETransportWrapper::AddReference()
{
	return CMEReferenceControlT<MutexType>::AddReference();
}

DWORD CMETransportWrapper::RemoveReference()
{
	return CMEReferenceControlT<MutexType>::RemoveReference();
}

void CMETransportWrapper::OnReceive_c( 
	const char* pszData, 
	UINT uLength )
{
	//AddReference();

	CMEMessageBlock mbBlock(
		uLength,
		(void*)pszData,
		uLength );

	{
		CMELockGuardT<CMELockThreadMutex> Guard( &m_DataBufLock );
		if ( m_pSink && 
			(TRUE == m_UdpDataBuf.empty()) )
		{
			m_pSink->OnReceive(
				this,
				&mbBlock );

			//RemoveReference();

			return;
		}

	

		m_UdpDataBuf.push_back( mbBlock.Clone() );
	}

	//RemoveReference();
}

void CMETransportWrapper::OnReceive( 
	IMETransportImpl* pTransport, 
	CMEMessageBlock* pmbData )
{
	//AddReference();

	{
		CMELockGuardT<CMELockThreadMutex> Guard( &m_DataBufLock );

		if ( FALSE == m_UdpDataBuf.empty() )
		{
			ME_ASSERTE( ME_BIT_ENABLED(m_dwType, IMEConnectionManager::CONNECTION_TYPE_UDP) );
			ME_WARNING_TRACE_THIS( "recv data from network and yet the 'OnConnect' notification is still on road " );

			/*{
				CMELockGuardT<CMELockThreadMutex> Guard( &m_DataBufLock );

				m_UdpDataBuf.push_back( pmbData->Clone() );
			}*/
			m_UdpDataBuf.push_back( pmbData->Clone() );

			//RemoveReference();

			return;
		}
	}
	/**
	* 这里原本是需要把operator结合进来，由于忙不来，又为了不block进度
	* 先把原生类直接放这里，不管operator，等后边写完，再补
	*/

	// TODO

    ME_Result hResult = ME_OK;

	do 
	{
		/** 
		* 如果<m_bClosed>为FALSE，说明在上边的OnReceive() callback中，
		* 上层没有调用过Close()
		*/
		if ( m_bClosed == FALSE )
		{
			hResult = OperatorPreRecv( pmbData );

			if ( ME_BIT_DISABLED(hResult, IMETransportOperator::RETURN_VALUE_FINISH) && (m_pSink) )
			{
				m_pSink->OnReceive(
					this,
					pmbData );
			}

			OperatorPostRecv( pmbData );
		}
		else
		{
			break;
		}
	} while ( ME_BIT_ENABLED(hResult, IMETransportOperator::RETURN_VALUE_AGAIN) );

	//RemoveReference();
}

void CMETransportWrapper::OnDisconnect( 
	IMETransportImpl* pTransport, 
	ME_Result hReason )
{
	/**
	* 这里原本是需要把operator结合进来，由于忙不来，又为了不block进度
	* 先把原生类直接放这里，不管operator，等后边写完，再补
	*/
	// TODO

	//AddReference();

	if ( m_bClosed != FALSE )
	{
		//RemoveReference();

		return;
	}

	m_pSink->OnDisconnect(
		this,
		hReason );

	//RemoveReference();
}

void CMETransportWrapper::OnCanSendAgain( 
	IMETransportImpl* pTransport, 
	ME_Result hResult /* = ME_OK */ )
{
	/**
	* 这里原本是需要把operator结合进来，由于忙不来，又为了不block进度
	* 先把原生类直接放这里，不管operator，等后边写完，再补
	*/

	//AddReference();

	if ( m_bClosed != FALSE )
	{
		//RemoveReference();

		return;
	}

	// TODO
	m_pSink->OnCanSendAgain(
		this,
		hResult );

	//RemoveReference();
}

void CMETransportWrapper::DeliveryData()
{
	ME_ASSERTE( ME_BIT_ENABLED(m_dwType, IMEConnectionManager::CONNECTION_TYPE_UDP) );

	//AddReference();

	{
		CMELockGuardT<CMELockThreadMutex> Guard( &m_DataBufLock );

		if ( FALSE == m_UdpDataBuf.empty() )
		{
			DataBufferType::iterator iter = m_UdpDataBuf.begin();
			for ( ; iter != m_UdpDataBuf.end(); ++iter )
			{
				if ( m_bClosed != FALSE )
				{
					//RemoveReference();

					return;
				}

				if ( NULL == m_pSink )
				{
					break;
				}

				m_pSink->OnReceive(
					this,
					(*iter).ParaIn() );
			}

			m_UdpDataBuf.clear();
		}
	}

	//RemoveReference();
}

ME_Result CMETransportWrapper::OperatorPreSend( 
	CMEMessageBlock*& pmbBlock )
{
	ME_Result hResult = IMETransportOperator::RETURN_VALUE_OK;

// 	MapType::reverse_iterator iterOperator = m_mapOperator.rbegin();
// 	for( ; iterOperator != m_mapOperator.rend(); ++iterOperator )
// 	{
// #ifdef ME_DEBUG
// 		ME_ASSERTE_RETURN( (iterOperator->second), ME_ERROR_NULL_POINTER );
// #endif
// 		hResult |= iterOperator->second->PreSendData( pmbBlock );
// 		if ( ME_BIT_ENABLED(hResult, IMETransportOperator::RETURN_VALUE_FINISH) )
// 		{
// 			break;
// 		}
// 	}

    OperatorDequeType::reverse_iterator iter = m_dequeOperator.rbegin();
    for (; iter != m_dequeOperator.rend(); ++iter )
    {
#ifdef ME_DEBUG
        ME_ASSERTE_RETURN( ((*iter) != NULL), ME_ERROR_NULL_POINTER );
#endif
        hResult |= (*iter)->PreSendData( pmbBlock );
        if ( ME_BIT_ENABLED(hResult, IMETransportOperator::RETURN_VALUE_FINISH) )
        {
            break;
        }
    }

	return hResult;
}

ME_Result CMETransportWrapper::OperatorPostSend( 
	CMEMessageBlock*& pmbBlock )
{
	ME_Result hResult = IMETransportOperator::RETURN_VALUE_OK;

// 	MapType::reverse_iterator iterOperator = m_mapOperator.rbegin();
// 	for( ; iterOperator != m_mapOperator.rend(); ++iterOperator )
// 	{
// #ifdef ME_DEBUG
// 		ME_ASSERTE_RETURN( (iterOperator->second), ME_ERROR_NULL_POINTER );
// #endif
// 		hResult |= iterOperator->second->PostSendData( pmbBlock );
// 		if ( ME_BIT_ENABLED(hResult, IMETransportOperator::RETURN_VALUE_FINISH) )
// 		{
// 			break;
// 		}
// 	}

    OperatorDequeType::reverse_iterator iter = m_dequeOperator.rbegin();
    for ( ; iter != m_dequeOperator.rend(); ++iter )
    {
#ifdef ME_DEBUG
        ME_ASSERTE_RETURN( ((*iter) != NULL), ME_ERROR_NULL_POINTER );
#endif
        hResult |= (*iter)->PostSendData( pmbBlock );
        if ( ME_BIT_ENABLED(hResult, IMETransportOperator::RETURN_VALUE_FINISH) )
        {
            break;
        }
    }

	return hResult;
}

ME_Result CMETransportWrapper::OperatorPreRecv( 
	CMEMessageBlock*& pmbBlock )
{
	ME_Result hResult = IMETransportOperator::RETURN_VALUE_OK;

// 	MapType::iterator iterOperator = m_mapOperator.begin();
// 	for( ; iterOperator != m_mapOperator.end(); ++iterOperator )
// 	{
// #ifdef ME_DEBUG
// 		ME_ASSERTE_RETURN( (iterOperator->second), ME_ERROR_NULL_POINTER );
// #endif
// 		hResult |= iterOperator->second->PreOnReceive( pmbBlock );
// 		if ( ME_BIT_ENABLED(hResult, IMETransportOperator::RETURN_VALUE_FINISH) )
// 		{
// 			break;
// 		}
// 	}

    OperatorDequeType::iterator iter = m_dequeOperator.begin();
    for ( ; iter != m_dequeOperator.end(); ++iter )
    {
#ifdef ME_DEBUG
        ME_ASSERTE_RETURN( ((*iter) != NULL), ME_ERROR_NULL_POINTER );
#endif
        hResult |= (*iter)->PreOnReceive( pmbBlock );
        if ( ME_BIT_ENABLED(hResult, IMETransportOperator::RETURN_VALUE_FINISH) )
        {
            break;
        }
    }

	return hResult;
}

ME_Result CMETransportWrapper::OperatorPostRecv( 
	CMEMessageBlock*& pmbBlock )
{
	ME_Result hResult = IMETransportOperator::RETURN_VALUE_OK;

// 	MapType::iterator iterOperator = m_mapOperator.begin();
// 	for( ; iterOperator != m_mapOperator.end(); ++iterOperator )
// 	{
// #ifdef ME_DEBUG
// 		ME_ASSERTE_RETURN( (iterOperator->second), ME_ERROR_NULL_POINTER );
// #endif
// 		hResult |= iterOperator->second->PostOnReceive( pmbBlock );
// 		if ( ME_BIT_ENABLED(hResult, IMETransportOperator::RETURN_VALUE_FINISH) )
// 		{
// 			break;
// 		}
// 	}

    OperatorDequeType::iterator iter = m_dequeOperator.begin();
    for ( ; iter != m_dequeOperator.end(); ++iter )
    {
#ifdef ME_DEBUG
        ME_ASSERTE_RETURN( ((*iter) != NULL), ME_ERROR_NULL_POINTER );
#endif
        hResult |= (*iter)->PostOnReceive( pmbBlock );
        if ( ME_BIT_ENABLED(hResult, IMETransportOperator::RETURN_VALUE_FINISH) )
        {
            break;
        }
    }

	return hResult;
}

ME_Result CMETransportWrapper::CreateSync( 
	DWORD dwType,
	ME_HANDLE hHandle )
{
	do 
	{
		/* TCP */
		if ( ME_BIT_ENABLED(dwType, IMEConnectionManager::CONNECTION_TYPE_TCP) )
		{
			m_pTransport = new CMETransportTcp( hHandle );

			break;
		}

		/* UDP */
		else if ( ME_BIT_ENABLED(dwType, IMEConnectionManager::CONNECTION_TYPE_UDP) )
		{
			m_pTransport = new CMETransportUdp( hHandle );

			break;
		}

		/* 只可能有这两种，其他的，通通asserte */
		ME_ASSERTE( FALSE );

		return ME_ERROR_INVALID_ARG;

	} while ( 0 );

	return ME_OK;
}

ME_Result CMETransportWrapper::CreateAsync(
	DWORD dwType,
	ME_HANDLE hHandle )
{
	do 
	{
		/* TCP */
		if ( ME_BIT_ENABLED(dwType, IMEConnectionManager::CONNECTION_TYPE_TCP) )
		{
			m_pTransport = new CMETransportAsyncTcp( hHandle );

			break;
		}

		/* UDP */
		else if ( ME_BIT_ENABLED(dwType, IMEConnectionManager::CONNECTION_TYPE_UDP) )
		{
			m_pTransport = new CMETransportAsyncUdp( hHandle );

			break;
		}

		/* 只可能有这两种，其他的，通通asserte */
		ME_ASSERTE( FALSE );

		return ME_ERROR_INVALID_ARG;

	} while ( 0 );

	return ME_OK;
}

ME_NAME_SPACE_END
