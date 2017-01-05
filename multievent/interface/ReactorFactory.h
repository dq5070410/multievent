/**
 *  分离器工厂类声明
 *
 *
 *  History
 *  02/19/2016 create file
 */


#ifndef __REACTOR_FACTORY_H_
#define __REACTOR_FACTORY_H_

#include "Base.h"
#include "SingletonT.h"
#include "LockGuardT.h"

ME_NAME_SPACE_BEGIN

class CMEReactor;

class CMEReactorFactory
{
public:
    /**
     *  根据分离器类型创建分离器
     *
     *  <iType>, 分离器的类型
     *  <pReactor>, 输出分离器实例指针（如果成功），传入的指针应置为NULL
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    ME_Result Create(
        INT iType,
		INT iClassificationType,	// 2013.9.16 added by 韦珂
        CMEReactor*& pReactor );

	CMEReactor* Create( 
		INT iType,
		DWORD dwThreadNum );

    /**
     *  销毁分离器
     *
     *  <pReactor>, 分离器实例指针
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
	ME_Result Destroy( CMEReactor*& pReactor );
};

typedef CMESingletonT<CMEReactorFactory> CMEReactorFactorySingleton;

ME_NAME_SPACE_END

#endif // __REACTOR_FACTORY_H_

