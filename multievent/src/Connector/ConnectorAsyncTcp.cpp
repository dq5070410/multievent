#include "ConnectorAsyncTcp.h"
#include "Reactor.h"

#include "ReactorAsyncEpoll.h"

ME_NAME_SPACE_BEGIN

CMEConnectorAsyncTcp::CMEConnectorAsyncTcp()
#ifdef ME_LINUX
	: CMEAsyncIoResult(
		CMEReactor::EVENT_TYPE_CONNECT,
		NULL )

	, m_pRefSink( NULL )
#else	// ME_LINUX

	: m_pRefSink( NULL )
#endif	// ME_LINUX
{

}

CMEConnectorAsyncTcp::~CMEConnectorAsyncTcp()
{

}

DWORD CMEConnectorAsyncTcp::AddReference()
{
	if ( m_pRefSink )
	{
		m_pRefSink->AddReference();
	}

	return CMEReferenceControlT<MutexType>::AddReference();
}

DWORD CMEConnectorAsyncTcp::RemoveReference()
{
	if ( m_pRefSink )
	{
		m_pRefSink->RemoveReference();
	}

	return CMEReferenceControlT<MutexType>::RemoveReference();
} 

#ifdef ME_WIN

ME_Result CMEConnectorAsyncTcp::Open( 
	IMEConnectorImplSink* pSink, 
	const CMEInetAddress& peerAddress, 
	CMEReactor* pReactor, 
	ME_HANDLE& hHandle, 
	CMEInetAddress* pLocalAddress /* = NULL */ )
{
	ME_ASSERTE_RETURN( pSink && pReactor, ME_ERROR_NULL_POINTER );

	//Close();	// ȷ�������ظ���

	ME_Result hResult = ME_ERROR_FAILURE;
	BOOL bAutoBind = FALSE;
	if ( pLocalAddress )
	{
		hResult = m_SocketTcp.Open(
			*pLocalAddress,
			TRUE );
	}
	else
	{
		bAutoBind = TRUE;

		hResult = m_SocketTcp.Open( TRUE );
	}

	/* ����Open�ǳɹ�����ʧ�ܣ��������µľ������ */
	hHandle = m_SocketTcp.GetHandle();

	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	/**
	* �����ص�����
	* 
	* �ڶ��̻߳����£�һ��<m_asyncConnect>һ��Post��������һ��
	* �߳����������п��ܵ��£�<m_pSink>��û�б���ֵ���Ǳ��Ѿ�
	* �ڴ������ӳɹ�����Ϣ����������OnConnect()û�лص�
	*/
	m_pSink = pSink;

	/* ����һ��<pSink>��Ϊ��ʹ<pSink>������������TransportAsyncTcp��ͬ */
	m_pRefSink = pSink;

	/* ���ﲻ���������ü�������Ϊ�ᵼ��sink�޷�ɾ���������ڴ�й¶ */
	//m_pRefSink->AddReference();

	int iTmpIndex = -1;
	hResult = pReactor->Register(
		this,
		(CMEConnectorTcp*)this,
		CMEReactor::EVENT_TYPE_CONNECT | CMEReactor::EVENT_TYPE_CLOSE,
		iTmpIndex );

	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	hResult = m_asyncConnect.Post(
		this,
		GetHandle(),
		&peerAddress,
		/**
		* ��Ϊ׼ȷ��д��Ӧ���Ǹ���<peerAddress>������ - IPv4����IPv6��ȷ��
		* ������CMEInetAddress��û���ṩ���������������ʱ�̶�ΪIPv4
		*/
		TRUE,
		bAutoBind );

	if ( ME_SUCCEEDED(hResult) )
	{
		//m_pSink = pSink;
		m_pReactor = pReactor;
	}

	return hResult;
}

void CMEConnectorAsyncTcp::Close()
{
	m_asyncConnect.Cancel( GetHandle() );

	CMEConnectorTcp::Close();
}

void CMEConnectorAsyncTcp::OnInput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	ME_ASSERTE( FALSE );
}

void CMEConnectorAsyncTcp::OnOutput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	CMEConnectorTcp::OnOutput(
		GetHandle(),
		iEvent );
}

void CMEConnectorAsyncTcp::OnClose( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	CMEConnectorTcp::OnClose(
		GetHandle(),
		iEvent );
}

#elif defined( ME_LINUX )

ME_Result CMEConnectorAsyncTcp::Open(
    IMEConnectorImplSink* pSink,
    const CMEInetAddress& peerAddress,
    CMEReactor* pReactor,
    ME_HANDLE& hHandle,
    CMEInetAddress* pLocalAddress /* = NULL */ )
{
	ME_ASSERTE_RETURN( pSink, ME_ERROR_NULL_POINTER );

	ME_Result hResult = ME_ERROR_FAILURE;
    if ( pLocalAddress )
    {
        hResult = m_SocketTcp.Open(
            *pLocalAddress,
            TRUE );
    }
    else
    {
        hResult = m_SocketTcp.Open( TRUE );
    }

	hHandle = m_SocketTcp.GetHandle();
    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	hResult = m_SocketTcp.Enable( CMEIPBase::NON_BLOCK );
	if ( ME_FAILED(hResult) )
    {
        CMESocketErrorAutoSave errSave;
        ME_ERROR_TRACE_THIS(
            "failed to set NON_BLOCK to socket"
            << ", errno = " << errSave.GetLastError()
            //<< ", socket handle = " << tmpSocket.GetHandle() );
            << ", socket handle = " << m_SocketTcp.GetHandle() );

        return hResult;
    }

	int iTmpIndex = -1;
	hResult = Connect(
        &m_SocketTcp,
        peerAddress,
        pReactor,
        iTmpIndex );

	if ( EWOULDBLOCK == hResult )
    {
        hResult = ME_OK;   // �����ϲ㱾�β����ɹ�
    }
	/* ���亱��������£��������첽��socketҲ�п���һconnect�ͳɹ� */
    else if ( ME_SUCCEEDED(hResult) )
    {
        /**
        * ͬ���ϲ㣬�����������������֪ͨ����Ҫ������֪ͨ
        * ������ɣ�Open()������û������OnConnect()��callback�����ֲ����������
        */
        NotifySuccess();
    }

	if ( ME_SUCCEEDED(hResult) )
    {
        m_pSink = pSink;

        hResult = pReactor->Register(
            this,
            this,
            CMEReactor::EVENT_TYPE_CONNECT | CMEReactor::EVENT_TYPE_CLOSE,
            iTmpIndex );

        ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

        m_iIndex = iTmpIndex;
    }

	return hResult;
}

void CMEConnectorAsyncTcp::Close()
{
	CMESingletonT<CMEReactorAsyncEpoll>::Instance()->UnRegister(
		this,
		CMEReactor::EVENT_TYPE_ALL,
		Internal );

	Internal = -1;
	m_pSink = NULL;
	m_SocketTcp.SetHandle( ME_INVALID_HANDLE );
	m_iIndex = -1;
}

void CMEConnectorAsyncTcp::OnOutput(
	ME_HANDLE hHandle,
	int iEvent )
{
	ME_ASSERTE_RETURN_VOID( m_pSink );
	IMEConnectorImplSink* pTmpSink = m_pSink;

	Close();

	pTmpSink->OnConnect(
		ME_OK,
		hHandle );
}

void CMEConnectorAsyncTcp::OnCompleteImpl( 
	BOOL bResult,
	int iActiveEvent,
	ME_HANDLE hInterface )
{
	AddReference();

	if ( ME_BIT_DISABLED(iActiveEvent, EPOLLERR) )
	{
		OnOutput(
			GetHandle(),
			CMEReactor::EVENT_TYPE_CONNECT );
	}
	else
	{
		CMEConnectorTcp::OnClose(
			GetHandle(),
			CMEReactor::EVENT_TYPE_ERR );
	}
}

void CMEConnectorAsyncTcp::OnDestroy()
{
	RemoveReference();
}

#endif	// ME_WIN


ME_NAME_SPACE_END



