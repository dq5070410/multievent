/**
 *  �����¼����е��̶߳���֪ͨ�¼����н��������һ���¼�����
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

