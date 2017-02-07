
template <class LockType>
ME_INLINE DWORD CMEReferenceControlT<LockType>::AddReference()
{
	return ++m_Atomic;
}

template <class LockType>
ME_INLINE DWORD CMEReferenceControlT<LockType>::RemoveReference()
{
	if ( m_Atomic == 0 )
	{
		/* 
		* 如果引用计数已经是0，就无需进行删除，不然会导致两次删除引起的crash
		*
		* 注意：如果你的类继承于IMEReferenceControlT，或者CMEReferenceControlT
		* 同时没有重写RemoveReference()，而且new出了实例之后并没有AddReference()，
		* 又想使用RemoveReference()进行实例删除，这个时候，实例是不会删除的，因为
		* 这个实例的引用计数从头到尾都是0
		*/
		//OnReferenceRelease();

		return 0;
	}

	DWORD dwRef = --m_Atomic;
	if ( 0 == dwRef )
	{
		OnReferenceRelease();
	}

	return dwRef;
}

template <class LockType>	
ME_INLINE DWORD CMEReferenceControlT<LockType>::GetReference()
{
	return m_Atomic.GetValue();
}

template <class LockType>
void CMEReferenceControlT<LockType>::OnReferenceRelease()
{
	delete this;
}





