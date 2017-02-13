/**
 *  仅带有事件队列的线程对象类声明
 *
 *  History
 *  01/29/2016 create file
 */

#ifndef __THREAD_EQ_H_
#define __THREAD_EQ_H_

#include "Thread.h"

ME_NAME_SPACE_BEGIN


/**
 *  仅带有事件队列的线程对象类
 *
 *  仅带有事件队列，支持事件投递，但需要注意的是这种类型的线程对象会不间断的通知
 *  事件队列检查并处理事件，可能会导致较高的CPU占用
 *
 */
class CMEThreadEQ : public CMEThread
{
public:
    /**
     *  构造函数
     *
     *  <bBlock>, 指定线程是否为阻塞模式
     *  <iThreadID>, 指定线程对象的ID
     *  <iType>, 指定线程对象的类型
     *  <pEventQueue>, 指定一个事件队列的实例
     *  <pThreadSink>, 指定一个用户通知回调（允许NULL）
     *
     *  返回值:
     *      无
     */
    CMEThreadEQ(
        BOOL bBlock,
        INT iThreadID,
        INT iType,
        CEventQueue* pEventQueue,
        IMEThreadSink* pThreadSink );

    /**
     *  析构函数
     *
     *  返回值:
     *      无
     */
    virtual ~CMEThreadEQ();

    /**
     *  非阻塞式事件投递，覆盖基类实现
     *
     *  <pEvent>, 自定义事件实例指针
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result PostEvent( ICustomizeEvent* pEvent );

    /**
     *  阻塞式事件投递，覆盖基类实现
     *
     *  <pEvent>, 自定义事件实例指针
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result SendEvent( ICustomizeEvent* pEvent );

    /**
     *  等待线程停止，覆盖基类实现，这里实际上还是会调用基类的Join，但是在这之前
     *  会修改线程对象的运行标识，并投递停止事件，使线程对象进入停止过程
     *
     *  <iTimeout>, 等待超时时间，单位：毫秒
     *  <bTerminate>, 是否在等待超时后强制终止线程
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result Join(
        UINT iTimeout,
        BOOL bTerminate );

protected:
    /**
     *  线程对象工作过程，这种类型的线程对象就是不断的通知事件队列检查并处理事件
     *
     *  返回值:
     *      NULL
     */
    virtual ME_THREAD_RETURN_TYPE Worker();

private:
    friend class CMEThreadFactory;

    /* 事件队列实例指针 */
    CEventQueue*                 m_pEventQueue;
};

ME_NAME_SPACE_END

#endif // __THREAD_EQ_H_

