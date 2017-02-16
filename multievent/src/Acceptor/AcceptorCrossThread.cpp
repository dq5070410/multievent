#include "AcceptorCrossThread.h"
#include "CustomizeEventRun.h"
#include "EventAcceptorConnectorT.h"
#include "ThreadManager.h"
#include "Thread.h"
#include "TransportFactory.h"

ME_NAME_SPACE_BEGIN

CMEAcceptorCrossThread::CMEAcceptorCrossThread( DWORD dwType )
	: m_pSink( NULL )
	, m_dwType( dwType )
{

}

CMEAcceptorCrossThread::~CMEAcceptorCrossThread()
{

}

ME_Result CMEAcceptorCrossThread::Open( 
	IAcceptorSink* pSink, 
	const CMEInetAddress& hiaLocalAddr )
{
	ME_ASSERTE_RETURN( pSink, ME_ERROR_NULL_POINTER );
	m_pSink = pSink;
	m_hiaLocalAddr = hiaLocalAddr;

	OpenNetworkEvent* pEvent = new OpenNetworkEvent(
		this,
		&m_hiaLocalAddr );

	return pEvent->Run( 
		IMEThreadManager::Instance()->GetNetworkThread(),
		TRUE );
}

ME_Result CMEAcceptorCrossThread::Open_n( CMEInetAddress* pLocalAddress )
{
	ME_Result hResult = IConnectionManager::Instance()->CreateServer(
		m_dwType,
		m_pAcceptorAutoPtr );

	if ( ME_SUCCEEDED(hResult) )
	{
		hResult = m_pAcceptorAutoPtr->Open(
			this,
			*pLocalAddress );
	}
	else
	{
		m_pAcceptorAutoPtr = NULL;
	}

	return hResult;
}

void CMEAcceptorCrossThread::Close( ME_Result hReason /* = ME_OK */ )
{
	m_pSink = NULL;

	CloseNetworkEvent* pEvent = new CloseNetworkEvent( this );
	pEvent->Run( IMEThreadManager::Instance()->GetNetworkThread() );
}

ME_Result CMEAcceptorCrossThread::SetOption(
    DWORD dwOptionType,
    void* pOptionValue )
{
    ME_ASSERTE_RETURN( (m_pAcceptorAutoPtr != NULL), ME_ERROR_NULL_POINTER );

    return m_pAcceptorAutoPtr->SetOption(
        dwOptionType,
        pOptionValue );
}

ME_Result CMEAcceptorCrossThread::GetOption(
    WORD dwOptionType,
    void*& pOptionValue )
{
    ME_ASSERTE_RETURN( (m_pAcceptorAutoPtr != NULL), ME_ERROR_NULL_POINTER );

    return m_pAcceptorAutoPtr->GetOption(
        dwOptionType,
        pOptionValue );
}

void CMEAcceptorCrossThread::Close_n()
{
	ME_ASSERTE_RETURN_VOID( m_pAcceptorAutoPtr );

	m_pAcceptorAutoPtr->Close( ME_OK );
}

DWORD CMEAcceptorCrossThread::AddReference()
{
	return CMEReferenceControlT<MutexType>::AddReference();
}

DWORD CMEAcceptorCrossThread::RemoveReference()
{
	return CMEReferenceControlT<MutexType>::RemoveReference();
}

void CMEAcceptorCrossThread::OnReferenceRelease()
{
	// TODO 这就不能马上删除了，要来回post event以确保这个实例所涉及的线程的所有属于它的事件
	// 全部处理完了才能delete

	Destroy_u();
}

void CMEAcceptorCrossThread::Destroy_u()
{
	m_pSink = NULL;

	CMEMTSafeDestroy::Destroy_u();
}

void CMEAcceptorCrossThread::Destroy_n()
{
	Close_n();

	CMEMTSafeDestroy::Destroy_n();
}

void CMEAcceptorCrossThread::OnConnect( CTransportAutoPtr& pTransportAutoPtr )
{
	OnConnectUserEvent* pEvent = new OnConnectUserEvent(
		this,
		pTransportAutoPtr,
		ME_OK );

	pEvent->Run( IMEThreadManager::Instance()->GetUserThread() );
}

void CMEAcceptorCrossThread::OnConnect_u( 
	CTransportAutoPtr pTransport, 
	ME_Result hResult )
{
	ME_ASSERTE_RETURN_VOID( m_pSink );

	CTransportAutoPtr pTransportCrossThread;
	CMETransportFactorySingleton::Instance()->Create(
		pTransport.ParaIn(),
		pTransportCrossThread.ParaOut() ); 

	m_pSink->OnConnect( pTransportCrossThread );
}

ME_NAME_SPACE_END






