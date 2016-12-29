/**
 *  带有时钟队列和分离器的线程对象类声明
 *
 *  History
 *  01/29/2016 create file
 */

#ifndef __THREAD_TQR_H_
#define __THREAD_TQR_H_

#include "ThreadReactor.h"

ME_NAME_SPACE_BEGIN

/**
 *  带有时钟队列和分离器的线程对象类
 *
 *  因为CMEThreadTQR没有事件队列，因此不支持事件的投递，需要覆盖基类中的事件投
 *  递的实现，返回不支持；而时钟的添加和取消因为基类就已经实现了，所以只需要继承
 *  就OK了
 *
 *  参考:
 *      CMEThreadReactor
 */
class CMEThreadTQR : public CMEThreadReactor
{
public:
    /**
     *  构造函数
     *
     *  <bBlock>, 指定线程是否为阻塞模式
     *  <iThreadID>, 指定线程对象的ID
     *  <iType>, 指定线程对象的类型
     *  <pReactor>, 指定一个分离器的实例
     *  <pTimerQueue>, 指定一个时钟队列的实例
     *  <pThreadSink>, 指定一个用户通知回调（允许NULL）
     *
     *  返回值:
     *      无
     */
    CMEThreadTQR(
        BOOL bBlock,
        INT iThreadID,
        INT iType,
        CMEReactor* pReactor,
        CMETimerQueue* pTimerQueue,
        IMEThreadSink* pThreadSink );

    /**
     *  析构函数
     *
     *  返回值:
     *      无
     */
    virtual ~CMEThreadTQR();

    /**
     *  非阻塞式事件投递，当前线程对象不支持，需要覆盖基类CMEReactorR的实现，返
     *  回不支持
     *
     *  <pEvent>, 事件对象实例指针
     *
     *  返回值:
     *      ME_ERROR_NOT_AVAILABLE表示不支持
     */
    virtual ME_Result PostEvent( IMECustomizeEvent* pEvent );

    /**
     *  阻塞式事件投递，当前线程对象不支持，需要覆盖基类CMEReactorR的实现，返回
     *  不支持
     *
     *  <pEvent>, 事件对象实例指针
     *
     *  返回值:
     *      ME_ERROR_NOT_AVAILABLE表示不支持
     */
    virtual ME_Result SendEvent( IMECustomizeEvent* pEvent );
};

ME_NAME_SPACE_END

#endif // __THREAD_TQR_H_

