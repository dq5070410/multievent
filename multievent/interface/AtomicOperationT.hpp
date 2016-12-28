#ifdef ME_LINUX
ME_USING_HIK
#endif	// ME_LINUX

template <class LockType>
CMEAtomicOperationT<LockType>::CMEAtomicOperationT( long lValue /* = 0 */ )
	: m_lValue( lValue )
{

}

template <class LockType>
ME_INLINE long CMEAtomicOperationT<LockType>::operator ++ ()
{
	CMELockGuardT<LockType> Guard( &m_Mutex );
	return ++m_lValue;
}

template <class LockType>
ME_INLINE long CMEAtomicOperationT<LockType>::operator ++ ( int )
{
	return ++*this - 1;
}

template <class LockType>
ME_INLINE long CMEAtomicOperationT<LockType>::operator += ( long lSourceValue )
{
	CMELockGuardT<LockType> Guard( &m_Mutex );
	*this + lSourceValue;

	return *this;
}

template <class LockType>
ME_INLINE long CMEAtomicOperationT<LockType>::operator -- ()
{
	CMELockGuardT<LockType> Guard( &m_Mutex );
	return --m_lValue;
}

template <class LockType>
ME_INLINE long CMEAtomicOperationT<LockType>::operator -- ( int )
{
	return --*this + 1;
}

template <class LockType>
ME_INLINE long CMEAtomicOperationT<LockType>::operator -= ( long lSourceValue )
{
	CMELockGuardT<LockType> Guard( &m_Mutex );
	*this - lSourceValue;

	return *this;
}

template <class LockType>
ME_INLINE int CMEAtomicOperationT<LockType>::operator == ( long lSourceValue ) const
{
	return ( m_lValue == lSourceValue );
}

template <class LockType>
ME_INLINE int CMEAtomicOperationT<LockType>::operator != ( long lSourceValue ) const
{
	return ( m_lValue != lSourceValue );
}

template <class LockType>
ME_INLINE int CMEAtomicOperationT<LockType>::operator >= ( long lSourceValue ) const
{
	return ( m_lValue >= lSourceValue );
}

template <class LockType>
ME_INLINE int CMEAtomicOperationT<LockType>::operator > ( long lSourceValue ) const
{
	return ( m_lValue > lSourceValue );
}

template <class LockType>
ME_INLINE int CMEAtomicOperationT<LockType>::operator <= ( long lSourceValue ) const
{
	return ( m_lValue <= lSourceValue );
}

template <class LockType>
ME_INLINE int CMEAtomicOperationT<LockType>::operator < ( long lSourceValue ) const
{
	return ( m_lValue < lSourceValue );
}

template <class LockType>
ME_INLINE void CMEAtomicOperationT<LockType>::operator = ( long lSourceValue )
{
	CMELockGuardT<LockType> Guard( &m_Mutex );
	m_lValue = lSourceValue;
}

template <class LockType>
ME_INLINE void CMEAtomicOperationT<LockType>::operator = ( const CMEAtomicOperationT<LockType>& haoSourceValue )
{
	CMELockGuardT<LockType> Guard( &m_Mutex );
	m_lValue = haoSourceValue.m_lValue;
}

template <class LockType>
ME_INLINE long CMEAtomicOperationT<LockType>::GetValue() const
{
	return m_lValue;
}

//CMEAtomicOperationT<CMELockMutex>::CMEAtomicOperationT( long lValue /* = 0 */ )
//	: m_lValue( lValue )
//{
//
//}

ME_INLINE long CMEAtomicOperationT<CMELockMutex>::operator ++ ()
{
#ifdef ME_WIN
	return ::InterlockedIncrement( const_cast<long*>(&m_lValue) );
#else
	return ::__sync_add_and_fetch(
		&m_lValue,
		1 );
#endif // ME_WIN
}

ME_INLINE long CMEAtomicOperationT<CMELockMutex>::operator ++ ( int )
{
	return ++*this - 1;
}

ME_INLINE long CMEAtomicOperationT<CMELockMutex>::operator += ( long lSourceValue )
{
#ifdef ME_WIN
	return ( ::InterlockedExchangeAdd(
		const_cast<long*>(&m_lValue), 
		lSourceValue ) + lSourceValue );
#else
	return ::__sync_add_and_fetch(
		&m_lValue,
		lSourceValue );
#endif // ME_WIN
}

ME_INLINE long CMEAtomicOperationT<CMELockMutex>::operator -= ( long lSourceValue )
{
#ifdef ME_WIN
	return ( ::InterlockedExchangeAdd(
		const_cast<long*>(&m_lValue), 
		-lSourceValue ) - lSourceValue );
#else
	return ::__sync_sub_and_fetch(
		&m_lValue, 
		lSourceValue );
#endif // ME_WIN
}

ME_INLINE long CMEAtomicOperationT<CMELockMutex>::operator -- ()
{
#ifdef ME_WIN
	return ::InterlockedDecrement( const_cast<long*>(&m_lValue) );
#else
	return ::__sync_sub_and_fetch(
		&m_lValue,
		1 );
#endif	// ME_WIN
}

ME_INLINE long CMEAtomicOperationT<CMELockMutex>::operator -- ( int )
{
	return --*this + 1;
}

ME_INLINE int CMEAtomicOperationT<CMELockMutex>::operator == ( long lSourceValue ) const
{
	return ( m_lValue == lSourceValue );
}

ME_INLINE int CMEAtomicOperationT<CMELockMutex>::operator != ( long lSourceValue ) const
{
	return ( m_lValue != lSourceValue );
}

ME_INLINE int CMEAtomicOperationT<CMELockMutex>::operator >= ( long lSourceValue ) const
{
	return ( m_lValue >= lSourceValue );
}

ME_INLINE int CMEAtomicOperationT<CMELockMutex>::operator > ( long lSourceValue ) const
{
	return ( m_lValue > lSourceValue );
}

ME_INLINE int CMEAtomicOperationT<CMELockMutex>::operator <= ( long lSourceValue ) const
{
	return ( m_lValue <= lSourceValue );
}

ME_INLINE int CMEAtomicOperationT<CMELockMutex>::operator < ( long lSourceValue ) const
{
	return ( m_lValue < lSourceValue );
}

ME_INLINE void CMEAtomicOperationT<CMELockMutex>::operator = ( long lSourceValue )
{
#ifdef ME_WIN
	::InterlockedExchange(
		const_cast<long*>(&m_lValue),
		lSourceValue );
#else
	::__sync_lock_test_and_set(
		&m_lValue,
		lSourceValue );
#endif	// ME_WIN
}

ME_INLINE void CMEAtomicOperationT<CMELockMutex>::operator = ( const CMEAtomicOperationT<CMELockMutex>& haoSourceValue )
{
#ifdef ME_WIN
	::InterlockedExchange(
		const_cast<long*>(&m_lValue),
		haoSourceValue.m_lValue );
#else
	::__sync_lock_test_and_set(
		&m_lValue,
		haoSourceValue.m_lValue );
#endif	// ME_WIN
}

ME_INLINE long CMEAtomicOperationT<CMELockMutex>::GetValue() const
{
	return m_lValue;
}





