/**
 *  带有事件队列的线程对象通知事件队列结束的最后一个事件定义
 *
 *
 *  History
 *  03/18/2016 create file
 */


#include "EventQueueDestroy.h"

ME_NAME_SPACE_BEGIN

CEventQueueDestroy::CEventQueueDestroy( CEventQueue* pEventQueue )
    : m_pEventQueue( pEventQueue )
{

}

CEventQueueDestroy::~CEventQueueDestroy()
{
    m_pEventQueue = NULL;
}

ME_Result CEventQueueDestroy::OnEvent()
{
    ME_ASSERTE_RETURN( (NULL != m_pEventQueue), ME_ERROR_NULL_POINTER );

    m_pEventQueue->Destroy();

    return ME_OK;
}

ME_NAME_SPACE_END

