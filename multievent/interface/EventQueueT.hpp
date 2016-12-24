#ifdef ME_LINUX
ME_USING_HIK
#endif	// ME_LINUX

template <class EventType, class LockType>
CEventQueueT<EventType, LockType>::CEventQueueT()
	: m_bExit( FALSE )
{

}

template <class EventType, class LockType>
CEventQueueT<EventType, LockType>::~CEventQueueT()
{
	if ( FALSE == m_nodeList.IsEmpty() )
	{
		UINT nSize = m_nodeList.Size();
		for ( ; nSize > 0; --nSize )
		{
			NodeType* pNode = m_nodeList.EraseFirstItem();
			ME_DELETE( pNode );
		}
	}
}

template <class EventType, class LockType>
ME_Result CEventQueueT<EventType, LockType>::PostEvent( EventType* pEvent )
{
	return PostEvent(
		pEvent,
		NodeType::MODE_POST );
}

template <class EventType, class LockType>
ME_Result CEventQueueT<EventType, LockType>::PostEvent( 
	EventType* pEvent, 
	typename NodeType::MODE nMode, 
	SyncEvent* pSyncEvent /* = NULL */ )
{
	ME_ASSERTE_RETURN( pEvent, ME_ERROR_NULL_POINTER );

	CLockGuardT<LockType> Guard( &m_Lock );

	if ( TRUE == m_bExit )
	{
		return ME_ERROR_NOT_AVAILABLE;
	}

	NodeType* pNode = NULL;
	ME_Result hResult = ME_ERROR_FAILURE;


	pNode = new NodeType(
		pEvent,
		nMode,
		pSyncEvent );

	hResult = m_nodeList.Add( pNode );

	return hResult;
}

template <class EventType, class LockType>
ME_Result CEventQueueT<EventType, LockType>::HandleEvent( BOOL& bExit )
{
	ListType tmpList;
	UINT iSize = 0;
	{
		CLockGuardT<LockType> Guard( &m_Lock );

		if ( TRUE == m_bExit )
		{
			bExit = m_bExit;

			return ME_ERROR_NOT_AVAILABLE;
		}

		if ( TRUE == IsEmpty() )
		{
			return ME_OK;
		}
		
		iSize = m_nodeList.GetActiveNodes( tmpList );
	}

	NodeType* pNode = NULL;
	for ( UINT i = 0; i < iSize; ++i )
	{
		pNode = tmpList.GetFirstItem();
		ME_ASSERTE_RETURN( pNode, ME_OK );

		tmpList.Remove( pNode );
		pNode->Active();

		ME_DELETE( pNode );
	}

#ifdef ME_DEBUG
	ME_ASSERTE_RETURN( (TRUE == tmpList.IsEmpty()), ME_ERROR_FAILURE );

#endif	// ME_DEBUG

	return ME_OK;
}

template <class EventType, class LockType>
BOOL CEventQueueT<EventType, LockType>::IsEmpty()
{
	CLockGuardT<LockType> Guard( &m_Lock );

	return m_nodeList.IsEmpty();
}

template <class EventType, class LockType>
void CEventQueueT<EventType, LockType>::Destroy()
{
	CLockGuardT<LockType> Guard( &m_Lock );

	m_bExit = TRUE;
}

template <class EventType, class LockType>
void CEventQueueT<EventType, LockType>::operator = ( const CEventQueueT<EventType, LockType>& Dst )
{
	m_nodeList = Dst.m_nodeList;
	m_bExit = Dst.m_bExit;
}

