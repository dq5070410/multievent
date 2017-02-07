
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
		* ������ü����Ѿ���0�����������ɾ������Ȼ�ᵼ������ɾ�������crash
		*
		* ע�⣺��������̳���IMEReferenceControlT������CMEReferenceControlT
		* ͬʱû����дRemoveReference()������new����ʵ��֮��û��AddReference()��
		* ����ʹ��RemoveReference()����ʵ��ɾ�������ʱ��ʵ���ǲ���ɾ���ģ���Ϊ
		* ���ʵ�������ü�����ͷ��β����0
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





