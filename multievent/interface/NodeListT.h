/**
* �ڵ����ģ����
* 
*
* History
*	2/05/2016		Created
*/
#ifndef __NODE_LIST_T_H_
#define __NODE_LIST_T_H_

#include "Base.h"
#include "TimerNodeT.h"

ME_NAME_SPACE_BEGIN

template<class ListNodeType> class CNodeListDefaultSort;

template <class NodeType, class SorterType = CNodeListDefaultSort<NodeType>, DWORD DELIVERY_MAX_NUM = 150>
class CMENodeListT
{
public:
	typedef typename NodeType::SuperType NodeSuperType;
	
public:
	CMENodeListT();

	~CMENodeListT();

public:
	/**
	* ����һ���ڵ�
	*
	* <pNode>������ʲô�ڵ�
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������Ϊʧ��
	*/
	ME_Result Add( NodeType* pNode );

	/**
	* ɾ��һ���ڵ�
	*
	* <pNode>����һ���ڵ�
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������Ϊʧ��
	*/
	ME_Result Remove( NodeType* pNode );

	/**
	* ��ȡ��һ���ڵ�
	*
	* ����ֵ��
	*		�ɹ�Ϊ��NULL������ΪNULL
	*/
	NodeType* GetFirstItem();

	/**
	* ɾ����һ���ڵ㲢���ظýڵ��ֵ
	*
	* ����ֵ��
	*		�ɹ�Ϊ��NULL������ΪNULL
	*/
	NodeType* EraseFirstItem();

	/**
	* ��ȡ������<DELIVERY_MAX_NUM>�ġ���ġ��ڵ�����
	*
	* <activeList>��out���ɹ�ʱ�������size()����0
	*
	* ����ֵ��
	*		���ػ�ڵ�ĸ���(0 ~ DELIVERY_MAX_NUM)
	*/
	UINT GetActiveNodes( CMENodeListT<NodeType, SorterType, DELIVERY_MAX_NUM>& activeList );

	/**
	* ��ȡ��ǰ�����size
	*
	* ����ֵ��
	*		UINT��Χ������ֵ
	*/
	UINT Size() const;

	/**
	* ��ȡ��ǰ�����Ƿ�Ϊ��
	*
	* ����ֵ��
	*		TRUE����FALSE
	*/
	BOOL IsEmpty() const;

	/**
	* ����SorterType���ͽ�������
	*
	* ��Ҫָ����SorterType����operator ()
	*
	* ����ֵ��
	*		��
	*/
	void Sort();

	/**
	* ������Ҫ�󣬲�����Ч�ʽϵ�
	*
	* ��ʹ�ñ����ķ�ʽ����
	*/
	NodeType* Find( const NodeType* pNode );

	/**
	* ���Find()������֧����Щ�����򡱵������ṹ��Ҳ���ǣ���ʹ��
	* ��֮ǰ������Ҫȷ�����������ù�Sort()����
	*
	* ��ʹ�ö��ֲ����㷨
	*/
	NodeType* Find( const NodeSuperType* pNodeSuperType );

private:
	/* ����һ�����취���������⣬������Ż� */
	/*template <class T>
	class CMENodeCompareT
	{
	public:
		BOOL operator () ( const T& pSrcNode, const T& pDstNode )
		{
			BOOL bResult = (*pSrcNode == *pDstNode);

			return !bResult;
		}
	};*/

private:
	typedef std::list<NodeType*> ListType;

private:
	ListType						m_List;	// �ڵ�洢�ṹ
};

template<class ListNodeType> 
class CNodeListDefaultSort 
{
public:
	/**
	* ��ΪCNodeListT��Ĭ������
	*
	* ������Ҫ�������ǣ�ʲô������
	*/
	BOOL operator () ( const ListNodeType& lntSrc, const ListNodeType& lntDst );
};


#include "NodeListT.hpp"

ME_NAME_SPACE_END

#endif	// __NODE_LIST_T_H_
















