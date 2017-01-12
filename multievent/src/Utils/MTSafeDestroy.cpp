#include "MTSafeDestroy.h"
#include "Thread.h"
#include "ThreadManager.h"
#include "OSThread.h"

ME_NAME_SPACE_BEGIN

CMEMTSafeDestroy::CMEMTSafeDestroy()
	: m_dwStatus( DESTROY_STATUS_CALL_IN_CURRENT_THREAD )
{

}

CMEMTSafeDestroy::~CMEMTSafeDestroy()
{
	/* 
	* 其实这个赋值基本上没用，不过，为求心安吧 
	*
	* 其实也就当一个magic number来用
	*/
	m_dwStatus = DESTROY_STATUS_NONE;
}

void CMEMTSafeDestroy::Destroy_u()
{
	/* 如果当前对象是第一次call<Destroy_u> */
	if (  DESTROY_STATUS_CALL_IN_CURRENT_THREAD == m_dwStatus )
	{	
		CMEThread* pThread = (CMEThread*)CMEThreadManager::Instance()->GetNetworkThread();
		ME_ASSERTE_RETURN_VOID( pThread );

		pThread->PostEvent( this );
	}
}

void CMEMTSafeDestroy::Destroy_n()
{
	ME_ASSERTE( DESTROY_STATUS_CALL_IN_NETWORK_THREAD == m_dwStatus );

	CMEThread* pThread = (CMEThread*)CMEThreadManager::Instance()->GetUserThread();
	ME_ASSERTE_RETURN_VOID( pThread );

	pThread->PostEvent( this );
}

ME_Result CMEMTSafeDestroy::OnEvent()
{
	/* 当前是网络线程，开始做清理工作 */
	if ( DESTROY_STATUS_CALL_IN_NETWORK_THREAD == ++m_dwStatus )
	{
		Destroy_n();
	}

	/** 
	* 如果是网络线程清理工作已经完成, 什么都不要做
	* 等待OnDestroy的发生, 在那里, 进行真正的删除
	*/

	return ME_OK;
}

ME_Result CMEMTSafeDestroy::OnDestroy()
{
	CMEThread* pNetworkThread = (CMEThread*)CMEThreadManager::Instance()->GetNetworkThread();
	CMEThread* pUserThread = (CMEThread*)CMEThreadManager::Instance()->GetUserThread();
	ME_ASSERTE_RETURN( pNetworkThread && pUserThread, ME_ERROR_FAILURE );

	/* 如果已经进入过网络线程清理，且当前是出于用户线程 */
	if ( DESTROY_STATUS_BACK_TO_CURRENT_THREAD == m_dwStatus &&
		 pUserThread->GetThreadOSID() == OS::GetThreadID() )
	{
		/**
		* 由于是来回在两个线程里投递事件，无法得知事件处理的先后顺序(先后顺序在这里很重要)
		* 然而又必须要网络线程先处理好，用户线程才能继续处理，所以这里就必须阻塞等待，
		* 不然肯定会在某些情况下crash
		*/
		m_Event.Wait();
		
		delete this;	// 真正的删除
	}

	/* 网络线程已经处理完毕，通知用户线程以便上边的那个阻塞能够解除 */
	if ( pNetworkThread->GetThreadOSID() == OS::GetThreadID() )
	{
		m_Event.Signal();
	}

	return ME_OK;
}

ME_NAME_SPACE_END



