/**
 *  带有事件队列，时钟队列以及分离器的线程对象类声明
 *
 *  History
 *  01/29/2016 create file
 */

#ifndef __THREADEQTQR_H_
#define __THREADEQTQR_H_

#include "HikThreadReactor.h"

ME_NAME_SPACE_BEGIN

/**
 *  带有事件队列，时钟队列以及分离器的线程对象类
 *
 *  这个类存在的意义是在于和CMEThreadEQR、CMEThreadTQR处于相同的设计层次上，它
 *  的功能HikThreadReactor都已实现，CMEThreadEQTQR则是提供构造和析构函数，能够
 *  创建具有事件队列、时钟队列和分离器的线程对象类型
 *
 *  参考:
 *      CMEThreadReactor
 */
class CMEThreadEQTQR : public CMEThreadReactor
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
     *  <pEventQueue>, 指定一个事件队列的实例
     *  <pThreadSink>, 指定一个用户通知回调（允许NULL）
     *
     *  返回值:
     *      无
     */
    CMEThreadEQTQR(
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
    virtual ~CMEThreadEQTQR();
};

ME_NAME_SPACE_END

#endif // __THREADEQTQR_H_

