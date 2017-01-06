/**
 *  线程对象ID管理器定义
 *
 *  History
 *  02/18/2016 create file
 */


#include "ThreadIDManager.h"


ME_NAME_SPACE_BEGIN

CMEThreadIDManager::CMEThreadIDManager()
    : m_iNewNetworkThreadID( ME_NETWORK_THREAD_ID_BEGIN )
    , m_iNewUserThreadID( ME_USER_THREAD_ID_BEGIN )
    , m_iExternalThreadID( ME_EXTERNAL_THREAD_ID_BEGIN )
{

}

CMEThreadIDManager::~CMEThreadIDManager()
{

}

INT CMEThreadIDManager::GenerateThreadID( INT iClassificationType )
{
    ME_INFO_TRACE_THIS( "iClassificationType: " << iClassificationType );

    switch ( iClassificationType )
    {
    case CLASSIFICATION_TYPE_NETWORK:
        {
            ME_ASSERTE_RETURN( (m_iNewNetworkThreadID < ME_USER_THREAD_ID_BEGIN), 0 );
            
            return m_iNewNetworkThreadID++;
        }
        
    case CLASSIFICATION_TYPE_USER:
        {
            ME_ASSERTE_RETURN( (m_iNewUserThreadID < ME_EXTERNAL_THREAD_ID_BEGIN), 0 );

            return m_iNewUserThreadID++;
        }

    case CLASSIFICATION_TYPE_EXTERNAL:
        {
            ME_INFO_TRACE_THIS( "" );

            return m_iExternalThreadID++;
        }

    default:
        {
            return 0;
        }
    }
}

BOOL CMEThreadIDManager::IsExternalThread( INT iThreadID )
{
    ME_INFO_TRACE_THIS( "iThreadID: " << iThreadID );

    return GetThreadClassificationType( iThreadID ) == CLASSIFICATION_TYPE_EXTERNAL ? TRUE : FALSE;
}

INT CMEThreadIDManager::GetThreadClassificationType( INT iThreadID )
{
    ME_INFO_TRACE_THIS( "iThreadID: " << iThreadID );
    ME_ASSERTE_RETURN( (0 < iThreadID), ME_ERROR_INVALID_ARG );

    /* [1, 100]，网络线程 */
    if ( iThreadID < ME_USER_THREAD_ID_BEGIN )
    {
        return CLASSIFICATION_TYPE_NETWORK;
    }

    /* [2001, +∞]，外部线程 */
    if ( iThreadID >= ME_EXTERNAL_THREAD_ID_BEGIN )
    {
        return CLASSIFICATION_TYPE_EXTERNAL;
    }

    /* 其余的都是用户线程 */
    return CLASSIFICATION_TYPE_USER;
}

ME_NAME_SPACE_END

