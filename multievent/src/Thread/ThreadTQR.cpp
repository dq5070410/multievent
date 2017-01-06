/**
 *  带有时钟队列和分离器的线程对象类定义
 *
 *
 *  History
 *  02/01/2016 create file
 */


#include "ThreadTQR.h"


ME_NAME_SPACE_BEGIN

CMEThreadTQR::CMEThreadTQR(
    BOOL bBlock,
    INT iThreadID,
    INT iType,
    CMEReactor* pReactor,
    CMETimerQueue* pTimerQueue,
    IMEThreadSink* pThreadSink )
    : CMEThreadReactor(
        bBlock,
        iThreadID,
        iType,
        pReactor,
        pTimerQueue,
        NULL,
        pThreadSink)
{

}

CMEThreadTQR::~CMEThreadTQR()
{

}

ME_Result CMEThreadTQR::PostEvent( IMECustomizeEvent* pEvent )
{
    ME_INFO_TRACE_THIS( "pEvent: " << pEvent );

    return ME_ERROR_NOT_AVAILABLE;
}

ME_Result CMEThreadTQR::SendEvent( IMECustomizeEvent* pEvent )
{
    ME_INFO_TRACE_THIS( "pEvent: " << pEvent );

    return ME_ERROR_NOT_AVAILABLE;
}

ME_NAME_SPACE_END

