
template <class NodeType, class SorterType, DWORD DELIVERY_MAX_NUM>
CMENodeListT<NodeType, SorterType, DELIVERY_MAX_NUM>::CMENodeListT()
{

}

template <class NodeType, class SorterType, DWORD DELIVERY_MAX_NUM>
CMENodeListT<NodeType, SorterType, DELIVERY_MAX_NUM>::~CMENodeListT()
{
	ME_ASSERTE( TRUE == IsEmpty() );
}

template <class NodeType, class SorterType, DWORD DELIVERY_MAX_NUM>
ME_Result CMENodeListT<NodeType, SorterType, DELIVERY_MAX_NUM>::Add( NodeType* pNode )
{
	m_List.push_back( pNode );

	return ME_OK;
}

template <class NodeType, class SorterType, DWORD DELIVERY_MAX_NUM>
ME_Result CMENodeListT<NodeType, SorterType, DELIVERY_MAX_NUM>::Remove( NodeType* pNode )
{
	if ( TRUE == IsEmpty() )
	{
		return ME_OK;
	}

	m_List.remove( pNode );

	return ME_OK;
}

template <class NodeType, class SorterType, DWORD DELIVERY_MAX_NUM>
NodeType* CMENodeListT<NodeType, SorterType, DELIVERY_MAX_NUM>::GetFirstItem()
{
	if ( TRUE == IsEmpty() )
	{
		return NULL;
	}

	return *m_List.begin();
}

template <class NodeType, class SorterType, DWORD DELIVERY_MAX_NUM>
NodeType* CMENodeListT<NodeType, SorterType, DELIVERY_MAX_NUM>::EraseFirstItem()
{
	NodeType* pNode = GetFirstItem();
	Remove( pNode );

	return pNode;
}

template <class NodeType, class SorterType, DWORD DELIVERY_MAX_NUM>
UINT CMENodeListT<NodeType, SorterType, DELIVERY_MAX_NUM>::GetActiveNodes( CMENodeListT<NodeType, SorterType, DELIVERY_MAX_NUM>& activeList )
{
	int iActualSize = Size();	// linux下是逐个数过来，效率太低，不利于多次调用
	iActualSize = iActualSize > DELIVERY_MAX_NUM ? DELIVERY_MAX_NUM : iActualSize;

	typename ListType::iterator iter = m_List.begin();
	for ( int i = 0; i < iActualSize; ++i, ++iter );

	activeList.m_List.assign(
		m_List.begin(),
		iter );

	m_List.erase( 
		m_List.begin(),
		iter );

	return iActualSize;
}

template <class NodeType, class SorterType, DWORD DELIVERY_MAX_NUM>
UINT CMENodeListT<NodeType, SorterType, DELIVERY_MAX_NUM>::Size() const
{
	return m_List.size();
}

template <class NodeType, class SorterType, DWORD DELIVERY_MAX_NUM>
BOOL CMENodeListT<NodeType, SorterType, DELIVERY_MAX_NUM>::IsEmpty() const
{
	return (BOOL)m_List.empty();
}

template <class NodeType, class SorterType, DWORD DELIVERY_MAX_NUM>
void CMENodeListT<NodeType, SorterType, DELIVERY_MAX_NUM>::Sort()
{
	/* 不是特别的确定vc6版本的stl是否支持这个特性 */
	SorterType Sorter;

	m_List.sort( Sorter );
}

template <class NodeType, class SorterType, DWORD DELIVERY_MAX_NUM>
NodeType* CMENodeListT<NodeType, SorterType, DELIVERY_MAX_NUM>::Find( const NodeType* pNode )
{
	if ( TRUE == m_List.empty() )
	{
		return NULL;
	}

	typename ListType::iterator iter = std::find( 
		m_List.begin(),
		m_List.end(),
		pNode );

	return (m_List.end() == iter ? NULL : *iter);
}

template <class NodeType, class SorterType, DWORD DELIVERY_MAX_NUM>
NodeType* CMENodeListT<NodeType, SorterType, DELIVERY_MAX_NUM>::Find( const NodeSuperType* pNodeSuperType )
{
	if ( TRUE == m_List.empty() )
	{
		return NULL;
	}

	/*NodeType targetNode( const_cast<NodeSuperType*>(pNodeSuperType) );
	CMENodeCompareT<NodeType*> compareFunctor;

	ListType::iterator iter = std::lower_bound(
		m_List.begin(),
		m_List.end(),
		&targetNode,
		compareFunctor );*/

	/* 先用一个笨办法解决这个问题，后边再优化 */
	typename ListType::iterator iter = m_List.begin();
	for ( ; iter != m_List.end(); ++iter )
	{
		if ( **iter == *pNodeSuperType )
		{
			break;
		}
	}

	return (m_List.end() == iter ? NULL : *iter);
}

//template <class NodeType, class SorterType, DWORD DELIVERY_MAX_NUM>
//BOOL CMENodeListT<NodeType, SorterType, DELIVERY_MAX_NUM>::CMENodeCompareT::operator () ( const NodeType*& pSrcNode, const NodeType*& pDstNode )
//{
//	return !(pSrcNode == pDstNode);
//}

template<class ListNodeType>
BOOL CMENodeListDefaultSort<ListNodeType>::operator () ( const ListNodeType& lntSrc, const ListNodeType& lntDst )
{
	return FALSE;
}

 



















