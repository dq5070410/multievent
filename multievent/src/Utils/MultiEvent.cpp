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
		::atexit( CMECleanupBase::CleanupAll );	// �ڳ���ʹ��MFC�������

#else
		CMECleanupBase::CleanupAll();				// ����MFC��atexit����ռ�ã�óȻ����ע����ܻᵼ�²���Ԥ֪�ĺ��

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

		/* ʵ��������Ҫasserte�û��̵߳�blocktype == TRUE������Ŀǰ����ӿ�û���ṩ */
		pUserThread->Run();
	}
}

ME_NAME_SPACE_END







