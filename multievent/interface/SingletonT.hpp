#ifdef ME_LINUX
ME_USING_HIK
#endif	// ME_LINUX

template <class ObjectType>
ObjectType* CMESingletonT<ObjectType>::Instance()
{
	if ( NULL == s_pInstance )
	{
		/* 这里要拿到一个全局锁 */
		MutexType* pMutex = GetCleanupMutex();
		if ( pMutex )
		{
			CMELockGuardT<MutexType> Guard( pMutex );
			if ( NULL == s_pInstance )
			{
				s_pInstance = new CMESingletonT<ObjectType>;
			}
		}
		else
		{
			return NULL;
		}
	}

	return &s_pInstance->m_Intance;
}

template <class ObjectType>
ObjectType* CMESingletonT<ObjectType>::Instance( ObjectType* pObj )
{
	if ( NULL == s_pInstance )
	{
		/* 这里要拿到一个全局锁 */
		MutexType* pMutex = GetCleanupMutex();
		if ( pMutex )
		{
			CMELockGuardT<MutexType> Guard( pMutex );
			if ( NULL == s_pInstance )
			{
				s_pInstance = new CMESingletonT<ObjectType>;
				s_pInstance->m_Intance = *pObj;
			}
		}
		else
		{
			return NULL;
		}
	}

	return &s_pInstance->m_Intance;
}

template <class ObjectType>
CMESingletonT<ObjectType>::CMESingletonT()
{

}

template <class ObjectType>
CMESingletonT<ObjectType>::~CMESingletonT()
{ 
	s_pInstance = NULL;
}







