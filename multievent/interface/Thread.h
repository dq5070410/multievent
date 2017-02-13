/**
 *  线程对象基类声明
 *
 *  History
 *  01/29/2016 create file
 */

#ifndef __ME_THREAD_H_
#define __ME_THREAD_H_

#include "ThreadInterface.h"
#include "EventQueueT.h"
#include "TimerQueueT.h"
#include "LockThreadMutex.h"

#include "LockEvent.h"

ME_NAME_SPACE_BEGIN

// 预声明
class ITimerSink;
class CTimeValue;
class CMETimer;
class CMEReactor;

/**
 *  线程对象基类
 *
 *  各种不同类型线程对象的基类，继承并实现IMEThread的接口，并扩展了诸如时钟，等
 *  待停止等接口，用于组件内部的开发。 它也承担对相同操作采用统一的实现方式，需要
 *  注意的是它本身不具有事件队列或是时钟队列这些属性，事件投递或时钟的操作都会返
 *  回不支持，而支持这些操作的子类则需要覆盖基类的实现
 *
 */
class CMEThread : public IMEThread
{
public:
    /* 线程对象的状态 */
    enum RUN_STATE
    {
        RUN_STATE_NONE = 300,

        // 表示线程对象正处于正常运行状态
        RUN_STATE_RUNNING,

        // 表示线程对象处于退出状态或已经退出
        RUN_STATE_EXIT,
    };

    CMEThread(
        BOOL bBlock,
        INT iThreadID,
        INT iType,
        IMEThreadSink* pThreadSink );

    virtual ~CMEThread();

    /**
     *  获得线程对象ID，获取线程对象在MultiEvent组件中的ID，注意因为它不是操作系
     *  统中的线程ID，所以不能尝试调用操作系统的线程管理接口来操作这个ID
     *
     *  返回值:
     *      >= 1，线程对象ID
     */
    virtual INT GetThreadID() const;

    /**
     *  获得线程对象类型，获取线程对象的类型，这个类型和创建时指定的类型相同
     *
     *  返回值:
     *      线程对象类型
     *
     *  参考:
     *      IMEThreadManager::ME_THREAD_OPTION
     */
    virtual INT GetThreadType() const;

    /**
     *  运行线程对象，注意，如果是非阻塞模式的线程对象，则不需要额外调用此接口，
     *  线程对象管理器在创建线程对象后会自动启动线程对象；如果是阻塞模式的线程
     *  对象，那么就需要不断调用Run让线程对象不断运行
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result Run();

    /**
     *  等待线程停止
     *
     *  <iTimeout>, 等待超时时间，单位：毫秒
     *  <bTerminate>, 是否在等待超时后强制终止线程
     *
     *  返回值:
     *      ME_OK表示成功; ME_ERROR_TIMEOUT表示超时; 其他表示失败
     */
    virtual ME_Result Join(
        UINT iTimeout,
        BOOL bTerminate );

    /**
     *  非阻塞式事件投递，如果线程对象具有事件队列，那么在调用这个接口后，在事件
     *  被成功投递到事件队列后就会返回；如果线程对象不具有事件队列，那么调用这个
     *  接口会返回错误
     *
     *  <pEvent>, 自定义事件对象实例指针
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result PostEvent( ICustomizeEvent* pEvent );

    /**
     *  阻塞式事件投递，如果线程对象具有事件队列，那么在调用这个接口后，在事件被
     *  成功投递到事件队列后，会一直等待事件被执行；如果线程对象不具有事件队列，
     *  那么调用这个接口会返回错误
     *  需要关注的是，如果在同一个线程对象的环境中调用此接口，也会返回错误
     *
     *  <pEvent>, 自定义事件对象实例指针
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result SendEvent( ICustomizeEvent* pEvent );

    /**
     *  向线程对象的时钟队列添加一个时钟对象
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
        ITimerSink* pSink,
        const CTimeValue& htvInterval,
        const CTimeValue& htvDelay,
        UINT nLoopTime = 0 );

    /**
     *  从线程对象的时钟队列中取消一个时钟对象
     *
     *  <pTimer>, 时钟对象实例指针
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result CancelTimer( CMETimer* pTimer );

    /**
     *  获取线程对象上的分离器实例
     *
     *  注意，基类是不带有分离器的，所以默认返回NULL
     *
     *  返回值:
     *      非空，分离器实例指针; NULL，当前线程对象不带有分离器
     */
    virtual CMEReactor* GetReactor();

protected:
    /**
     *  线程回调函数，在非阻塞模式下创建线程使用
     *
     *  <pParam>, 线程回调用户数据指针，这里应该就是线程对象本身
     *
     *  返回值:
     *      NULL
     */
    static ME_THREAD_RETURN_TYPE ME_STDCALL ThreadFunc( LPVOID pParam );

    /**
     *  线程对象工作过程函数，不同运行方式的线程对象类型都要继承并实现这个接口
     *
     *  返回值:
     *      NULL
     */
    virtual ME_THREAD_RETURN_TYPE Worker() = 0;

public:
    /**
     *  获取线程对象实际的线程ID
     *
     *  返回值:
     *      线程对象实际的线程ID
     */
    ME_THREAD_ID GetThreadOSID() const;

    /**
     *  获取线程对象实际的句柄
     *
     *  返回值:
     *      线程对象实际的句柄
     */
    ME_THREAD_HANDLE GetThreadHandle();

protected:
    /* 标识线程对象是否为阻塞模式 */
    BOOL                            m_bBlock;

    /* 标识线程对象的运行状态 */
    INT                             m_iStatus;

    /* 用户回调类型指针 */
    IMEThreadSink*                   m_pThreadSink;

    /* 线程句柄(对应OS中的线程句柄) */
    ME_THREAD_HANDLE               m_hThreadHandle;

    /* 线程ID(对应OS中的线程ID) */
    ME_THREAD_ID                   m_ThreadID;

    /* 线程锁 */
    CMELockThreadMutex             m_hThreadMutex;

private:
    /* 声明线程对象管理器为线程对象的友元类 */
    friend class CMEThreadManager;

    /* 线程对象ID(线程对象管理器分配的ID，不是OS中的线程ID) */
    INT                             m_iThreadID;

    /* 线程对象类型 */
    INT                             m_iThreadType;

	CLockEvent					m_lockEvent;	// 线程真正跑起来之后，通知其他等待线程
};

ME_NAME_SPACE_END

#endif // __ME_THREAD_H_

