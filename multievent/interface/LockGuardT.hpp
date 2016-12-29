
template<class LockType>
CMELockGuardT<LockType>::CMELockGuardT( LockType* pLock )
	: m_pLock( pLock )
{
	Lock();
}

template<class LockType>
CMELockGuardT<LockType>::~CMELockGuardT()
{
	UnLock();
}

template<class LockType>
ME_INLINE ME_Result CMELockGuardT<LockType>::Lock()
{
	return m_pLock->Lock();
}

template<class LockType>
ME_INLINE ME_Result CMELockGuardT<LockType>::UnLock()
{
	return m_pLock->UnLock();
}








