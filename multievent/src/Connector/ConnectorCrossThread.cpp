#include "ConnectorCrossThread.h"
#include "CustomizeEventRun.h"
#include "EventAcceptorConnectorT.h"
#include "ThreadManager.h"
#include "ThreadInterface.h"
#include "TransportFactory.h"

ME_NAME_SPACE_BEGIN

CMEConnectorCrossThread::CMEConnectorCrossThread( DWORD dwType )
	: m_pSink( NULL )
	, m_dwType( dwType )
{

}

CMEConnectorCrossThread::~CMEConnectorCrossThread()
{

}

ME_Result CMEConnectorCrossThread::Open( 
	IMEConnectorSink* pSink, 
	const CMEInetAddress& peerAddress, 
	CMETimeValue* pTimeout /* = NULL */, 
	CMEInetAddress* pLocalAddress /* = NULL */ )
{
	ME_ASSERTE_RETURN( pSink, ME_ERROR_NULL_POINTER );
	m_pSink = pSink;
	m_hiaPeerAddr = peerAddress;
	
	if ( pTimeout )
	{
		m_htvTimeout = *pTimeout;
	}

	if ( pLocalAddress )
	{
		m_hiaLocalAddr = *pLocalAddress;
	}

	OpenNetworkEvent* pEvent = new OpenNetworkEvent(
		this,
		pTimeout ? &m_htvTimeout : NULL,
		&m_hiaPeerAddr,
		pLocalAddress ? &m_hiaLocalAddr : NULL );

	return pEvent->Run( IMEThreadManager::Instance()->GetNetworkThread() );
}

ME_Result CMEConnectorCrossThread::Open_n( 
	const CMEInetAddress& peerAddress, 
	CMETimeValue* pTimeout /* = NULL */, 
	CMEInetAddress* pLocalAddress /* = NULL */ )
{
	ME_Result hResult = ME_OK;
	if ( !m_pConnectorAutoPtr )
	{
		hResult = IMEConnectionManager::Instance()->CreateClient(
			m_dwType,
			m_pConnectorAutoPtr );
	}

	if ( ME_FAILED(hResult) )
	{
		CMETransportAutoPtr pNullTransport;
		OnConnectUserEvent* pEvent = new OnConnectUserEvent(
			this,
			pNullTransport,
			hResult );

		/* 创建connector失败，也要通知OnConnect，不然上层无法得知最终结果 */
		return pEvent->Run( IMEThreadManager::Instance()->GetNetworkThread() );
	}

	/* 如果创建成功，那么最终结果就依赖于<m_pConnectorAutoPtr>的操作 */
	return m_pConnectorAutoPtr->Open(
		this,
		peerAddress,
		pTimeout,
		pLocalAddress );
}

void CMEConnectorCrossThread::Close()
{
	m_pSink = NULL;

	CloseNetworkEvent* pEvent = new CloseNetworkEvent( this );
	pEvent->Run( IMEThreadManager::Instance()->GetNetworkThread() );
}

void CMEConnectorCrossThread::Close_n()
{
	if ( m_pConnectorAutoPtr )
	{
		m_pConnectorAutoPtr->Close();
	}

	// m_pConnectorAutoPtr = NULL; ????
}

ME_Result CMEConnectorCrossThread::SetOption(
    DWORD dwOptionType,
    void* pOptionValue )
{
    ME_ASSERTE_RETURN( (m_pConnectorAutoPtr != NULL), ME_ERROR_NULL_POINTER );

    return m_pConnectorAutoPtr->SetOption(
        dwOptionType,
        pOptionValue );
}

ME_Result CMEConnectorCrossThread::GetOption(
    WORD dwOptionType,
    void*& pOptionValue )
{
    ME_ASSERTE_RETURN( (m_pConnectorAutoPtr != NULL), ME_ERROR_NULL_POINTER );

    return m_pConnectorAutoPtr->GetOption(
        dwOptionType,
        pOptionValue );
}

DWORD CMEConnectorCrossThread::AddReference()
{
	return CMEReferenceControlT<MutexType>::AddReference();
}

DWORD CMEConnectorCrossThread::RemoveReference()
{
	return CMEReferenceControlT<MutexType>::RemoveReference();
}

void CMEConnectorCrossThread::OnReferenceRelease()
{
	// TODO 这就不能马上删除了，要来回post event以确保这个实例所涉及的线程的所有属于它的事件
	// 全部处理完了才能delete

	Destroy_u();
}

void CMEConnectorCrossThread::Destroy_u()
{
	m_pSink = NULL;

	CMEMTSafeDestroy::Destroy_u();
}

void CMEConnectorCrossThread::Destroy_n()
{
	Close_n();

	CMEMTSafeDestroy::Destroy_n();
}

void CMEConnectorCrossThread::OnConnect( 
	CMETransportAutoPtr& pTransportAutoPtr, 
	ME_Result hResult )
{
	OnConnectUserEvent* pEvent = NULL;
	pEvent = new OnConnectUserEvent(
		this,
		pTransportAutoPtr,
		hResult );

	ME_ASSERTE_RETURN_VOID( pEvent );

	pEvent->Run( IMEThreadManager::Instance()->GetUserThread() );
}

void CMEConnectorCrossThread::OnConnect_u( 
	TransportPtrType pTransport, 
	ME_Result hResult )
{
	ME_ASSERTE_RETURN_VOID( m_pSink );

	if ( ME_SUCCEEDED(hResult) )
	{
		CMETransportAutoPtr pTransportAutoPtr;
		hResult = CMETransportFactorySingleton::Instance()->Create(
			pTransport.ParaIn(),
			pTransportAutoPtr.ParaOut() );

		ME_ASSERTE( ME_SUCCEEDED(hResult) );
		
		m_pSink->OnConnect(
			pTransportAutoPtr,
			hResult );

		return;
	}
	
	/* 连接失败 */
	m_pSink->OnConnect(
		pTransport,
		hResult );
}

ME_NAME_SPACE_END









