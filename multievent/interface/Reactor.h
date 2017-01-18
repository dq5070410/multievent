/**
 *  IO捕捉与分派器的壳
 * 
 *
 *  History
 *  12/12/2016 create file
 */

#ifndef __REACTOR_H_
#define __REACTOR_H_

#include "Base.h"
#include "EventQueueT.h"
#include "TimerQueueT.h"
#include "LockThreadMutex.h"

ME_NAME_SPACE_BEGIN

class IMEReactorImpl;
class IMEIO;
class IMEIOSink;
class IMECustomizeEvent;
class CMETimer;
class ITimerSink;

class ME_OS_EXPORT CMEReactor
{
public:
    /**
     *  分离器的类型
     */
    enum REACTOR_TYPE
    {
        REACTOR_TYPE_NULL = 1 << 0,

		REACTOR_TYPE_SYNC = 1 << 1,

		REACTOR_TYPE_ASYNC = 1 << 2,

        /* 采用select方式的分离器 */
        REACTOR_TYPE_SELECT = (1 << 3) | REACTOR_TYPE_SYNC,
        
        /* 采用WaitForMultipleObjects方式的分离器 */
        REACTOR_TYPE_WFMO = (1 << 4) | REACTOR_TYPE_SYNC,

		/* 异步模式，win32下有效，使用完成端口 */
		REACTOR_TYPE_ASYCH_IOCP = (1 << 5) | REACTOR_TYPE_ASYNC,

		/* 异步模式，linux有效，使用epoll */
		REACTOR_TYPE_ASYCH_EPOLL = (1 << 6) | REACTOR_TYPE_ASYNC,

		/* 异步模式，linux有效，使用aio系列 */
		REACTOR_TYPE_ASYCH_AIO = (1 << 7) | REACTOR_TYPE_ASYNC,
    };

    /**
     * 用于标识Reactor的运行模式
     */
    enum RUN_MODE
    {
        RUN_MODE_NONE = 100,

        /** 
         * 循环模式，这个模式会阻塞当前线程
         *
         * 在你调用Run()函数后会阻塞
         */
        RUN_MODE_LOOP,

        /** 
         * 心跳模式，在这个模式下，线程不会被阻塞 
         *
         * 在你调用Run()函数后，Reactor会在完成一次事件捕捉及
         * 分派之后，让Run()函数返回
         *
         * 所以当你需要在这个模式下不停的捕捉事件，那么，你就
         * 需要不停的调用Run()
         */
        RUN_MODE_HEART_BEAT,
    };

    enum NOTIFY_TYPE
    {
        NOTIFY_TYPE_NONE = 200,

        /* 用于唤醒Reactor */
        NOTIFY_TYPE_WAKEUP,

        /* 使Reactor退出阻塞 */
        NOTIFY_TYPE_EXIT,
    };

    enum RUN_STATE
    {
        RUN_STATE_NONE = 300,

        /* 表示Reactor正处于正常运行状态 */
        RUN_STATE_RUNNING,

        /* 表示Reactor处于或已经退出，这个状态，任何事件投递都会失败 */
        RUN_STATE_EXIT,
    };

public:
    /**
     * 参考自winsock2定义
     */
    enum EVENT_TYPE
    {
        EVENT_TYPE_NULL = 0,
        EVENT_TYPE_READ = 1 << 0,
        EVENT_TYPE_WRITE = 1 << 1,
        EVENT_TYPE_EXCEPTION = 1 << 2,
        EVENT_TYPE_ACCEPT = 1 << 3,
        EVENT_TYPE_CONNECT = 1 << 4,
        EVENT_TYPE_CLOSE = 1 << 5,
        EVENT_TYPE_ABANDONED = EVENT_TYPE_CLOSE,

        /* 以下定义是对上述类型的分类和汇总，方便代码的编写 */
        EVENT_TYPE_ACTIVE = EVENT_TYPE_READ | EVENT_TYPE_WRITE,

        EVENT_TYPE_IN = EVENT_TYPE_READ | EVENT_TYPE_ACCEPT,
        EVENT_TYPE_OUT = EVENT_TYPE_WRITE | EVENT_TYPE_CONNECT,
        EVENT_TYPE_ERR = EVENT_TYPE_CLOSE | EVENT_TYPE_EXCEPTION,

        EVENT_TYPE_ALL = EVENT_TYPE_IN | EVENT_TYPE_OUT | EVENT_TYPE_ERR,
    };

public:
    CMEReactor( IMEReactorImpl* pReactorImpl );

    ~CMEReactor();

public:
    /**
     *  运行分离器，分离器有2种运行模式，心跳模式和循环模式，分别对应于线程对象
     *  的阻塞模式和非阻塞模式
     *
     *  <iRunMode>, 指定线程对象的运行模式，默认是循环模式
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     *
     *  参考:
     *      RUN_MODE::RUN_MODE_LOOP
     *      RUN_MODE::RUN_MODE_HEART_BEAT
     */
    ME_Result Run( INT iRunMode = RUN_MODE_LOOP );

    /**
     *  通知分离器跳出等待，一般有两种情况，当分离器运行过程中有自定义事件投递到
     *  事件队列或是添加/取消时钟，那么需要通知分离器从当前阻塞等待事件中跳出，
     *  立即通知事件队列或是时钟队列检查自定义事件或是时钟事件
     *
     *  <iNotifyType>, 通知类型
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    ME_Result Notify( INT iNotifyType );

    /**
     *  注册感兴趣的IO对象，IO事件及事件处理回调到分离器；并且，能够通过这个接口
     *  更新当前已注册的IO对象的IO事件
     *
     *  <pIO>, 关注的IO对象实例指针
     *  <pIOSink>, IO事件回调实例指针
     *  <iEvent>, 关注的IO事件类型
     *  <iIndex>, 如果是注册新的IO对象，那么必须使用-1，注册成功将会输出IO对象在
     *            分离器中的索引；如果是修改已注册IO对象的事件类型，那么就必须输
     *            入已注册IO对象在分离器中的索引，否则可能会失败或引起不可预期的
     *            问题
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    ME_Result Register( 
        IMEIO* pIO,
        IMEIOSink* pIOSink,
        INT iEvent,
        INT& iIndex );

    /**
     *  注销已注册的IO对象的IO事件，允许部分注销已注册的IO事件
     *
     *  <pIO>, 关注的IO对象实例指针
     *  <iEvent>, 所要注销的IO事件类型
     *  <iIndex>, 必须输入已注册IO对象在分离器中的索引，否则可能会失败或引起不可
     *            预期的问题
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    ME_Result UnRegister(
        IMEIO* pIO,
        INT iEvent,
        INT iIndex );

    /**
     *  附加分离器到事件队列，目前，一个分离器只能够附加到一个事件队列，以第一次
     *  附加成功为准
     *
     *  <pEq>, 所要附加的事件队列
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    ME_Result AttachEventQueue( CMEEventQueue* pEq );

    /**
     *  附加分离器到时钟队列，目前，一个分离器只能够附加到一个时钟队列，以第一次
     *  附加成功为准
     *
     *  <pTq>, 所要附加的时钟队列
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    ME_Result AttachTimerQueue( CMETimerQueue* pTq );

	/* 2013.9.4 added by 韦珂 */
	/**
	* 获取Reactor的类型
	*
	* 返回值:
	*	int型，值出自REACTOR_TYPE
	*/
	int GetType() const;

private:
    friend class CMEReactorFactory;

    IMEReactorImpl*                m_pReactorImpl; // Reactor真正的实现依托类

    CMELockThreadMutex             m_hThreadMutex; // 保护Reactor的Run过程
};


ME_NAME_SPACE_END

#endif    // __REACTOR_H_











