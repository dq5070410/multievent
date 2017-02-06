/**
* 节点队列模版类
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
	* 增加一个节点
	*
	* <pNode>，增加什么节点
	*
	* 返回值：
	*		ME_OK为成功；其他为失败
	*/
	ME_Result Add( NodeType* pNode );

	/**
	* 删除一个节点
	*
	* <pNode>，哪一个节点
	*
	* 返回值：
	*		ME_OK为成功；其他为失败
	*/
	ME_Result Remove( NodeType* pNode );

	/**
	* 获取第一个节点
	*
	* 返回值：
	*		成功为非NULL；其他为NULL
	*/
	NodeType* GetFirstItem();

	/**
	* 删除第一个节点并返回该节点的值
	*
	* 返回值：
	*		成功为非NULL；其他为NULL
	*/
	NodeType* EraseFirstItem();

	/**
	* 获取不超过<DELIVERY_MAX_NUM>的“活动的”节点链表
	*
	* <activeList>，out，成功时该链表的size()大于0
	*
	* 返回值：
	*		返回活动节点的个数(0 ~ DELIVERY_MAX_NUM)
	*/
	UINT GetActiveNodes( CMENodeListT<NodeType, SorterType, DELIVERY_MAX_NUM>& activeList );

	/**
	* 获取当前链表的size
	*
	* 返回值：
	*		UINT范围的任意值
	*/
	UINT Size() const;

	/**
	* 获取当前链表是否为空
	*
	* 返回值：
	*		TRUE或者FALSE
	*/
	BOOL IsEmpty() const;

	/**
	* 根据SorterType类型进行排序
	*
	* 需要指定的SorterType重载operator ()
	*
	* 返回值：
	*		无
	*/
	void Sort();

	/**
	* 无特殊要求，不过，效率较低
	*
	* 它使用遍历的方式查找
	*/
	NodeType* Find( const NodeType* pNode );

	/**
	* 这个Find()函数仅支持那些“有序”的容器结构，也就是，在使用
	* 它之前，你需要确认你曾经调用过Sort()函数
	*
	* 它使用二分查找算法
	*/
	NodeType* Find( const NodeSuperType* pNodeSuperType );

private:
	/* 先用一个笨办法解决这个问题，后边再优化 */
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
	ListType						m_List;	// 节点存储结构
};

template<class ListNodeType> 
class CNodeListDefaultSort 
{
public:
	/**
	* 作为CNodeListT的默认排序
	*
	* 它的主要工作就是：什么都不做
	*/
	BOOL operator () ( const ListNodeType& lntSrc, const ListNodeType& lntDst );
};


#include "NodeListT.hpp"

ME_NAME_SPACE_END

#endif	// __NODE_LIST_T_H_
















