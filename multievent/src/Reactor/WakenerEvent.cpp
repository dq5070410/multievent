/**
 *  基于Windows事件的唤醒者实现
 *
 *
 *  History
 *  01/31/2016 create file
 */


#include "WakenerEvent.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

CMEWakenerEvent::CMEWakenerEvent()
    : m_hEvent( NULL )
{

}

CMEWakenerEvent::~CMEWakenerEvent()
{
    /* 防止意外发生，在这里也检查一下是不是句柄没关掉 */
    Finalize();
}

void CMEWakenerEvent::SetHandle( ME_HANDLE hHandle )
{
    ME_ERROR_TRACE( "Not available" );
}

ME_HANDLE CMEWakenerEvent::GetHandle() const
{
    return m_hEvent;
}

ME_Result CMEWakenerEvent::Notify()
{
    ME_ASSERTE_RETURN( (NULL != m_hEvent), ME_ERROR_NOT_INITIALIZED );

    if ( SetEvent(m_hEvent) )
    {
        return ME_OK;
    }
    else
    {
        return ME_ERROR_FAILURE;   // 错误码待定
    }
}

void CMEWakenerEvent::OnInput(
    ME_HANDLE hHandle,
    INT iEvent )
{
    ME_INFO_TRACE_THIS( "" );
}

void CMEWakenerEvent::OnOutput(
    ME_HANDLE hHandle,
    INT iEvent )
{
    ME_INFO_TRACE_THIS( "" );
}

void CMEWakenerEvent::OnClose(
    ME_HANDLE hHandle,
    INT iEvent )
{
    ME_INFO_TRACE_THIS( "" );
}

ME_Result CMEWakenerEvent::Initialize()
{
    ME_ASSERTE_RETURN( (NULL == m_hEvent), ME_ERROR_ALREADY_INITIALIZED );

    m_hEvent = ::CreateEvent(
        NULL,
        FALSE,
        FALSE,
        NULL );
    
    ME_ASSERTE_RETURN( (NULL != m_hEvent), ME_ERROR_FAILURE );

    return ME_OK;
}

ME_Result CMEWakenerEvent::Finalize()
{
    if ( NULL != m_hEvent )
    {
        ::CloseHandle( m_hEvent );
        m_hEvent = NULL;
    }

    return ME_OK;
}

ME_NAME_SPACE_END

#endif	// ME_WIN
