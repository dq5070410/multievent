/**
 *  带有事件队列的线程对象通知事件队列结束的最后一个事件声明
 *
 *  History
 *  03/18/2013 create file
 */


#ifndef __EVENT_QUEUE_DESTROY_H_
#define __EVENT_QUEUE_DESTROY_H_

#include "Base.h"
#include "CustomizeEvent.h"
#include "EventQueueT.h"

NAME_SPACE_BEGIN

class CEventQueueDestroy : public ICustomizeEvent
{
public:
    CEventQueueDestroy( CEventQueue* pEventQueue );

    virtual ~CHikEventQueueDestroy();

    virtual ME_Result OnEvent();

private:
    CEventQueue*                 m_pEventQueue;
};

NAME_SPACE_END

#endif // __EVENT_QUEUE_DESTROY_H_

