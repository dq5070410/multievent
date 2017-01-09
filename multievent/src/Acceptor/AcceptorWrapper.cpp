#include "AcceptorWrapper.h"
#include "InetAddress.h"
#include "AcceptorFactory.h"
#include "TransportFactory.h"
#include "OSSocket.h"
#include "ThreadManager.h"
#include "Thread.h"
#include "TransportUdpManager.h"
#include "EventAcceptorConnectorT.h"
#include "Reactor.h"
#include "AsyncIoRecvFrom.h"
#include "MessageBlock.h"

ME_NAME_SPACE_BEGIN

typedef CMETransportUdpManagerSingleton UdpManager;

CMEAcceptorWrapper::CMEAcceptorWrapper()
	: m_dwType( IMEConnectionManager::CONNECTION_TYPE_NONE )
	, m_pAcceptor( NULL )
	, m_iReactorType( CMEReactor::REACTOR_TYPE_NULL )
	, m_pSink( NULL )
{
	
}

CMEAcceptorWrapper::~CMEAcceptorWrapper()
{
	/**
	* �ϲ���Ҫ������֮ǰ��ʾ�ĵ���Close()��
	* ����Ͳ����ظ�������
	*/
	//Close( ME_OK );
}

ME_Result CMEAcceptorWrapper::Initialize( DWORD dwType )
{
	AddReference();

	IMEAcceptorImpl* pAcceptor = NULL;
	ME_Result hResult = CMEAcceptorFactorySingleton::Instance()->Create(
		dwType,
		pAcceptor );

	if ( ME_FAILED(hResult) )
	{
		ME_DELETE( pAcceptor );

		
		RemoveReference();

		return hResult;
	}

	m_dwType = dwType;
	m_pAcceptor = pAcceptor;

	RemoveReference();

	return ME_OK;
}

ME_Result CMEAcceptorWrapper::Open( 
	IMEAcceptorSink* pSink, 
	const CMEInetAddress& localAddress )
{
	// ����Ӧ��Ҫ�õ���ǰ�̵߳�Reactor��ֻ������������m_pAcceptor->Open

	ME_ASSERTE_RETURN( m_pAcceptor && pSink, ME_ERROR_NULL_POINTER );
	ME_ASSERTE_RETURN( !m_pSink, ME_ERROR_ALREADY_INITIALIZED );

	AddReference();

	CMEThread* pThread = (CMEThread*)CMEThreadManager::Instance()->GetNetworkThread();
	ME_Result hResult = m_pAcceptor->Open(
		this,
		pThread->GetReactor(),
		localAddress );

	if ( ME_FAILED(hResult) )
	{
		m_pAcceptor->Close( pThread->GetReactor() );
	}
	else
	{
		m_iReactorType = pThread->GetReactor()->GetType();
		m_pSink = pSink;
	}

	RemoveReference();

	return hResult;
}

void CMEAcceptorWrapper::Close( ME_Result scReason /* = ME_OK */ )
{
	AddReference();

	CMEThread* pThread = (CMEThread*)CMEThreadManager::Instance()->GetNetworkThread();
	if ( m_pAcceptor )
	{
		m_pAcceptor->Close( pThread->GetReactor() );

		//ME_DELETE( m_pAcceptor );
		m_pAcceptor = NULL;
	}

	m_pSink = NULL;

	RemoveReference();
}

ME_Result CMEAcceptorWrapper::RegisterOperator( IMEConnectionOperator* pOperator )
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

ME_Result CMEAcceptorWrapper::UnregisterOperator( IMEConnectionOperator* pOperator )
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

ME_Result CMEAcceptorWrapper::SetOption(
    DWORD dwOptionType,
    void* pOptionValue )
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

ME_Result CMEAcceptorWrapper::GetOption(
    WORD dwOptionType,
    void*& pOptionValue )
{
    return ME_ERROR_FAILURE;
}

DWORD CMEAcceptorWrapper::AddReference()
{
	return CMEReferenceControlT<MutexType>::AddReference();
}

DWORD CMEAcceptorWrapper::RemoveReference()
{
	return CMEReferenceControlT<MutexType>::RemoveReference();
}

void CMEAcceptorWrapper::OnConnect( 
	ME_Result hResult, 
	ME_HANDLE hHandle, 
	CMEInetAddress* pPeerAddress )
{
	ME_ASSERTE_RETURN_VOID( m_pSink );
	ME_ASSERTE_RETURN_VOID( hHandle && (ME_INVALID_HANDLE != hHandle) );

	AddReference();

	/**
	* ���������������Ŀ��ܣ��������������ص�������hResult�ض���OK��
	* ������Ϊ�˷�ֹ���⣬���Ǽ���������һ��
	*/
	if ( ME_FAILED(hResult) )
	{
		OS::SocketClose( hHandle );

		ME_ERROR_TRACE_THIS( "Unexpected error = " << errno );

		RemoveReference();

		return;
	}

	CMETransportAutoPtr pTransportAutoPtr;
	hResult = CMETransportFactorySingleton::Instance()->Create(
		m_dwType,
		hHandle,
		pTransportAutoPtr.ParaOut() );

	ME_ASSERTE( ME_SUCCEEDED(hResult) && pTransportAutoPtr );

    hResult = PreOnConnect(
        hHandle,
        pTransportAutoPtr );

    if ( ME_FAILED(hResult) )
    {
        pTransportAutoPtr = NULL;

        RemoveReference();

        return;
    }

	m_pSink->OnConnect( pTransportAutoPtr );

	/**
	* �����tcpview֮��Ĺ��ߣ����ر�һ�����ڼ����Ķ˿ڣ��ᷢ��ʲô��
	* �����selectģ�ͣ���listen�ɹ�֮��ʵ���ϣ�ֻ��FD_READ��FD_WRITE
	* ���͵��¼��������tcpviewǿ��kill�����ڼ����Ķ˿ڣ�����FD_READ����
	* ô�������������ô������<hHandle>�ǵ���listen�Ǹ����ô��
	*
	* �е����ˣ���ͷ��Ҫ����һ��
	*/

	RemoveReference();
}

void CMEAcceptorWrapper::OnInput( 
	ME_Result hResult, 
	ME_HANDLE hHandle )
{
	AddReference();

	char szBuf[ME_CONNECTION_READ_MAX_SIZE];
	::memset(
		szBuf,
		0,
		ME_CONNECTION_READ_MAX_SIZE );

	
	CMEInetAddress peerAddress;
	int iRecvLen = 0;
	char* pszBuf = szBuf;

	/* ��ȡ���ݲ���ȡ�Զ˵�ַ */
	ReadData(
		hHandle, 
		iRecvLen, 
		pszBuf, 
		peerAddress );

	/* ���������ܵ� */
	if ( 0 > iRecvLen )
	{
		ME_WARNING_TRACE_THIS( "length of received data = " << iRecvLen );

		RemoveReference();

		return;
	}

	IMETransport* pTransport = UdpManager::Instance()->FindTransport( peerAddress );

	/* ֪ͨTransportUdpManager������������OnConnect()�Լ�OnReceive()��callback */
	NotifyUdpManager( 
		pTransport,
		iRecvLen,
		pszBuf,
		peerAddress );

	RemoveReference();
}

void CMEAcceptorWrapper::ReadData( 
	ME_HANDLE hHandle, 
	int& iRecvLen, 
	char*& pszData, 
	CMEInetAddress& hiaPeerAddr )
{
	if ( CMEReactor::REACTOR_TYPE_ASYCH_IOCP != m_iReactorType )
	{
		m_SocketUdp.SetHandle( hHandle );

		iRecvLen = m_SocketUdp.RecvFrom(
			pszData,
			ME_CONNECTION_READ_MAX_SIZE,
			hiaPeerAddr );
	}

#ifdef ME_WIN	
	else
	{
		CMEAsyncIoRecvFrom::Result* pIoResult = (CMEAsyncIoRecvFrom::Result*)hHandle;
		iRecvLen = pIoResult->m_pmbBlock->GetLength();
		hiaPeerAddr = *pIoResult->m_pFromAddress;

		/**
		*
		* ��ʵ�������ȷ�Ұ�ȫ��д��Ӧ������m_pmbBlock->Read()���������ݶ�ȡ��szBuf��
		* ��������Ϊ�ͺ��ϱ�һ���ˣ����ǣ���AsyncIO������£�m_pmbBlock��Ԥ�ȷ�����
		* ֻ����һ��DB������Ϊ��Ч�ʣ�����ʱ��ôд
		*/
		iovec ioData[1];
		pIoResult->m_pmbBlock->FillIOVec( ioData, 1 );
		pszData = ioData[0].iov_base;

		ME_ASSERTE( iRecvLen == ioData[0].iov_len );
	}
#endif	// ME_WIN
}

void CMEAcceptorWrapper::NotifyUdpManager( 
	IMETransport* pTransport, 
	int iRecvLen, 
	char* pszData, 
	const CMEInetAddress& hiaPeerAddr )
{
	/* ����ܹ���Զ˵�ַ�󶨵�transport�����ܹ��ҵ� */
	if ( pTransport )
	{
		/* ˵��֮ǰ���յ�������<peerAddress>�İ��ˣ�ֱ��ת<UdpManager>������� */
		UdpManager::Instance()->OnUserDataReceive(
			pTransport,
			pszData,
			iRecvLen );
	}
	else
	{
		/* û�ҵ�����֪ͨOnUserConnect���Ա��ȡһ��transport */
		UdpManager::Instance()->OnUserConnect(
			m_dwType,
			hiaPeerAddr,
			pTransport );

		/* ��Ҫ������������ȥcall OnConnect() */
		OnAcceptCurrentEvent* pEvent = new OnAcceptCurrentEvent(
			this,
			pTransport );

		pEvent->Run( CMEThreadManager::Instance()->GetCurrentThread() );

		/* �������ٴ������ݣ������Ϳ������ͺ�TCP������һ���� -- ��OnConnect����OnReceive */
		UdpManager::Instance()->OnUserDataReceive(
			pTransport,
			pszData,
			iRecvLen );
	}
}

void CMEAcceptorWrapper::OnConnect_c( TransportType* pTransport )
{
	ME_ASSERTE_RETURN_VOID( pTransport );

	AddReference();

	if ( m_pSink )
	{
		CMETransportAutoPtr pTmpTransport;
		pTmpTransport.ParaOut() = pTransport;

		m_pSink->OnConnect( pTmpTransport );
	}

	RemoveReference();
}

ME_Result CMEAcceptorWrapper::PreOnConnect(
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











