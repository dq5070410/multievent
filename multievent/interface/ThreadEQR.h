/**
 *  带有事件队列和分离器的线程对象类声明
 *
 *  History
 *  01/29/2013 create file
 */

#ifndef __THREAD_EQR_H_
#define __THREAD_EQR_H_

#include "ThreadReactor.h"

ME_NAME_SPACE_BEGIN

/**
 *  带有事件队列和分离器的线程对象类
 *
 *  因为CMEThreadEQR没有时钟队列，所以需要覆盖基类中的接口实现，返回不支持，而
 *  事件的投递因为基类已经实现了，所以只是继承了就OK了
 *
 *  参考:
 *      CMEThreadReactor
 */
class CMEThreadEQR : public CMEThreadReactor
{
public:
    /**
     *  构造函数
     *
     *  <bBlock>, 指定线程是否为阻塞模式
     *  <iThreadID>, 指定线程对象的ID
     *  <iType>, 指定线程对象的类型
     *  <pReactor>, 指定一个分离器的实例
     *  <pEventQueue>, 指定一个事件队列的实例
     *  <pThreadSink>, 指定一个用户通知回调（允许NULL）
     *
     *  返回值:
     *      无
     */
    CMEThreadEQR(
        BOOL bBlock,
        INT iThreadID,
        INT iType,
        CMEReactor* pReactor,
        CMEEventQueue* pEventQueue,
        IMEThreadSink* pThreadSink );

    /**
     *  析构函数
     *
     *  返回值:
     *      无
     */
    virtual ~CMEThreadEQR();

    /**
     *  添加时钟对象，覆盖基类实现，返回不支持，因为这种类型的线程对象不具有时钟
     *  队列
     *
     *  <pTimer>, 时钟对象实例指针
     *  <pSink>, 时钟回调实例指针
     *  <hktInterval>, 时钟循环周期
     *  <hktDelay>, 时钟延时时间
     *  <nLoopTime>, 时钟循环次数，默认值0，只调用一次；ME_INFINITE则是无限次
     *
     *  返回值:
     *      ME_ERROR_NOT_AVAILABLE表示不支持
     */
    virtual ME_Result ScheduleTimer(
        CMETimer* pTimer,
        IMETimerSink* pSink,
        const CMETimeValue& htvInterval,
        const CMETimeValue& htvDelay,
        UINT nLoopTime = 0 );

    /**
     *  取消时钟对象，覆盖基类实现，返回不支持，因为这种类型的线程对象不具有时钟
     *  队列
     *
     *  <pTimer>, 时钟对象实例指针
     *
     *  返回值:
     *      ME_ERROR_NOT_AVAILABLE表示不支持
     */
    virtual ME_Result CancelTimer( CMETimer* pTimer );
};

ME_NAME_SPACE_END

#endif // __THREAD_EQR_H_

