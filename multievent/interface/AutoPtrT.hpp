
template <class ReferenceObject>
CMEAutoPtrT<ReferenceObject>::CMEAutoPtrT( ReferenceObject* pObj /* = NULL */ )
	: m_pObj( pObj )
{
	if ( m_pObj )
	{
		m_pObj->AddReference();
	}
}

template <class ReferenceObject>
CMEAutoPtrT<ReferenceObject>::CMEAutoPtrT( const CMEAutoPtrT& AutoPtr )
	: m_pObj( AutoPtr.m_pObj )
{
	if ( m_pObj )
	{
		m_pObj->AddReference();
	}
}

template <class ReferenceObject>
CMEAutoPtrT<ReferenceObject>::~CMEAutoPtrT()
{
	if ( m_pObj )
	{
		m_pObj->RemoveReference();
	}
}

template <class ReferenceObject>
CMEAutoPtrT<ReferenceObject>& CMEAutoPtrT<ReferenceObject>::operator = ( const CMEAutoPtrT& AutoPtr ) 
{
	return ( *this = AutoPtr.m_pObj );
}

template <class ReferenceObject>
CMEAutoPtrT<ReferenceObject>& CMEAutoPtrT<ReferenceObject>::operator = ( ReferenceObject* pReferenceObject ) 
{
	if ( m_pObj == pReferenceObject )
	{
		return *this;
	}

	if ( pReferenceObject )
	{
		pReferenceObject->AddReference();
	}

	if ( m_pObj )
	{
		m_pObj->RemoveReference();
	}

	m_pObj = pReferenceObject;

	return *this;
}

template <class ReferenceObject>
CMEAutoPtrT<ReferenceObject>::operator void* () const 
{
	return m_pObj;
}

template <class ReferenceObject>
ReferenceObject& CMEAutoPtrT<ReferenceObject>::operator * () const 
{
	return *m_pObj;
}

template <class ReferenceObject>
ReferenceObject* CMEAutoPtrT<ReferenceObject>::operator -> () const 
{
	return m_pObj;
}

template <class ReferenceObject>
ReferenceObject* CMEAutoPtrT<ReferenceObject>::Get() const 
{
	return m_pObj;
}

template <class ReferenceObject>
ReferenceObject* CMEAutoPtrT<ReferenceObject>::ParaIn() const 
{
	return m_pObj;
}

template <class ReferenceObject>
ReferenceObject*& CMEAutoPtrT<ReferenceObject>::ParaOut() 
{
	if ( m_pObj ) 
	{
		m_pObj->RemoveReference();
		m_pObj = NULL;
	}

	return static_cast<ReferenceObject*&>( m_pObj );
}

template <class ReferenceObject>
ReferenceObject*& CMEAutoPtrT<ReferenceObject>::ParaInOut() 
{
	return static_cast<ReferenceObject*&>( m_pObj );
}

template <class ReferenceObject>
bool CMEAutoPtrT<ReferenceObject>::operator < ( const CMEAutoPtrT& hapSource ) const
{
	return m_pObj < hapSource.m_pObj;
}

template <class ReferenceObject>
bool CMEAutoPtrT<ReferenceObject>::operator < ( const ReferenceObject* hapSource ) const
{
	return m_pObj < hapSource;
}

template <class ReferenceObject>
bool CMEAutoPtrT<ReferenceObject>::operator () (
	const CMEAutoPtrT& hapSource, 
	const CMEAutoPtrT& hapDest ) const
{
	return hapSource.m_pObj < hapDest.m_pObj;
}



