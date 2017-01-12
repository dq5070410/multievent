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
	* ��ʵ�����ֵ������û�ã�������Ϊ���İ��� 
	*
	* ��ʵҲ�͵�һ��magic number����
	*/
	m_dwStatus = DESTROY_STATUS_NONE;
}

void CMEMTSafeDestroy::Destroy_u()
{
	/* �����ǰ�����ǵ�һ��call<Destroy_u> */
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
	/* ��ǰ�������̣߳���ʼ�������� */
	if ( DESTROY_STATUS_CALL_IN_NETWORK_THREAD == ++m_dwStatus )
	{
		Destroy_n();
	}

	/** 
	* ����������߳��������Ѿ����, ʲô����Ҫ��
	* �ȴ�OnDestroy�ķ���, ������, ����������ɾ��
	*/

	return ME_OK;
}

ME_Result CMEMTSafeDestroy::OnDestroy()
{
	CMEThread* pNetworkThread = (CMEThread*)CMEThreadManager::Instance()->GetNetworkThread();
	CMEThread* pUserThread = (CMEThread*)CMEThreadManager::Instance()->GetUserThread();
	ME_ASSERTE_RETURN( pNetworkThread && pUserThread, ME_ERROR_FAILURE );

	/* ����Ѿ�����������߳������ҵ�ǰ�ǳ����û��߳� */
	if ( DESTROY_STATUS_BACK_TO_CURRENT_THREAD == m_dwStatus &&
		 pUserThread->GetThreadOSID() == OS::GetThreadID() )
	{
		/**
		* �����������������߳���Ͷ���¼����޷���֪�¼�������Ⱥ�˳��(�Ⱥ�˳�����������Ҫ)
		* Ȼ���ֱ���Ҫ�����߳��ȴ���ã��û��̲߳��ܼ���������������ͱ��������ȴ���
		* ��Ȼ�϶�����ĳЩ�����crash
		*/
		m_Event.Wait();
		
		delete this;	// ������ɾ��
	}

	/* �����߳��Ѿ�������ϣ�֪ͨ�û��߳��Ա��ϱߵ��Ǹ������ܹ���� */
	if ( pNetworkThread->GetThreadOSID() == OS::GetThreadID() )
	{
		m_Event.Signal();
	}

	return ME_OK;
}

ME_NAME_SPACE_END



