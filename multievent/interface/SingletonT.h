/**
* 单例模版类
* 
*
* History
*	12/08/2012		Created
*/

#ifndef __SINGLETON_H_
#define __SINGLETON_H_

#include "CleanupBase.h"
#include "LockGuardT.h"

ME_NAME_SPACE_BEGIN

/*
 * 一个保证单例的工具类
 * 所用使用了这个工具类的实例都不需要也不允许delete
 * 这些实例会在进程退出之前被删除
 *
 * 如何使用:
 *
 * class CTest
 * {
 * public:
 *		CTest();	// 确保<CTest>的构造函数(至少其中一个构造函数)不带参数, 否则会导致编译失败
 *		void Func();
 * }
 *
 * typedef CMESingletonT<CTest> CTestSingleton;
 * CTestSingleton::Instance()->Func();
 * 
*/
template <class ObjectType>
class CMESingletonT : public CMECleanupBase
{
public:
	static ObjectType* Instance();

	static ObjectType* Instance( ObjectType* pObj );

protected:
	CMESingletonT();
	virtual ~CMESingletonT();

private:
	void operator = ( const CMESingletonT& );
	CMESingletonT( const CMESingletonT& );

protected:
	ObjectType						m_Intance;
	/**
	* 声明自己，在释放的时候设置为空。
	* 在反复创建时就能够保证单例对象的正确创建。
	*/
	static CMESingletonT<ObjectType>* s_pInstance;
};

template <class ObjectType> 
CMESingletonT<ObjectType>* CMESingletonT<ObjectType>::s_pInstance = NULL;

#include "SingletonT.hpp"

ME_NAME_SPACE_END

#endif	// __SINGLETON_H_




















