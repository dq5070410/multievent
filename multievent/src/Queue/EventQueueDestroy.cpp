/**
 *  带有事件队列的线程对象通知事件队列结束的最后一个事件定义
 *
 *
 *  History
 *  03/18/2013 create file
 */


#include "EventQueueDestroy.h"

ME_NAME_SPACE_BEGIN

CMEEventQueueDestroy::CMEEventQueueDestroy( CMEEventQueue* pEventQueue )
    : m_pEventQueue( pEventQueue )
{

}

CMEEventQueueDestroy::~CMEEventQueueDestroy()
{
    m_pEventQueue = NULL;
}

ME_Result CMEEventQueueDestroy::OnEvent()
{
    ME_ASSERTE_RETURN( (NULL != m_pEventQueue), ME_ERROR_NULL_POINTER );

    m_pEventQueue->Destroy();

    return ME_OK;
}

ME_NAME_SPACE_END

