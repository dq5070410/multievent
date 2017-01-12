/**
 *  带有事件队列，时钟队列以及分离器的线程对象类定义
 *
 *
 *  History
 *  02/01/2016 create file
 */


#include "ThreadEQTQR.h"


ME_NAME_SPACE_BEGIN

CMEThreadEQTQR::CMEThreadEQTQR(
    BOOL bBlock,
    int iThreadID,
    INT iType,
    CMEReactor* pReactor,
    CMETimerQueue* pTimerQueue,
    CMEEventQueue* pEventQueue,
    IMEThreadSink* pThreadSink )
    : CMEThreadReactor(
        bBlock,
        iThreadID,
        iType,
        pReactor,
        pTimerQueue,
        pEventQueue,
        pThreadSink )
{

}

CMEThreadEQTQR::~CMEThreadEQTQR()
{

}

ME_NAME_SPACE_END

