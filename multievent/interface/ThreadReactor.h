/**
 *  以分离器作为运行方式的线程对象类声明
 *
 *  History
 *  01/29/2016 create file
 */

#ifndef __ME_THREAD_REACTOR_H_
#define __ME_THREAD_REACTOR_H_

#include "Thread.h"

ME_NAME_SPACE_BEGIN

/* 预声明 */
class CMEReactor;


/**
 *  以分离器作为运行方式的线程对象类
 *
 *  以分离器作为运行方式的意思是这种线程对象的工作过程主要以运行分离器的捕获——分
 *  派过程为主，虽然带有分离器的线程对象有3种，但是运行的方式是一样的，都是由分
 *  离器来驱动事件的捕获和分派的，所以设置了这样一个基类，实际上这个类已经具有子
 *  类所有的功能，而它的子类则根据属性的不同，只具有部分的功能
 *
 *  注意：在构造线程对象的时候应该保证传入的参数是正确的，线程对象工厂必须要做到
 *        这一点
 *
 *  参考:
 *      CMEReactorEQR
 *      CMEReactorTQR
 *      CMEReactorEQTQR
 */
class CMEThreadReactor : public CMEThread
{
public:
    /**
     *  非阻塞式事件投递，带有分离器的线程对象，在投递事件成功后还需要通知分离器
     *  跳出等待
     *
     *  <pEvent>, 自定义事件实例指针
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result PostEvent( IMECustomizeEvent* pEvent );

    /**
     *  阻塞式事件投递，带有分离器的线程对象，在投递事件成功后，通知分离器跳出等
     *  待，然后再等待事件执行才返回
     *
     *  <pEvent>, 自定义事件实例指针
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result SendEvent( IMECustomizeEvent* pEvent );

    /**
     *  添加时钟对象，向时钟队列添加时钟对象后，还需要通知分离器跳出等待
     *
     *  <pTimer>, 时钟对象实例指针
     *  <pSink>, 时钟回调实例指针
     *  <hktInterval>, 时钟循环周期
     *  <hktDelay>, 时钟延时时间
     *  <nLoopTime>, 时钟循环次数，默认值0，只调用一次；ME_INFINITE则是无限次
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result ScheduleTimer(
        CMETimer* pTimer,
        IMETimerSink* pSink,
        const CMETimeValue& htvInterval,
        const CMETimeValue& htvDelay,
        UINT nLoopTime = 0 );

    /**
     *  取消时钟对象，根据时钟的索引号取消时钟队列中的时钟对象，同样需要通知分离
     *  器跳出等待
     *
     *  <pTimer>, 时钟对象实例指针
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result CancelTimer( CMETimer* pTimer );

    /**
     *  获取线程对象中的分离器
     *
     *  返回值:
     *      分离器实例指针
     */
    virtual CMEReactor* GetReactor();

    /**
     *  等待线程对象停止，覆盖基类CMEReactor的实现，首先通知分离器停止，让分离
     *  器进入退出过程，然后调用基类CMEReactor的Join，等待线程对象停止
     *
     *  <iTimeout>, 等待超时时间
     *  <bTerminate>, 是否允许强制终止线程对象
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result Join(
        UINT iTimeout,
        BOOL bTerminate );

protected:
    /**
     *  构造函数，声明为受保护的，CMEReactorR是不能够创建出的，它只是代表了一类
     *  线程对象的运行模式，而具体的采用这种运行模式的线程对象类型则是它的子类
     *
     *  <bBlock>, 指定线程是否为阻塞模式
     *  <iThreadID>, 指定线程对象的ID
     *  <iType>, 指定线程对象的类型
     *  <pReactor>, 指定一个分离器的实例
     *  <pTimerQueue>, 指定一个时钟队列的实例
     *  <pEventQueue>, 指定一个事件队列的实例
     *  <pThreadSink>, 指定一个用户通知回调（允许NULL）
     *
     *  返回值:
     *      无
     */
    CMEThreadReactor(
        BOOL bBlock,
        INT iThreadID,
        INT iType,
        CMEReactor* pReactor,
        CMETimerQueue* pTimerQueue,
        CMEEventQueue* pEventQueue,
        IMEThreadSink* pThreadSink );

    /**
     *  析构函数
     *
     *  返回值:
     *      无
     */
    virtual ~CMEThreadReactor();

    /**
     *  线程对象工作过程，对于带有分离器的线程对象类型，主要的工作过程就是调用分
     *  离器的Run接口，启动分离器的工作过程（即分离器的捕获——分派过程）
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_THREAD_RETURN_TYPE Worker();

private:
    friend class CMEThreadFactory;

    // 分离器实例指针
    CMEReactor*                    m_pReactor;

    // 时钟队列实例指针
    CMETimerQueue*                 m_pTimerQueue;

    // 事件队列实例指针
    CMEEventQueue*                 m_pEventQueue;
};

ME_NAME_SPACE_END

#endif // __THREAD_REACTOR_H_

