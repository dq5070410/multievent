#include "ConfigManager.h"
#include "ThreadInterface.h"
#include "ThreadManager.h"
#include "SingletonT.h"
#include "ReactorAsyncIOCP.h"
#include "ReactorAsyncEpoll.h"
#include "WakenerAsyncIOCP.h"
#include "WakenerSocketPair.h"
#include "OSProcess.h"
#include "ReactorFactory.h"
#include "Reactor.h"
#include "Thread.h"

/* SSL moudle initialize and finalize */
#include "SSLContext.h"

ME_NAME_SPACE_BEGIN

ME_Result CMEConfigManager::Initialize( 
	IMEThreadSink* pUserThreadSink /* = NULL */, 
	BOOL bUserThreadWorkInBlockMode /* = FALSE */ )
{
    /* 初始化SSL模块，暂时都传NULL，以后配置模块应该按照配置传入默认证书/私钥路
       径等参数 */
    CRYPTO::CMESSLContextSingleton::Instance()->Initialize(
        NULL,
        NULL,
        NULL );

	/* 初始化线程管理器，并创建用户线程 */
	ME_Result hResult = CMEThreadManagerSingleton::Instance()->Initialize(
		pUserThreadSink,
		bUserThreadWorkInBlockMode );

	ME_ASSERTE( ME_SUCCEEDED(hResult) );

	/**
	* 不完整，但先这么写
	* 
	* 更合适的做法是根据配置文件来决定是否使用异步IO的IO捕捉方式
	*/
	DWORD dwThreadNum = OS::GetCPUCount();

#ifdef ME_WIN
	ME_HANDLE hInterface = ::CreateIoCompletionPort( 
		ME_INVALID_HANDLE,
		NULL,
		NULL,
		0 );

	if ( (NULL == hInterface) || (ME_INVALID_HANDLE == hInterface) )
	{
		::CloseHandle( hInterface );

		return ME_ERROR_FAILURE;
	}

	CMEWakenerAsyncIOCP* pWakener = new CMEWakenerAsyncIOCP( hInterface );
	pWakener->Initialize();

// 	CMEReactorAsyncIOCP* pReactorIOCP = new CMEReactorAsyncIOCP( 
// 		pWakener, 
// 		hInterface, 
// 		dwThreadNum );
// 
// 	CMESingletonT<CMEReactorAsyncIOCP>::Instance( pReactorIOCP );

	/* 复位异步ReactorImpl状态，使可Initialize-UnInitialize重复调用 */
	CMEReactorAsyncIOCP::Instance()->Initialize( 
		hInterface, 
		pWakener, 
		dwThreadNum );

//	ME_DELETE( pReactorIOCP );

#elif defined( ME_LINUX )
	IMEWakener* pWakener = new CMEWakenerSocketPair();
	hResult = pWakener->Initialize();

	ME_HANDLE fdEpoll = ::epoll_create( 1 );
	CMEReactorAsyncEpoll* pReactorEpoll = new CMEReactorAsyncEpoll(
		pWakener,
		fdEpoll,
		dwThreadNum );

	CMESingletonT<CMEReactorAsyncEpoll>::Instance( pReactorEpoll );

	/* 复位异步ReactorImpl状态，使可Initialize-UnInitialize重复调用 */
	CMEReactorAsyncEpoll::Instance()->Initialize();

	ME_DELETE( pReactorEpoll );

#endif	// ME_WIN

	/* 创建网络线程 */
	CMEThreadManagerSingleton::Instance()->CreateThread( dwThreadNum );
	ME_ASSERTE( ME_SUCCEEDED(hResult) );

	return ME_OK;
}

ME_Result CMEConfigManager::UnInitialize()
{

// 	CMEThread* pNetworkThread = (CMEThread*)CMEThreadManagerSingleton::Instance()->GetNetworkThread();
// 	CMEReactor* pNetworkReactor = pNetworkThread->GetReactor();

	CMEThreadManagerSingleton::Instance()->JoinAll( ME_INFINITE );
	CMEThreadManagerSingleton::Instance()->Finalize();

	/* fixed bug# BG00A0009 */
// 	if ( pNetworkReactor && (ME_BIT_ENABLED(pNetworkReactor->GetType(), CMEReactor::REACTOR_TYPE_ASYNC)) )
// 	{
// 		CMEReactorFactorySingleton::Instance()->Destroy( pNetworkReactor );
// 	}

    CRYPTO::CMESSLContextSingleton::Instance()->Finalize();

	return ME_OK;
}

ME_NAME_SPACE_END







