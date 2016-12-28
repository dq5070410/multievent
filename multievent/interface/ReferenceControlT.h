/**
* ���ü�����
* 
*
* History
*	12/11/2012		Created
*/

#ifndef __REFERENCE_CONTROL_T_H_
#define __REFERENCE_CONTROL_T_H_

#include "AtomicOperationT.h"

ME_NAME_SPACE_BEGIN

/**
* ���ü�����
* 
* ����ԭ�Ӳ���CMEAtomicOperationT
*/
class IMEReferenceControl
{
public:
	virtual ~IMEReferenceControl() {}

public:
	/**
	* �������ü���1
	*
	* ����ֵ:
	*		����֮������ü���, DWORD
	*/
	virtual DWORD AddReference() = 0;

	/**
	* �������ü���1
	*
	* ����ֵ:
	*		�ݼ�֮������ü���, DWORD
	*/
	virtual DWORD RemoveReference() = 0;
};

template <class LockType>
class CMEReferenceControlT : public IMEReferenceControl
{
public:
	virtual DWORD AddReference();

	virtual DWORD RemoveReference();

	/**
	* ȡ�õ�ǰ���ü���
	*
	* ����ֵ:
	*		DWORDֵ
	*/
	virtual DWORD GetReference();

protected:

	/**
	* �����ü���Ϊ0ʱ, ����¼����ᷢ��
	*
	* ��Ĭ�ϵ���Ϊ��ɾ���Լ�
	* �̳������������Ҫ��, ���������������, ��һ��Ҫ��ס
	* ��ִ���ض��߼�֮��, ��Ҫ���˵���CMEReferenceControlT<xx>::OnReferenceRelease()
	* ����������֮���OnReferenceRelease()����delete this
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnReferenceRelease();

protected:
	CMEAtomicOperationT<LockType>	m_Atomic;
};

#include "ReferenceControlT.hpp"

ME_NAME_SPACE_END

#endif	// __REFERENCE_CONTROL_T_H_


