/**
 *  IO捕捉与分派器的壳定义
 *
 *
 *  History
 *  12/12/2016 create file
 */

#include "Reactor.h"
#include "LockThreadMutex.h"
#include "LockGuardT.h"
#include "ReactorImpl.h"

ME_NAME_SPACE_BEGIN

CMEReactor::CMEReactor( IMEReactorImpl* pReactorImpl )
    : m_pReactorImpl( pReactorImpl )
{

}

CMEReactor::~CMEReactor()
{

}

ME_Result CMEReactor::Run( INT iRunMode /* = RUN_MODE_LOOP */ )
{
    ME_INFO_TRACE_THIS( "iRunMode: " << iRunMode );
    ME_ASSERTE_RETURN( (NULL != m_pReactorImpl), ME_ERROR_NULL_POINTER );
    ME_ASSERTE_RETURN( (RUN_MODE_LOOP == iRunMode || RUN_MODE_HEART_BEAT == iRunMode), ME_ERROR_INVALID_ARG );

    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    ME_Result hResult = ME_ERROR_FAILURE;

    do
    {
        /* 开始捕获事件 */
        hResult = m_pReactorImpl->HandleEvent();

        /* 开始分派事件 */
        if ( ME_OK == hResult || ME_ERROR_TIMEOUT == hResult )
        {
            hResult = m_pReactorImpl->Dispatch( FALSE );
        }

        /* 分派结束后会知道分离器是否需要退出，这时做最后一次分派（搞定所有事件） */
        if ( RUN_STATE_EXIT == hResult )
        {
            hResult = m_pReactorImpl->Dispatch( TRUE );
            break;
        }

        /* 如果是心跳模式就需要跳出循环 */
        if ( RUN_MODE_HEART_BEAT == iRunMode )
        {
            break;
        }

    } while ( TRUE );

    return hResult;
}

ME_Result CMEReactor::Notify( INT iNotifyType )
{
    ME_INFO_TRACE_THIS( "iNotifyType: " << iNotifyType );
    ME_ASSERTE_RETURN( (NULL != m_pReactorImpl), ME_ERROR_NULL_POINTER );
    ME_ASSERTE_RETURN( (NOTIFY_TYPE_EXIT == iNotifyType || NOTIFY_TYPE_WAKEUP == iNotifyType), ME_ERROR_INVALID_ARG );

    return m_pReactorImpl->Notify( iNotifyType );
}

ME_Result CMEReactor::Register(
    IMEIO* pIO,
    IMEIOSink* pIOSink,
    INT iEvent,
    INT& iIndex )
{
    ME_INFO_TRACE_THIS( "pIO: " << pIO
        << ", pIOSink: " << pIOSink
        << ", iEvent: " << iEvent
        << ", iIndex: " << iEvent );
    ME_ASSERTE_RETURN( (NULL != m_pReactorImpl), ME_ERROR_NULL_POINTER );
    ME_ASSERTE_RETURN( (NULL != pIO && NULL != pIOSink && (EVENT_TYPE_ALL & iEvent)), ME_ERROR_INVALID_ARG );

    return m_pReactorImpl->Register(
        pIO,
        pIOSink,
        iEvent,
        iIndex );
}

ME_Result CMEReactor::UnRegister(
    IMEIO* pIO,
    INT iEvent,
    INT iIndex )
{
    ME_INFO_TRACE_THIS( "pIO: " << pIO
        << ", iEvent: " << iEvent
        << ", iIndex: " << iIndex );
    ME_ASSERTE_RETURN( (NULL != m_pReactorImpl), ME_ERROR_NULL_POINTER );
    ME_ASSERTE_RETURN( (NULL != pIO && (EVENT_TYPE_ALL & iEvent)), ME_ERROR_INVALID_ARG );

    return m_pReactorImpl->UnRegister(
        pIO,
        iEvent,
        iIndex );
}

ME_Result CMEReactor::AttachEventQueue( CMEEventQueue* pEq )
{
    ME_INFO_TRACE_THIS( "pEq: " << pEq );
    ME_ASSERTE_RETURN( (NULL != m_pReactorImpl), ME_ERROR_NULL_POINTER );
    ME_ASSERTE_RETURN( (NULL != pEq), ME_ERROR_INVALID_ARG );

    return m_pReactorImpl->AttachEventQueue( pEq );
}

ME_Result CMEReactor::AttachTimerQueue( CMETimerQueue* pTq )
{
    ME_INFO_TRACE_THIS( _ME_FUNCTION_
        << ", pTq: " << pTq );
    ME_ASSERTE_RETURN( (NULL != m_pReactorImpl), ME_ERROR_NULL_POINTER );
    ME_ASSERTE_RETURN( (NULL != pTq), ME_ERROR_INVALID_ARG );

    return m_pReactorImpl->AttachTimerQueue( pTq );
}

int CMEReactor::GetType() const
{
	return m_pReactorImpl->GetType();
}

ME_NAME_SPACE_END

