#include "LockConditionVariable.h"
#include "OS.h"

ME_NAME_SPACE_BEGIN

CMELockConditionVariable::CMELockConditionVariable( 
	const char* pszName /* = NULL */, 
	BOOL bActAsEvent /* = FALSE */ )
	: m_bActAsEvent( bActAsEvent )
{
	OS::ConditionVariableCreate( 
		&m_ConditionVariable, 
		pszName );

	OS::ThreadMutexCreate( m_Mutex );
}

CMELockConditionVariable::~CMELockConditionVariable()
{
	OS::ConditionVariableDestroy( &m_ConditionVariable );
	OS::ThreadMutexDestroy( m_Mutex );
}

ME_Result CMELockConditionVariable::Lock()
{
	return OS::ThreadMutexLock( m_Mutex );
}

ME_Result CMELockConditionVariable::UnLock()
{
	return OS::ThreadMutexUnLock( m_Mutex );
}

ME_Result CMELockConditionVariable::Signal()
{
	if ( TRUE == m_bActAsEvent )
	{
		m_Signal++;
	}
	
	return OS::ConditionVariableSignal( &m_ConditionVariable );
}

ME_Result CMELockConditionVariable::Broadcast()
{
	if ( TRUE == m_bActAsEvent )
	{
		m_Signal++;
	}

	return OS::ConditionVariableBroadcast( &m_ConditionVariable );
}

ME_Result CMELockConditionVariable::Wait( CMETimeValue* pTimeout /* = NULL */ )
{
	if ( FALSE == m_bActAsEvent )
	{
		return OS::ConditionVariableWait( 
			&m_ConditionVariable,
			&m_Mutex,
			pTimeout ); 
	}

	/** 
	* ���<m_Signal>Ϊ0, ˵������wait֮ǰ, û��Signal()����Broadcast()��
	* ��, ����wait�ǿ��Բ���Signal()��Broadcast()�������ź�
	*/
	if ( m_Signal == 0 )
	{
		return OS::ConditionVariableWait( 
			&m_ConditionVariable,
			&m_Mutex,
			pTimeout );
	}
	/** 
	* ���<m_Signal>����0, ˵����wait֮ǰ, �Ѿ�Signal()����Broadcast()
	* ����ֱ��ִ��wait֮��Ĵ���
	* ����˴���Ȼǿ�е�wait, ��ô���п��ܵ���wait����
	*/
	else
	{
		m_Signal = 0;	// ֱ����Ϊ0��Ϊ, ����Signal()��Broadcast()���ٴ�, waitֻ�ܲ�һ��
	}

	return ME_OK;
}

ME_NAME_SPACE_END



