/**
* 智能指针类(基于ReferenceControl)
* 
*
* History
*	12/11/2016		Created
*/

#ifndef __AUTO_PTR_T_H_
#define __AUTO_PTR_T_H_

ME_NAME_SPACE_BEGIN

/**
* 智能指针类(基于引用计数)
* 
* 请确保放入的对象是继承自IMEReferenceControlT
*/
template <class ReferenceObject>
class CMEAutoPtrT
{
public:
	CMEAutoPtrT( ReferenceObject* pObj = NULL );

	CMEAutoPtrT( const CMEAutoPtrT& AutoPtr );

	~CMEAutoPtrT();

public:
	CMEAutoPtrT& operator = ( const CMEAutoPtrT& AutoPtr );

	CMEAutoPtrT& operator = ( ReferenceObject* pReferenceObject );

	/**
	* 重载void*是为了在以下这种情况时, 编译通过
	* if ( <autoptr> ) { ... }
	* 非默认类型的对象会被编译器首先转换成void*, 然后再进行比较
	*
	* 返回值:
	*		<m_pObj>
	*/
	operator void* () const;

	ReferenceObject& operator * () const;

	ReferenceObject* operator -> () const;

	/**
	 * 取得原始指针
	 *
	 * 返回值:
	 *		非NULL为成功; NULL为失败
	*/
	ReferenceObject* Get() const;

	/**
	 * 传入参数, 类似于函数参数中普通的指针传入, 如Func( void* p );
	 * 
	 * 返回值:
	 *		非NULL为成功; NULL为失败
	*/
	ReferenceObject* ParaIn() const;

	/**
	 * 引用传入, 类似于函数参数中的引用指针传入, 如Func( void*& p );
	 * 当原始指针不为空时, 这个函数会首先减少引用计数, 然后置为NULL
	 *
	 * 返回值:
	 *		<m_pObj>
	*/
	ReferenceObject*& ParaOut();

	/**
	* 引用传入, 类似于函数参数中的引用指针传入, 如Func( void*& p );
	* 与ParaOut()所不同的是, 这个函数不会检查原始指针是否为空
	* 在你没有把握原始指针是否为空时, 不要用这个函数, 否则会引起内存泄漏
	*
	* 返回值:
	*		<m_pObj>
	*/
	ReferenceObject*& ParaInOut();

	bool operator < ( const CMEAutoPtrT& hapSource ) const;

	bool operator < ( const ReferenceObject* hapSource ) const;

	bool operator () (
		const CMEAutoPtrT& hapSource, 
		const CMEAutoPtrT& hapDest ) const;

private:
	ReferenceObject*				m_pObj;
};

#include "AutoPtrT.hpp"

ME_NAME_SPACE_END

#endif	// __ME_AUTO_PTR_T_H_



