/**
* ԭ�Ӳ�����
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
* ԭ�Ӳ�����
* 
* �����ACE�Լ�linux atomic_2.hʵ��
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
* ԭ�Ӳ�����
* 
* �����ACE�Լ�linux atomic_2.hʵ��
*
* ���Ǹ�ƫ�ػ��汾��ԭ�Ӳ��������ϱ��Ǹ������಻ͬ����
* ����汾ʹ���˲���ϵͳ�ṩ��ԭ�Ӳ�����Ч�ʴ�����ϱ�
* �Ǹ���6~7����
*/
template <>
class CMEAtomicOperationT<CMELockMutex>
{
public:
	/**
	* ֻҪ������캯����ʵ��д�������û������
	* ����������hpp�ļ���ʵ�֣��ͻ�һֱ����LNK2005����(����û���⣬linkͨ����)
	* 
	* û�취��ֻ�ܷ���������
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









