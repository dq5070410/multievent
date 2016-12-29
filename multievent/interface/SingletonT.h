/**
* ����ģ����
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
 * һ����֤�����Ĺ�����
 * ����ʹ��������������ʵ��������ҪҲ������delete
 * ��Щʵ�����ڽ����˳�֮ǰ��ɾ��
 *
 * ���ʹ��:
 *
 * class CTest
 * {
 * public:
 *		CTest();	// ȷ��<CTest>�Ĺ��캯��(��������һ�����캯��)��������, ����ᵼ�±���ʧ��
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
	* �����Լ������ͷŵ�ʱ������Ϊ�ա�
	* �ڷ�������ʱ���ܹ���֤�����������ȷ������
	*/
	static CMESingletonT<ObjectType>* s_pInstance;
};

template <class ObjectType> 
CMESingletonT<ObjectType>* CMESingletonT<ObjectType>::s_pInstance = NULL;

#include "SingletonT.hpp"

ME_NAME_SPACE_END

#endif	// __SINGLETON_H_




















