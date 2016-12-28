/**
* 原子操作类
* 
*
* History
*	12/08/2016		Created
*/

#ifndef __ATOMIC_OPERATION_H_
#define __ATOMIC_OPERATION_H_

#include "Base.h"
#include "LockMutex.h"
#include "LockGuardT.h"

ME_NAME_SPACE_BEGIN

/**
* 原子操作类
* 
* 借鉴了ACE以及linux atomic_2.h实现
*/
template <class LockType> 
class CMEAtomicOperationT
{
public:
	CMEAtomicOperationT( long lValue = 0 );

	long operator ++ ();

	long operator ++ ( int );

	long operator += ( long lSourceValue );

	long operator -= ( long lSourceValue );

	long operator -- ();

	long operator -- ( int );

	int operator == ( long lSourceValue ) const;

	int operator != ( long lSourceValue ) const;

	int operator >= ( long lSourceValue ) const;

	int operator > ( long lSourceValue ) const;

	int operator <= ( long lSourceValue ) const;

	int operator < ( long lSourceValue ) const;

	void operator = ( long lSourceValue );

	void operator = ( const CMEAtomicOperationT<LockType>& haoSourceValue );

	long GetValue() const;

private:
	LockType						m_Mutex;
	long							m_lValue;
};

/**
* 原子操作类
* 
* 借鉴了ACE以及linux atomic_2.h实现
*
* 这是个偏特化版本的原子操作，以上边那个泛型类不同的是
* 这个版本使用了操作系统提供的原子操作，效率大概是上边
* 那个的6~7倍吧
*/
template <>
class CMEAtomicOperationT<CMELockMutex>
{
public:
	/**
	* 只要这个构造函数的实现写在这里，就没有问题
	* 如果在下面的hpp文件里实现，就会一直出现LNK2005错误(编译没问题，link通不过)
	* 
	* 没办法，只能放在这里了
	*/
	CMEAtomicOperationT( long lValue = 0 ) 
		: m_lValue( lValue )
	{

	}

	//CMEAtomicOperationT( long lValue = 0 );

	long operator ++ ();

	long operator ++ ( int );

	long operator += ( long lSourceValue );

	long operator -= ( long lSourceValue );

	long operator -- ();

	long operator -- ( int );

	int operator == ( long lSourceValue ) const;

	int operator != ( long lSourceValue ) const;

	int operator >= ( long lSourceValue ) const;

	int operator > ( long lSourceValue ) const;

	int operator <= ( long lSourceValue ) const;

	int operator < ( long lSourceValue ) const;

	void operator = ( long lSourceValue );

	void operator = ( const CMEAtomicOperationT<CMELockMutex>& haoSourceValue );

	long GetValue() const;

private:
	volatile long					m_lValue;
};

#include "AtomicOperationT.hpp"

ME_NAME_SPACE_END

#endif	// __ATOMIC_OPERATION_H_









