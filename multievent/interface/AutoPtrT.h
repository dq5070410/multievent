/**
* ����ָ����(����ReferenceControl)
* 
*
* History
*	12/11/2016		Created
*/

#ifndef __AUTO_PTR_T_H_
#define __AUTO_PTR_T_H_

ME_NAME_SPACE_BEGIN

/**
* ����ָ����(�������ü���)
* 
* ��ȷ������Ķ����Ǽ̳���IMEReferenceControlT
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
	* ����void*��Ϊ���������������ʱ, ����ͨ��
	* if ( <autoptr> ) { ... }
	* ��Ĭ�����͵Ķ���ᱻ����������ת����void*, Ȼ���ٽ��бȽ�
	*
	* ����ֵ:
	*		<m_pObj>
	*/
	operator void* () const;

	ReferenceObject& operator * () const;

	ReferenceObject* operator -> () const;

	/**
	 * ȡ��ԭʼָ��
	 *
	 * ����ֵ:
	 *		��NULLΪ�ɹ�; NULLΪʧ��
	*/
	ReferenceObject* Get() const;

	/**
	 * �������, �����ں�����������ͨ��ָ�봫��, ��Func( void* p );
	 * 
	 * ����ֵ:
	 *		��NULLΪ�ɹ�; NULLΪʧ��
	*/
	ReferenceObject* ParaIn() const;

	/**
	 * ���ô���, �����ں��������е�����ָ�봫��, ��Func( void*& p );
	 * ��ԭʼָ�벻Ϊ��ʱ, ������������ȼ������ü���, Ȼ����ΪNULL
	 *
	 * ����ֵ:
	 *		<m_pObj>
	*/
	ReferenceObject*& ParaOut();

	/**
	* ���ô���, �����ں��������е�����ָ�봫��, ��Func( void*& p );
	* ��ParaOut()����ͬ����, �������������ԭʼָ���Ƿ�Ϊ��
	* ����û�а���ԭʼָ���Ƿ�Ϊ��ʱ, ��Ҫ���������, ����������ڴ�й©
	*
	* ����ֵ:
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



