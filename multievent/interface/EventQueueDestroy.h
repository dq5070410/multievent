/**
 *  �����¼����е��̶߳���֪ͨ�¼����н��������һ���¼�����
 *
 *  History
 *  03/18/2016 create file
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

    virtual ~CEventQueueDestroy();

    virtual ME_Result OnEvent();

private:
    CEventQueue*                 m_pEventQueue;
};

NAME_SPACE_END

#endif // __EVENT_QUEUE_DESTROY_H_

