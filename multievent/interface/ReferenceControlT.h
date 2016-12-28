/**
* 引用计数类
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
* 引用计数类
* 
* 基于原子操作CMEAtomicOperationT
*/
class IMEReferenceControl
{
public:
	virtual ~IMEReferenceControl() {}

public:
	/**
	* 增加引用计数1
	*
	* 返回值:
	*		递增之后的引用计数, DWORD
	*/
	virtual DWORD AddReference() = 0;

	/**
	* 减少引用计数1
	*
	* 返回值:
	*		递减之后的引用计数, DWORD
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
	* 取得当前引用计数
	*
	* 返回值:
	*		DWORD值
	*/
	virtual DWORD GetReference();

protected:

	/**
	* 当引用计数为0时, 这个事件将会发生
	*
	* 它默认的行为是删除自己
	* 继承类如果有特殊要求, 可以重载这个函数, 但一定要记住
	* 在执行特定逻辑之后, 不要忘了调用CMEReferenceControlT<xx>::OnReferenceRelease()
	* 或者在重载之后的OnReferenceRelease()增加delete this
	*
	* 返回值:
	*		无
	*/
	virtual void OnReferenceRelease();

protected:
	CMEAtomicOperationT<LockType>	m_Atomic;
};

#include "ReferenceControlT.hpp"

ME_NAME_SPACE_END

#endif	// __REFERENCE_CONTROL_T_H_


