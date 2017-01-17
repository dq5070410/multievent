/**
 *  线程对象及线程对象管理器接口
 *
 *
 *  History
 *  01/28/2016 create file
 */

#ifndef __THREAD_INTERFACE_H_
#define __THREAD_INTERFACE_H_

#include "Base.h"

NAME_SPACE_BEGIN

/* 预声明 */
class IMEThread;
class IMEThreadManager;
class IMEThreadSink;
class ICustomizeEvent;


/**
 *  线程对象类
 *
 *  MultiEvent对外提供的线程对象类，限制接口后仅保留事件投递以及获取线程对象基本
 *  信息的接口
 *
 */
class ME_OS_EXPORT IMEThread
{
public:
    /**
     *  运行线程对象，注意，如果是非阻塞模式的线程对象，则不需要额外调用此接口，
     *  线程对象管理器在创建线程对象后会自动启动线程对象；如果是阻塞模式的线程
     *  对象，那么就需要不断调用Run让线程对象不断运行
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     *
     *  参考:
     *      IMEThreadManager::CreateThread
     */
    virtual ME_Result Run() = 0;

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
    virtual ME_Result PostEvent( ICustomizeEvent* pEvent ) = 0;

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
    virtual ME_Result SendEvent( ICustomizeEvent* pEvent ) = 0;

    /**
     *  获得线程对象ID，获取线程对象在MultiEvent组件中的ID，注意，因为它不是操作
     *  系统中的线程ID，所以不能尝试调用操作系统的线程管理接口来操作这个ID
     *
     *  返回值:
     *      >= 1，线程对象ID
     */
    virtual INT GetThreadID() const = 0;

    /**
     *  获得线程对象类型，获取线程对象的类型，这个类型和创建时指定的类型相同
     *
     *  返回值:
     *      线程对象类型
     *
     *  参考:
     *      IMEThreadManager::THREAD_OPTION
     */
    virtual INT GetThreadType() const = 0;

protected:
    /**
     *  析构函数，被声明为受保护的，如果要销毁线程对象，就必须使用线程对象管理器
     *  对应的操作来执行，不能够调用delete来销毁线程对象
     *
     *  返回值:
     *      void
     *
     *  参考:
     *      IMEThreadManager::DestroyThread
     */
    virtual ~IMEThread() {}
};

/**
 *  线程对象用户回调类
 *
 *  如果希望线程对象在启动后或者退出前执行自己定义的逻辑，那么就继承它并实现
 *  OnStart和OnStop接口
 *
 *  注意:
 *      如果是创建用户逻辑类型的线程对象，必须在创建时指定这样一个回调类给它
 *
 *  参考:
 *      IMEThreadManager::CreateThread
 */
class  IMEThreadSink
{
public:
    virtual ~IMEThreadSink() {}

    /**
     *  线程对象启动后执行的过程，如果创建了用户逻辑类型的线程对象，那么就应该在
     *  这个接口实现需要线程对象来执行的用户逻辑
     *
     *  返回值:
     *      void
     */
    virtual void OnStart() = 0;

    /**
     *  线程对象停止前执行的过程，如果需要在线程退出前做一些善后工作，那么可以在
     *  这个接口中实现
     *
     *  返回值:
     *      void
     */
    virtual void OnStop() = 0;
};

/**
 *  线程对象管理器
 *
 *  管理组件中的线程对象，提供线程对象的创建（创建即运行），停止，销毁功能，并且
 *  只能通过线程对象管理器来做这些事情
 *
 */
class ME_OS_EXPORT IMEThreadManager
{
public:
    /**
     *  线程对象创建选项，线程对象能够具有4种基本的特性，但并非这4种特性都能独立
     *  存在，而由这4种基本特性组合还能够衍生出具有复合特性的线程对象，仅枚举存
     *  在的组合形式，不在枚举范围内的组合都不能创建出对应的线程对象
     *  
     */
    enum THREAD_OPTION
    {
        /* 具有用户逻辑，可创建仅包含该特性的线程对象 */
        ME_THREAD_CUSTOM_LOGIC = (1 << 0),

        /* 具有自定义事件队列，可创建仅包含该特性的线程对象 */
        ME_THREAD_EVENT_QUEUE = (1 << 1),

        /* 具有时钟队列，不可创建仅包含该特性的线程对象 */
        ME_THREAD_TIMER_QUEUE = (1 << 2),

        /* 具有事件多路分离器，不可创建仅包含该特性的线程对象 */
        ME_THREAD_REACTOR = (1 << 3),

        /* 具有事件队列和事件多路分离器，可创建该特性组合的线程对象 */
        ME_THREAD_EVENT_QUEUE_REACTOR = ME_THREAD_EVENT_QUEUE \
                                       | ME_THREAD_REACTOR,

        /* 具有时钟队列和事件多路分离器，可创建该特性组合的线程对象 */
        ME_THREAD_TIMER_QUEUE_REACTOR = ME_THREAD_TIMER_QUEUE \
                                       | ME_THREAD_REACTOR,

        /* 具有事件队列、时钟队列和事件多路分离器，可创建该特性组合的线程对象 */
        ME_THREAD_EVENT_QUEUE_TIMER_QUEUE_REACTOR = ME_THREAD_EVENT_QUEUE \
                                                   | ME_THREAD_TIMER_QUEUE \
                                                   | ME_THREAD_REACTOR,
    };

    /**
     *  获取线程对象管理器单例指针
     *
     *  返回值:
     *      线程对象管理器单例指针
     */
    static IMEThreadManager* Instance();

    /**
     *  创建线程对象，通过这个接口创建的线程对象能够直接接受线程对象管理器的管理
     *
     *  <bBlock>, 指出线程对象是否以阻塞模式运行
     *            阻塞模式：线程对象会在调用当前接口的线程环境首次运行，当一次工
     *            作周期完成后，将会返回，如果还需要线程对象继续执行，那么就需要
     *            不断的调用线程对象的Run接口
     *            非阻塞模式：线程对象会新建一个线程环境来执行线程逻辑，当启动后
     *            当前接口就会返回，这种情况也不需要另外再调用线程对象的Run接口
     *  <iType>, 必须指定线程对象的类型
     *  <pThreadSink>, 指定线程对象用户回调，如果创建用户逻辑类型的线程对象则必
     *                 须指定一个线程对象用户回调；创建其他类型的线程对象则不强
     *                 制要求
     *  <pThread>, 输出创建完成并且启动的线程对象
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result CreateThread(
        BOOL bBlock,
        INT iType,
        IMEThreadSink* pThreadSink,
        IMEThread*& pThread ) = 0;

    /**
     *  销毁线程对象，如果线程对象已注册到管理器，那么销毁前还会将其注销
     *  注意：无法销毁一个运行中的线程对象，在尝试销毁线程对象之前必须调用Join等
     *  待线程停止
     *
     *  <pThread>, 线程对象实例指针
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     *
     *  参考:
     *      IMEThreadManager::Join
     */
    virtual ME_Result DestroyThread( IMEThread* pThread ) = 0;

    /**
     *  等待线程对象停止
     *  注意：销毁线程对象之前，必须调用这个接口来等待线程对象停止
     *
     *  <pThread>, 线程对象实例指针
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     *
     *  参考:
     *      IMEThreadManager::DestroyThread
     */
    virtual ME_Result Join( IMEThread* pThread ) = 0;

    /**
     *  获得当前所在的线程对象
     *
     *  返回值:
     *      非空表示当前所在的线程对象实例指针; NULL表示没有找到
     */
    virtual IMEThread* GetCurrentThread() = 0;

    /**
     *  获得可用的网络线程对象
     *
     *  返回值:
     *      非空表示网络线程对象实例指针; NULL表示没有找到
     */
    virtual IMEThread* GetNetworkThread() = 0;

    /**
     *  获得可用的用户线程对象
     *
     *  返回值:
     *      非空表示用户线程对象实例指针; NULL表示没有找到
     */
    virtual IMEThread* GetUserThread() = 0;

protected:
    /**
     *  析构函数，被声明为受保护的，线程对象管理器单例只能由组件来销毁
     *
     *  返回值:
     *      无
     */
    virtual ~IMEThreadManager() {}
};

NAME_SPACE_END

#endif // __THREAD_INTERFACE_H_

