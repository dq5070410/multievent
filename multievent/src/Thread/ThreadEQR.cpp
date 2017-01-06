/**
 *  �����¼����кͷ��������̶߳�����ʵ��
 *
 *
 *  History
 *  02/01/2013 create file
 */


#include "ThreadEQR.h"


ME_NAME_SPACE_BEGIN

CMEThreadEQR::CMEThreadEQR(
    BOOL bBlock,
    INT iThreadID,
    INT iType,
    CMEReactor* pReactor,
    CMEEventQueue* pEventQueue,
    IMEThreadSink* pThreadSink )
    : CMEThreadReactor(
        bBlock,
        iThreadID,
        iType,
        pReactor,
        NULL,
        pEventQueue,
        pThreadSink )
{

}

CMEThreadEQR::~CMEThreadEQR()
{

}

ME_Result CMEThreadEQR::ScheduleTimer(
    CMETimer* pTimer,
    IMETimerSink* pSink,
    const CMETimeValue& htvInterval,
    const CMETimeValue& htvDelay,
    UINT nLoopTime /* = 0 */ )
{
    /* Ӧ�ò���ֱ��������ʵ������ע�� */
    ME_INFO_TRACE_THIS( "pTimer: " << pTimer
        << ", pSink: " << pSink
//         << ", htvInterval: " << htvInterval
//         << ", htvDelay: " << htvDelay
        << ", nLoopTime: " << nLoopTime );

    return ME_ERROR_NOT_AVAILABLE;
}

ME_Result CMEThreadEQR::CancelTimer( CMETimer* pTimer )
{
    ME_INFO_TRACE_THIS( "pTimer: " << pTimer );

    return ME_ERROR_NOT_AVAILABLE;
}

ME_NAME_SPACE_END