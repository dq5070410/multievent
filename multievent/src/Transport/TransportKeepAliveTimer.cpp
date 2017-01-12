/**
* 定时检测连接是否正常
* 
*
* History
*	3/12/2016		Created
*/

#include "TransportKeepAliveTimer.h"
#include "TransportOperatorKeepAlive.h"
#include "ThreadManager.h"
#include "Thread.h"

ME_NAME_SPACE_BEGIN

CMETransportKeepAliveTimer::CMETransportKeepAliveTimer()
	: m_iHBInterval( 2 )
	, m_iCheckNum( 0 )
{
	ME_Result hResult = Schedule( 
		this, 
		m_iHBInterval, 
		0,
		-1 );
	ME_ASSERTE_RETURN_VOID( ME_SUCCEEDED(hResult) );

    ME_INFO_TRACE_THIS( "Construct.Result=" << hResult<< ", Info=" << SelfInfo() );
}

CMETransportKeepAliveTimer::~CMETransportKeepAliveTimer()
{
    Cancel();

    ME_INFO_TRACE_THIS( "Destruct OK " << ", Info=" << SelfInfo() );
}

ME_Result CMETransportKeepAliveTimer::Register(
	IMETransportTimerOperator* pOperator, 
	INT iInterval  )
{
	ME_ASSERTE_RETURN( pOperator, ME_ERROR_NULL_POINTER );

	m_mapTimerOperator[pOperator] = iInterval;

    ME_INFO_TRACE_THIS( "Register One Operator.Info=" << SelfInfo() );

    return ME_OK;
}

ME_Result CMETransportKeepAliveTimer::UnRegister( IMETransportTimerOperator* pOperator )
{
	ME_ASSERTE_RETURN( pOperator, ME_ERROR_NULL_POINTER );

	m_mapTimerOperator.erase( m_mapTimerOperator.find(pOperator) );

    ME_INFO_TRACE_THIS( "UnRegister One Operator.Info=" << SelfInfo() );

    return ME_OK;
}

ME_Result CMETransportKeepAliveTimer::Update( 
	IMETransportTimerOperator* pOperator, 
	INT iInterval )
{
	OperatorMapType::iterator iter = m_mapTimerOperator.find( pOperator );
	if ( iter == m_mapTimerOperator.end() )
	{
		return ME_ERROR_NOT_FOUND;
	}

	m_mapTimerOperator[pOperator] = iInterval;

	GenInterval();

	ME_Result hResult = Schedule( 
		this, 
		m_iHBInterval, 
		0,
		-1 );
	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	ME_INFO_TRACE_THIS( "pInterval=" << iInterval << ",Update.Info=" << SelfInfo() );

	return ME_OK;
}

VOID CMETransportKeepAliveTimer::OnTimer( CMETimer* pTimerID )
{
    ME_INFO_TRACE_THIS( SelfInfo() );

	/* 
	* 检查到连接断开后（TimerCheck）将会调用UnRegister接口， 
	* 如果不做特殊处理将会导致操作失效iter引起崩溃 
	*/
	OperatorMapType::iterator iter = m_mapTimerOperator.begin();
	OperatorMapType::iterator iterTmp = iter;
	while( iter != m_mapTimerOperator.end() )
	{
		iterTmp = iter++;
		if ( 0 == m_iCheckNum * m_iHBInterval % iterTmp->second )
		{
			ME_ASSERTE_RETURN_VOID( iterTmp->first );

			iterTmp->first->TimerCheck();
		}
	}

	++m_iCheckNum;
}

VOID CMETransportKeepAliveTimer::GenInterval()
{
	for( INT iMaxCommonDivisor = 1; ; ++iMaxCommonDivisor )
	{
		OperatorMapType::iterator iter = m_mapTimerOperator.begin();
		for( ; iter != m_mapTimerOperator.end(); ++iter )
		{
			if ( iter->second % iMaxCommonDivisor )
			{
				m_iHBInterval = iMaxCommonDivisor - 1;

				return;
			}
		}
	}
}

CHAR* CMETransportKeepAliveTimer::SelfInfo()
{
	static CHAR selfInfo[ME_TRACE_AVAILABLE_DATA_LEN];
	::memset( selfInfo, 0, ME_TRACE_AVAILABLE_DATA_LEN );

	CMETraceFormator opFormator( selfInfo, ME_TRACE_AVAILABLE_DATA_LEN );

	opFormator << "Interval=" << m_iHBInterval << ", CheckNum=" << (INT)m_iCheckNum;
	opFormator << "Operators(" << m_mapTimerOperator.size() << "):";
	OperatorMapType::iterator iter = m_mapTimerOperator.begin();
	for ( ; iter != m_mapTimerOperator.end(); ++iter )
	{
		opFormator << iter->first << " " << iter->second << ",";
	}

	return selfInfo;
}

ME_NAME_SPACE_END
