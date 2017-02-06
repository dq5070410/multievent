
template <class EventType>
CEventNodeT<EventType>::CEventNodeT()
	: m_pEvent( NULL )
	, m_nEventMode( MODE_POST )
	, m_pSync( NULL )
	, m_nPriority( PRIORITY_NORMAL )
{

}

template <class EventType>
CEventNodeT<EventType>::CEventNodeT( SuperType* pSuper )
	: m_pEvent( pSuper )
	, m_nEventMode( MODE_POST )
	, m_pSync( NULL )
	, m_nPriority( PRIORITY_NORMAL )
{

}



template <class EventType>
CEventNodeT<EventType>::CEventNodeT( 
	EventType* pEvent, 
	MODE nMode, 
	CMESyncEvent* pSync, 
	PRIORITY nPriority /* = PRIORITY_NORMAL */ )
	: m_pEvent( pEvent )
	, m_nEventMode( nMode )
	, m_pSync( pSync )
	, m_nPriority( nPriority )
{

}

template <class EventType>
ME_Result CEventNodeT<EventType>::Set( 
	EventType* pEvent, 
	MODE nMode )
{
	ME_ASSERTE_RETURN( pEvent, ME_ERROR_NULL_POINTER );

	m_pEvent = pEvent;
	m_nEventMode = nMode;

	return ME_OK;
}

template <class EventType>
ME_Result CEventNodeT<EventType>::SetSyncEvent( CMESyncEvent* pSync )
{
	ME_ASSERTE_RETURN( pSync, ME_ERROR_NULL_POINTER );

	m_pSync = pSync;

	return ME_OK;
}

template <class EventType>
void CMEEventNodeT<EventType>::SetPriority( PRIORITY nPriority )
{
	m_nPriority = nPriority;
}

template <class EventType>
UINT CEventNodeT<EventType>::GetPriority() const
{
	return m_nPriority;
}

template <class EventType>
void CEventNodeT<EventType>::CMESyncEvent::Signal( ME_Result hResult /* = ME_OK */ )
{
	m_hResult = hResult;

	m_SyncEvent.Signal();
}

template <class EventType>
ME_Result CEventNodeT<EventType>::CMESyncEvent::GetResult()
{
	m_SyncEvent.Wait();

	return m_hResult;
}

template <class EventType>
ME_Result CEventNodeT<EventType>::Active()
{
	ME_ASSERTE_RETURN( m_pEvent, ME_ERROR_NULL_POINTER );

	ME_Result hResult = m_pEvent->OnEvent();
	m_pEvent->OnDestroy();
	m_pEvent = NULL;

	if ( MODE_SEND == m_nEventMode )
	{
		m_pSync->Signal( hResult );
	}

	return ME_OK;
}

template <class EventType>
BOOL CEventNodeT<EventType>::operator == ( const EventType* pEvent )
{
	return (m_pEvent == pEvent);
}
