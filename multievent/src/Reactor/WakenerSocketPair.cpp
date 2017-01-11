/**
 *  基于Socket Pair的唤醒者类定义
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

/* 如果直接用127.0.0.1:0会导致CMEInetAddress构造失败 */
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
     *  因为Notify可能在一个Reactor的捕获周期内被调用多次，这里就持续读取直到都
     *  读完，将可读事件消除
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

    /* 到这里意味着信号都收掉了，那么允许再次发信号来唤醒Reactor了 */
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
//     /* 现在已经有信号了，Reactor必然会苏醒，不必重复发信号 */
//     if ( m_bSignalOn )
//     {
//         return ME_OK;
//     }

    {
        /* 加锁，一次只能发出一个信号 */
        CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );
        if ( m_bSignalOn )
        {
            return ME_OK;
        }

        m_bSignalOn = TRUE;
    }

    /* 至多有一个线程能够到这里发出信号 */
    std::string strSignal( "WakenerSocketPairNotify" );
    INT iBytes = m_pSocketTcpWriter->Send(
        strSignal.data(),
        strSignal.size() );

    /* 现在应该不太可能出现这种情况了，缓冲区应该总是有足够的空间来放信号 */
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

        // 写端套接字设置为异步
        hResult = m_pSocketTcpWriter->Enable( CMEIPBase::NON_BLOCK );
        if ( SOCKET_ERROR == hResult )
        {
            break;
        }

        // 读端套接字设置为异步
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

