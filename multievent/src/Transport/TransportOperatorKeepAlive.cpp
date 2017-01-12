/**
 *  用作TCP自动检测该连接是否还是可用处理类
 *
 *  History
 *  03/28/2016 create file
 */

#include "TransportOperatorKeepAlive.h"
#include "TransportKeepAliveTimer.h"
#include "MessageBlock.h"
#include "LockThreadMutex.h"
#include "ConnectionInterface.h"
#include "ThreadManager.h"
#include "PDU.h"
#include "PDUDefines.h"

ME_NAME_SPACE_BEGIN

CMETransportOperatorKeepAlive::CMETransportOperatorKeepAlive( IMETransport* pTransport )
    : m_pTransport( pTransport )
    , m_tvActiveTime( CMETimeValue::TimeOfDay() )
	, m_bLastSendOK( TRUE )
	, m_nMaxCheckNum( 3 )
	, m_nCheckNum( m_nMaxCheckNum )
	, m_iHeadType( KEEPALIVE_TYPE_COMMON )
	, m_bNewPackage( TRUE )
{
	ME_Result hResult = CMETransportKeepAliveTimerSingleton::Instance()->Register( 
		this, 
		2 );

	ME_INFO_TRACE_THIS( "Construct.Result=" << hResult << " " << SelfInfo() );
}

CMETransportOperatorKeepAlive::~CMETransportOperatorKeepAlive()
{
	ME_Result hResult = CMETransportKeepAliveTimerSingleton::Instance()->UnRegister( this );

	ME_INFO_TRACE_THIS( "UnConstruct.Result=" << hResult << " " << SelfInfo() );
}

ME_Result CMETransportOperatorKeepAlive::PreSendData( CMEMessageBlock*& pmbBlock )
{
	ME_ASSERTE_RETURN( pmbBlock, ME_ERROR_NULL_POINTER );

	if ( TRUE == m_bNewPackage )
	{
		if ( KEEPALIVE_TYPE_REQ == m_iHeadType )
		{
			AddKeepAliveHead( *pmbBlock, KEEPALIVE_TYPE_REQ );
		}
		else if ( KEEPALIVE_TYPE_RSP == m_iHeadType )
		{
			AddKeepAliveHead( *pmbBlock, KEEPALIVE_TYPE_RSP );
		}
		else
		{
			AddKeepAliveHead( *pmbBlock, KEEPALIVE_TYPE_COMMON );
		}

		m_bNewPackage = FALSE;

		m_iHeadType = KEEPALIVE_TYPE_COMMON;
	}

	return ME_OK;
}

ME_Result CMETransportOperatorKeepAlive::PostSendData( CMEMessageBlock*& pmbBlock )
{
	ME_ASSERTE_RETURN( pmbBlock, ME_ERROR_NULL_POINTER );

	m_tvActiveTime.Set( CMETimeValue::TimeOfDay() );

	if ( pmbBlock->GetWritePos() == pmbBlock->GetReadPos() )
	{
		m_bLastSendOK = TRUE;

		/* 这个完整的包已经发送完成了 */
		m_bNewPackage = TRUE;
	}
	else
	{
		m_bLastSendOK = FALSE;
	}

	return ME_OK;
}

ME_Result CMETransportOperatorKeepAlive::PreOnReceive( CMEMessageBlock*& pmbBlock )
{
	ME_ASSERTE_RETURN( pmbBlock && m_pTransport, ME_ERROR_NULL_POINTER );
	ME_ASSERTE_RETURN( (0 == pmbBlock->GetReadPos()), ME_ERROR_UNEXPECTED );

	ME_INFO_TRACE_THIS( SelfInfo() << ", MB=" << pmbBlock << ", MBLength=" << pmbBlock->GetLength() );

	ME_Result hResult = RETURN_VALUE_OK;

	CMEPDUBase pduBase;
	pduBase.Decode( *pmbBlock );

	BYTE byType = pduBase.GetType();
	if ( PDUTYPE_KEEPALIVE_REQ == byType )
	{
		/* 收到心跳请求包 */
		if ( TRUE == Idle() )
		{
			SendHBMessage( KEEPALIVE_TYPE_RSP );
		}
		else
		{
			ME_INFO_TRACE_THIS( "Transport Is Busy, Do Not Need To Send Reply Package" << SelfInfo() );
		}

		hResult = RETURN_VALUE_FINISH;

		ME_INFO_TRACE_THIS( "Recv One HeartBeat Req Package" );
	}
	else if ( PDUTYPE_KEEPALIVE_RSP == byType )
	{
		/* 收到心跳确认包 */
		hResult = RETURN_VALUE_FINISH;

		ME_INFO_TRACE_THIS( "Recv One HeartBeat Reply Package" );
	}

	/* 凡是收到包都设置下面两个字段 */
	m_tvActiveTime.Set( CMETimeValue::TimeOfDay() );
	m_nCheckNum = m_nMaxCheckNum;

	return hResult;
}

DWORD CMETransportOperatorKeepAlive::GetType()
{
	return IMEConnectionManager::CONNECTION_TYPE_KEEP_ALIVE;
}

ME_Result CMETransportOperatorKeepAlive::SetOption( DWORD dwOptionType, VOID* pOptionValue )
{
	ME_INFO_TRACE_THIS( SelfInfo() << ", OptionType=" << dwOptionType );

	UINT* pValue = (UINT*)pOptionValue;
	ME_ASSERTE_RETURN( pValue, ME_ERROR_INVALID_ARG );

	switch( dwOptionType )
	{
	case IMETransport::OPTION_TYPE_SET_KEEPALIVE_INTERVEL:
	{	
		SetInterval( *pValue );

		break;
	}

	case IMETransport::OPTION_TYPE_SET_KEEPALIVE_CHECKNUM:
	{
		SetCheckNum( *pValue );

		break;
	}

	default:
	{
		/* 不接受其他的选项 */
		ME_ASSERTE_RETURN( FALSE, ME_ERROR_INVALID_ARG );

		break;
	}

	}

	return RETURN_VALUE_OK;
}

VOID CMETransportOperatorKeepAlive::TimerCheck( )
{
	ME_INFO_TRACE_THIS( SelfInfo() );

	ME_ASSERTE_RETURN_VOID( m_pTransport );

	if ( NULL == m_pTransport->GetSink() )
	{
		/* 还没有Open */
		ME_INFO_TRACE_THIS( "Transport Have Not Open" );

		return;
	}

	if ( FALSE == Idle() )
	{
		/* 不空闲 */
		ME_INFO_TRACE_THIS( "Transport is Busy,Do Not Need To Send HB Package" << SelfInfo() );

		return;
	}

	if ( TRUE == IsTimeOut() )
	{
		/* 超时 */
		ME_INFO_TRACE_THIS( "Transport Disconnect" );

		HandleTimerOut();

		return;
	}

	/* 链接在空闲、并且没有超时的情况下，发送心跳请求进行检查 */
	SendHBMessage( KEEPALIVE_TYPE_REQ );

	--m_nCheckNum;

	ME_INFO_TRACE_THIS( "Send HB Request Package." << SelfInfo() );
}

BOOL CMETransportOperatorKeepAlive::IsTimeOut()
{
   /**
    * 如果尝试次数等于0则认为超时
	*/
	if ( 0 == m_nCheckNum )
	{
		return TRUE;
	}

    return FALSE;
}

ME_Result CMETransportOperatorKeepAlive::HandleTimerOut()
{
	/* 
	* 关闭HikTransportWrapper， 由于网络线程可能存在还等待发送的数据，
	* 这时如果这里Close将会导致发送时崩溃
	*/
	EventCloseWrapper* pEventCloseWrapper = new EventCloseWrapper( 
		m_pTransport,
		ME_ERROR_TIMEOUT );
	pEventCloseWrapper->Run( CMEThreadManager::Instance()->GetNetworkThread() );

	/* 通知上层链接断开 */
	m_pTransport->GetSink()->OnDisconnect( 
		m_pTransport, 
		ME_ERROR_TIMEOUT );

	return ME_OK;
}

BOOL CMETransportOperatorKeepAlive::Idle()
{
   /**
	* 判断链接繁忙的条件有两个：
	* 1，上一次调用SendData接口发送失败
	* 2，前1s发送或接收了数据
	*/
	if ( FALSE == m_bLastSendOK ||
		 CMETimeValue::TimeOfDay().GetSecond() - m_tvActiveTime.GetSecond() <= 1 )
	{
		return FALSE;
	}

	return TRUE;
}

VOID CMETransportOperatorKeepAlive::SendHBMessage( CMETransportOperatorKeepAlive::KEEPALIVE_TYPE iType )
{
	m_iHeadType = iType;

	CMEMessageBlock mbKeepAliveReq( 0 );
	ME_Result hResult = m_pTransport->SendData( &mbKeepAliveReq );
	if ( ME_ERROR_WOULD_BLOCK == hResult ||
		ME_ERROR_NETWORK_PARTIAL_DATA == hResult )
	{
		/* 心跳包是在空闲时刻发送的包，如果发送返回这两个返回值，则直接assert */
		ME_ASSERTE_RETURN_VOID( FALSE );
	}
}

ME_Result CMETransportOperatorKeepAlive::SetInterval( UINT nInterval )
{
	KeepAliveSheduleTimer* pEvent = NULL;

	/* 抛到网络线程处理 */
	pEvent = new KeepAliveSheduleTimer( 
		CMETransportKeepAliveTimerSingleton::Instance(), 
		this,
		nInterval );

	pEvent->Run( CMEThreadManager::Instance()->GetNetworkThread() );

	return ME_OK;
}

ME_Result CMETransportOperatorKeepAlive::SetCheckNum( UINT nCheckNum )
{
	m_nMaxCheckNum = nCheckNum;

	m_nCheckNum = m_nMaxCheckNum;

	return ME_OK;
}

ME_Result CMETransportOperatorKeepAlive::AddKeepAliveHead( 
	CMEMessageBlock& mbSend,
	CMETransportOperatorKeepAlive::KEEPALIVE_TYPE iType )
{
	CMEPDUBase* pKeepAlivePDU = NULL;

	if ( KEEPALIVE_TYPE_REQ == iType )
	{
		pKeepAlivePDU = new CMEPDUKeepAliveReq;
	}
	else if ( KEEPALIVE_TYPE_RSP == iType )
	{
		pKeepAlivePDU = new CMEPDUKeepAliveRsp;
	}
	else
	{
		pKeepAlivePDU = new CMEPDUBase;
	}

	/* base pdu 只有一个字节 */
	CMEMessageBlock mbHead( pKeepAlivePDU->GetLength() );
	pKeepAlivePDU->Encode( mbHead );

	/* 在头部添加 */
	mbSend.Append( mbHead, FALSE );

	ME_DELETE( pKeepAlivePDU );

	return ME_OK;
}

CHAR* CMETransportOperatorKeepAlive::SelfInfo()
{
	static CHAR selfInfo[ME_TRACE_AVAILABLE_DATA_LEN];
	::memset( selfInfo, 0, ME_TRACE_AVAILABLE_DATA_LEN );

	CMETraceFormator opFormator( selfInfo, ME_TRACE_AVAILABLE_DATA_LEN );
	opFormator 
		<< ", Transport" << m_pTransport
		<< ", LastActiveTime=" << m_tvActiveTime.GetSecond()
		<< ", LastSendResult=" << m_bLastSendOK
		<< ", MaxCheckNum=" << m_nMaxCheckNum
		<< ", CheckNum=" << m_nCheckNum
		<< ", TimeNow" << CMETimeValue::TimeOfDay().GetSecond();

	return selfInfo;
}

ME_NAME_SPACE_END
