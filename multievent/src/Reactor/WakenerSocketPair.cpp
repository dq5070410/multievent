/**
 *  ����Socket Pair�Ļ������ඨ��
 *
 *  History
 *  01/31/2016 create file
 */


#include "WakenerSocketPair.h"
#include "OSSocket.h"
#include "SocketTcp.h"
#include "InetAddress.h"
#include "LockGuardT.h"
#include "Reactor.h"

ME_NAME_SPACE_BEGIN

/* ���ֱ����127.0.0.1:0�ᵼ��CMEInetAddress����ʧ�� */
const std::string CMEWakenerSocketPair::m_sStrLoopBackIP("127.0.0.1");

CMEWakenerSocketPair::CMEWakenerSocketPair()
    : m_pSocketTcpReader( NULL )
    , m_pSocketTcpWriter( NULL )
    , m_bSignalOn( FALSE )

#ifdef ME_LINUX
	, CMEAsyncIoResult(
		CMEReactor::NOTIFY_TYPE_WAKEUP,
		NULL )

#endif	// ME_LINUX
{

}

CMEWakenerSocketPair::~CMEWakenerSocketPair()
{
    Finalize();
}

void CMEWakenerSocketPair::SetHandle( ME_HANDLE hHandle )
{
    ME_ERROR_TRACE( "Not available" );
}

ME_HANDLE CMEWakenerSocketPair::GetHandle() const
{
    return m_pSocketTcpReader->GetHandle();
}

void CMEWakenerSocketPair::OnInput(
    ME_HANDLE hHandle,
    INT iEvent )
{
    /** 
     *  ��ΪNotify������һ��Reactor�Ĳ��������ڱ����ö�Σ�����ͳ�����ȡֱ����
     *  ���꣬���ɶ��¼�����
     */
    while ( TRUE )
    {
        std::string strSignal( "WakenerSocketPairNotify" );
        char Dumny[24] = { '0' };
        char* pDumny = Dumny;
        INT iBytes = m_pSocketTcpReader->Recv(
            pDumny,
            strSignal.size() );

        if ( strSignal.size() != (UINT)iBytes )
        {
            break;
        }
    }

    /* ��������ζ���źŶ��յ��ˣ���ô�����ٴη��ź�������Reactor�� */
    {
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );
        m_bSignalOn = FALSE;
    }

}

void CMEWakenerSocketPair::OnOutput(
    ME_HANDLE hHandle,
    INT iEvent )
{
    ME_ERROR_TRACE( "Not available" );
}

void CMEWakenerSocketPair::OnClose(
    ME_HANDLE hHandle,
    INT iEvent )
{
    ME_ERROR_TRACE( "Not available" );
}

ME_Result CMEWakenerSocketPair::Notify()
{
//     /* �����Ѿ����ź��ˣ�Reactor��Ȼ�����ѣ������ظ����ź� */
//     if ( m_bSignalOn )
//     {
//         return ME_OK;
//     }

    {
        /* ������һ��ֻ�ܷ���һ���ź� */
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );
        if ( m_bSignalOn )
        {
            return ME_OK;
        }

        m_bSignalOn = TRUE;
    }

    /* ������һ���߳��ܹ������﷢���ź� */
    std::string strSignal( "WakenerSocketPairNotify" );
    INT iBytes = m_pSocketTcpWriter->Send(
        strSignal.data(),
        strSignal.size() );

    /* ����Ӧ�ò�̫���ܳ�����������ˣ�������Ӧ���������㹻�Ŀռ������ź� */
    if ( strSignal.size() != (UINT)iBytes )
    {
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

        m_bSignalOn = FALSE;

        ME_ASSERTE_RETURN( FALSE, ME_ERROR_NETWORK_SOCKET_ERROR );
    }

    return ME_OK;
}

ME_Result CMEWakenerSocketPair::Initialize()
{
    ME_ASSERTE_RETURN( (NULL == m_pSocketTcpReader && NULL == m_pSocketTcpWriter), ME_ERROR_ALREADY_INITIALIZED );
    ME_Result hResult = ME_ERROR_FAILURE;
    CMEInetAddress hiaLocalAddr( m_sStrLoopBackIP.c_str(), 0 );

    m_pSocketTcpReader = new CMESocketTcp();
    m_pSocketTcpWriter = new CMESocketTcp();
    CMESocketTcp* pSocketTcpTempListener = new CMESocketTcp();

    do 
    {
        hResult = pSocketTcpTempListener->Open( TRUE );
        if ( ME_FAILED(hResult) )
        {
            break;
        }

        BOOL bTcpNoDelay = 1;
        hResult = pSocketTcpTempListener->SetOption(
            IPPROTO_TCP,
            TCP_NODELAY,
            (char*)&bTcpNoDelay,
            sizeof(bTcpNoDelay) );
        if ( ME_FAILED(hResult) )
        {
            break;
        }

        hResult = pSocketTcpTempListener->Listen( hiaLocalAddr );
        if ( ME_FAILED(hResult) )
        {
            break;
        }

        hResult = pSocketTcpTempListener->GetLoacalAddress( hiaLocalAddr );
        if ( ME_FAILED(hResult) )
        {
            break;
        }

        hResult = m_pSocketTcpWriter->Open( TRUE );
        if ( ME_FAILED(hResult) )
        {
            break;
        }

        hResult = OS::SocketConnect(
            m_pSocketTcpWriter->GetHandle(),
            hiaLocalAddr.GetRawAddress() );
        if ( ME_FAILED(hResult) )
        {
            break;
        }

        ME_HANDLE hhReader = ME_INVALID_HANDLE;
        hResult = pSocketTcpTempListener->Accept( hhReader, hiaLocalAddr );
        if ( ME_FAILED(hResult) )
        {
            break;
        }

        m_pSocketTcpReader->SetHandle( hhReader );

        // д���׽�������Ϊ�첽
        hResult = m_pSocketTcpWriter->Enable( CMEIPBase::NON_BLOCK );
        if ( SOCKET_ERROR == hResult )
        {
            break;
        }

        // �����׽�������Ϊ�첽
        hResult = m_pSocketTcpReader->Enable( CMEIPBase::NON_BLOCK );
        if ( SOCKET_ERROR == hResult )
        {
            break;
        }

    } while ( FALSE );

    pSocketTcpTempListener->Close();
    ME_DELETE( pSocketTcpTempListener );

    if ( ME_FAILED(hResult) )
    {
        m_pSocketTcpWriter->Close();
        ME_DELETE( m_pSocketTcpWriter );
        m_pSocketTcpReader->Close();
        ME_DELETE( m_pSocketTcpReader );
    }

    return hResult;
}

ME_Result CMEWakenerSocketPair::Finalize()
{
    if ( NULL != m_pSocketTcpWriter )
    {
        m_pSocketTcpWriter->Close();
        delete m_pSocketTcpWriter;
        m_pSocketTcpWriter = NULL;
    }

    if ( NULL != m_pSocketTcpReader )
    {
        m_pSocketTcpReader->Close();
        delete m_pSocketTcpReader;
        m_pSocketTcpReader = NULL;
    }

    return ME_OK;
}

#ifdef ME_LINUX

void CMEWakenerSocketPair::OnCompleteImpl( 
	BOOL bResult,
	int iActiveEvent,
	ME_HANDLE hInterface )
{
	ME_ASSERTE( TRUE == bResult );

	OnInput(
		GetHandle(),
		iActiveEvent );
}

void CMEWakenerSocketPair::OnDestroy()
{

}

#endif	// ME_LINUX

ME_NAME_SPACE_END

