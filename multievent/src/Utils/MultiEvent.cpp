#include "MultiEvent.h"
#include "AtomicOperationT.h"
#include "LockMutex.h"
#include "ConnectionManager.h"
#include "CleanupBase.h"
#include "ConfigManager.h"
#include "ThreadManager.h"

ME_NAME_SPACE_BEGIN

static CMEAtomicOperationT<CMELockMutex> g_Ref;

ME_Result Initialize(
	IMEThreadSink* pUserThreadSink /* = NULL */,
	BOOL bUserThreadWorkInBlockMode /* = FALSE */ )
{
	if ( 1 < ++g_Ref )
	{
		return ME_OK;
	}

	CMEConnectionManager::SocketStartup();

	CConfigManagerSingleton::Instance()->Initialize(
		pUserThreadSink,
		bUserThreadWorkInBlockMode );

	return ME_OK;
}

ME_Result UnInitialize()
{
    INT iRef = --g_Ref;
	if ( 0 == iRef )
	{
		CConfigManagerSingleton::Instance()->UnInitialize();
		CMEConnectionManager::SocketCleanup();

#ifndef ME_HAS_MFC
		::atexit( CMECleanupBase::CleanupAll );	// 在程序不使用MFC的情况下

#else
		CMECleanupBase::CleanupAll();				// 用了MFC，atexit被其占用，贸然重新注册可能会导致不可预知的后果

#endif	// ME_HAS_MFC
	}
    else if ( 0 > iRef )
    {
        ++g_Ref;
    }
	
	return ME_OK;
}

void Run()
{
	if ( 0 < g_Ref.GetValue() )
	{
		IMEThread* pUserThread = CMEThreadManagerSingleton::Instance()->GetUserThread();

		/* 实际这里需要asserte用户线程的blocktype == TRUE，但是目前这个接口没有提供 */
		pUserThread->Run();
	}
}

ME_NAME_SPACE_END







