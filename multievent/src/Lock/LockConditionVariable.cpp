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
	* 如果<m_Signal>为0, 说明进入wait之前, 没有Signal()或者Broadcast()过
	* 即, 本次wait是可以捕获Signal()或Broadcast()发出的信号
	*/
	if ( m_Signal == 0 )
	{
		return OS::ConditionVariableWait( 
			&m_ConditionVariable,
			&m_Mutex,
			pTimeout );
	}
	/** 
	* 如果<m_Signal>大于0, 说明在wait之前, 已经Signal()或者Broadcast()
	* 可以直接执行wait之后的代码
	* 如果此处仍然强行的wait, 那么很有可能导致wait死锁
	*/
	else
	{
		m_Signal = 0;	// 直接置为0因为, 无论Signal()或Broadcast()多少次, wait只能捕一次
	}

	return ME_OK;
}

ME_NAME_SPACE_END



