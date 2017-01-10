/**
 *  description
 *
 *
 *  History
 *  02/19/2013 create file
 */


#include "ReactorFactory.h"
#include "Reactor.h"
#include "ReactorSelect.h"
#include "ReactorWFMO.h"
#include "WakenerSocketPair.h"
#include "WakenerEvent.h"
#include "WakenerAsyncIOCP.h"
#include "ReactorAsyncIOCP.h"
#include "ReactorAsyncEpoll.h"
#include "ThreadIDManager.h"

ME_NAME_SPACE_BEGIN

ME_Result CMEReactorFactory::Create(
    INT iType,
	INT iClassificationType,
    CMEReactor*& pReactor )
{
    ME_INFO_TRACE_THIS( "iType: " << iType );

    /* 先限制传入的输出用的Reactor指针必须为NULL */
    ME_ASSERTE_RETURN( (NULL == pReactor), ME_ERROR_INVALID_ARG );

    ME_Result hResult = ME_ERROR_FAILURE;
    IMEWakener* pWakener = NULL;
    IMEReactorImpl* pReactorImpl = NULL;

    do 
    {
        /* 创建基于select的分离器 */
        if ( CMEReactor::REACTOR_TYPE_SELECT == iType )
        {
			if ( CMEThreadIDManager::CLASSIFICATION_TYPE_NETWORK != iClassificationType )
			{
				pWakener = new CMEWakenerSocketPair();
				hResult = pWakener->Initialize();
				if ( ME_FAILED(hResult) )
				{
					break;
				}
				pReactorImpl = new CMEReactorSelect( pWakener );
				pReactor = new CMEReactor( pReactorImpl );
				hResult = ME_OK;
			}
			else
			{
#ifdef ME_WIN
				pReactor = Create(
					CMEReactor::REACTOR_TYPE_ASYCH_IOCP,
					0 );

#elif defined( ME_LINUX )
				pReactor = Create(
					CMEReactor::REACTOR_TYPE_ASYCH_EPOLL,
					0 );

#endif	// ME_WIN

				hResult = ME_OK;
			}
        }

#ifdef ME_WIN
        /* 创建基于WaitForMultipleObjects的分离器 */
        else if ( CMEReactor::REACTOR_TYPE_WFMO == iType )
        {
            pWakener = new CMEWakenerEvent();
            hResult = pWakener->Initialize();
            if ( ME_FAILED(hResult) )
            {
                break;
            }
            pReactorImpl = new CMEReactorWFMO( pWakener );
            pReactor = new CMEReactor( pReactorImpl );
            hResult = ME_OK;
        }
#endif	// ME_WIN
        /* 其他的类型都认为是错误的 */
        else
        {
            ME_ASSERTE_RETURN( FALSE, ME_ERROR_INVALID_ARG );
        }

    } while ( FALSE );

    /* 异常情况需要回收已创建的资源 */
    if ( ME_FAILED(hResult) )
    {
        ME_DELETE( pReactor );
        ME_DELETE( pReactorImpl );
        ME_DELETE( pWakener );
    }

    return hResult;
}

ME_Result CMEReactorFactory::Destroy( CMEReactor*& pReactor )
{
    ME_INFO_TRACE_THIS( "" );

    if ( NULL != pReactor )
    {
		IMEWakener* pWakener = NULL;

		if ( ME_BIT_DISABLED(pReactor->GetType(), CMEReactor::REACTOR_TYPE_ASYNC) )
		{
			CMEReactorBase* pReactorBase = dynamic_cast<CMEReactorBase*>( pReactor->m_pReactorImpl );
			ME_ASSERTE_RETURN( (NULL != pReactorBase), ME_ERROR_INVALID_ARG );
			pWakener = pReactorBase->m_pWakener;
			ME_DELETE( pReactorBase );
		}
#ifdef ME_WIN
		else if ( ME_BIT_ENABLED(pReactor->GetType(), CMEReactor::REACTOR_TYPE_ASYCH_IOCP) )
		{
			CMEReactorAsyncIOCP* pReactorIOCP = dynamic_cast<CMEReactorAsyncIOCP*>( pReactor->m_pReactorImpl );
			ME_ASSERTE_RETURN( (NULL != pReactorIOCP), ME_ERROR_INVALID_ARG );
		}

#else	// ME_WIN
		else if ( ME_BIT_ENABLED(pReactor->GetType(), CMEReactor::REACTOR_TYPE_ASYCH_EPOLL) )
		{
			CMEReactorAsyncEpoll* pReactorEpoll = dynamic_cast<CMEReactorAsyncEpoll*>( pReactor->m_pReactorImpl );
			ME_ASSERTE_RETURN( (NULL != pReactorEpoll), ME_ERROR_INVALID_ARG );
			pWakener = pReactorEpoll->m_pWakener;
		}

#endif	// ME_WIN

        ME_DELETE( pReactor );
        ME_DELETE( pWakener );
    }

    return ME_OK;
}



CMEReactor* CMEReactorFactory::Create( 
	INT iType, 
	DWORD dwThreadNum )
{
	if ( CMEReactor::REACTOR_TYPE_ASYCH_IOCP == iType )
	{
#ifdef ME_WIN
		return new CMEReactor( CMEReactorAsyncIOCP::Instance() );

#endif	// ME_WIN
	}
	else if ( CMEReactor::REACTOR_TYPE_ASYCH_EPOLL == iType )
	{
#ifdef ME_LINUX
		return new CMEReactor( CMEReactorAsyncEpoll::Instance() );

#endif	// ME_LINUX
	}

	return NULL;
}

ME_NAME_SPACE_END

