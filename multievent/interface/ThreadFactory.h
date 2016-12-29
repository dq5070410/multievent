/**
 *  线程对象工厂类声明
 *
 *  History
 *  02/19/2013 create file
 */


#ifndef __THREAD_FACTORY_H_
#define __THREAD_FACTORY_H_

#include "Base.h"
#include "SingletonT.h"
#include "LockGuardT.h"

ME_NAME_SPACE_BEGIN

class CMEThread;
class IMEThreadSink;

class CMEThreadFactory
{
public:
    /**
     *  创建线程对象
     *
     *  <bBlock>, 指定线程对象是否阻塞
     *  <iThreadID>, 指定线程对象的ID，这个ID应是线程对象管理器指定的
     *  <iThreadType>, 线程对象的类型
     *  <iReactorType>, 分离器类型，如果线程对象不带有分离器，那么将忽略此参数
     *  <pThreadSink>, 用户逻辑回调
     *  <pThread>, 输出线程对象实例指针，如果成功；否则，输出NULL
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    ME_Result Create(
        BOOL bBlock,
        INT iThreadID,
        INT iThreadType,
		INT iClassificationType,	// 2013.9.16 added by 韦珂
        INT iReactorType,
        IMEThreadSink* pThreadSink,
        CMEThread*& pThread );

    /**
     *  销毁线程对象实例
     *
     *  <pThread>, 线程对象实例指针
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    ME_Result Destroy( CMEThread*& pThread );

private:
	/**
     *  根据不同入参类型创建队列
     *
     *  <pQueue>, out, 队列实例指针
	 *  <iType>, 线程类型(用户线程/网络线程/其他)
     *
     *  返回值:
     *      无
     */
	template <class Queue>
	void CreateQueue( 
		Queue*& pQueue,
		int iType );
};

typedef CMESingletonT<CMEThreadFactory> CMEThreadFactorySingleton;

ME_NAME_SPACE_END

#endif // __THREAD_FACTORY_H_

