/**
 *  事件多路分离器实现类接口定义
 *
 *
 *  History
 *  01/26/2016          create file
 */


#ifndef __REACTOR_IMPL_H_
#define __REACTOR_IMPL_H_

#include "Base.h"
#include "EventQueueT.h"
#include "TimerQueueT.h"
#include "LockThreadMutex.h"

ME_NAME_SPACE_BEGIN

class IMEIO;
class IMEIOSink;
class IMECustomizeEvent;
class CMETimer;
class ITimerSink;


/**
 *  事件多路分离器实现类接口
 *
 *  定义事件多路分离器的实现接口，包括事件多路分离器的实现类都应该从这个类继承
 *
 *  note
 */
class IMEReactorImpl
{
public:
    typedef CMEEventQueueT<IMECustomizeEvent, CMELockThreadMutex> CMEEventQueue;
    typedef CMEEventQueue::NodeType::CMESyncEvent CMESyncEventType;
    typedef CMETimerQueueT<CMETimer, ITimerSink, CMELockThreadMutex> CMETimerQueue;

public:
    virtual ~IMEReactorImpl() {}

    /**
     *  通知分离器跳出等待，分离器通常使用操作系统提供的多路分离机制来捕获IO事件
     *  所以，分离器会在捕获时等待事件发生，如果这时需要告诉分离器有自定义事件或
     *  添加/取消了时钟，那么就需要调用这个接口让分离器及时从等待中唤醒，通知事
     *  件队列或时钟队列去检查和处理自定义事件或时钟事件；此外，在退出时，也需要
     *  通知分离器不必再等待，及时进入退出过程
     *
     *  <iNotifyType>, 通知类型：唤醒或停止
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     *
     *  参考:
     *      CMEReactor::NOTIFY_TYPE
     */
    virtual ME_Result Notify( INT iNotifyType ) = 0;

    /**
     *  注册关注的IO对象、IO事件类型以及事件处理回调，注册成功后，分离器将会负责
     *  对关注的IO对象的IO事件进行捕获，捕获到IO事件后，将会调用注册时传入的事件
     *  处理回调来通知关注者
     *
     *  需要注意的是，能够重复注册同一个IO对象，并且采用覆盖原值的方式修改已注册
     *  的IO事件类型，这里可能需要关注在高并发情况下的效率问题
     *
     *  <pIO>, 关注的IO对象
     *  <pIOSink>, IO对象的事件处理回调
     *  <iEvent>, 关注的IO事件
     *  <iIndex>, 输入IO对象在分离器的索引值，如果需要修改关注该IO对象的事件类型
     *            输出注册成功后此IO对象在分离器的索引值，在注销时需要提供该索引
     *
     *  返回值:
     *      ME_OK表示成功, 输出索引值>0; 非ME_OK表示失败, 输出索引值为-1
     *
     *  参考:
     *      UnRegister
     */
    virtual ME_Result Register(
        IMEIO* pIO,
        IMEIOSink* pIOSink,
        INT iEvent,
        INT& iIndex ) = 0;

    /**
     *  将已注册到分离器的IO对象和事件注销，当注销后已没有关注的事件类型，分离器
     *  将不再捕获被注销的IO对象的IO事件，当然也不会再调用事件处理回调
     *
     *  需要注意的是，注销是部分取消，即存在关注多个事件类型时，允许只注销其中一
     *  种而保留对其余事件类型的关注
     *
     *  <pIO>, IO对象
     *  <iEvent>, 需要注销的事件类型
     *  <iIndex>, IO对象在分离器中的索引，这个索引值在注册时获取
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     *
     *  参考:
     *      Register
     */
    virtual ME_Result UnRegister(
        IMEIO* pIO,
        INT iEvent,
        INT iIndex ) = 0;

    /**
     *  附加到事件队列，只允许附加一次，第一次成功后，后面再尝试该操作都会返回
     *  错误
     *
     *  <pEq>, 需要被分离器附加的事件队列
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result AttachEventQueue( CMEEventQueue* pEq ) = 0;

    /**
     *  附加到时钟队列，只允许附加一次，第一次成功后，后面再尝试该操作都会返回
     *  错误
     *
     *  <pTq>, 需要被分离器附加的时钟队列
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result AttachTimerQueue( CMETimerQueue* pTq ) = 0;

    /**
     *  捕获IO事件，采用操作系统提供的多路分离机制，如：select，epoll_wait，
     *  WaitForMultipleObjects等。
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result HandleEvent() = 0;

    /**
     *  分派事件，在通过HandleEvent捕获到事件以后（或者超时），通过Dispatch来分
     *  派事件，分派分两种情况：一种是运行状态时，应避免出现自定义事件过多而不能
     *  及时去捕获IO事件的情况，这时需要控制事件队列处理事件的最大数量；一种是退
     *  出状态时，IO捕获已经停止，这时就需要让时间队列将所有的自定义事件都逐一处
     *  理。 当然，处理事件的方式由事件队列来完成，分离器只负责在不同的情况下作出
     *  响应的通知。
     *
     *  <bExit>, 指明是否处于退出状态
     *
     *  返回值:
     *      RUN_STATE_RUNNING，表示分离器处于运行状态
     *      RUN_STATE_EXIT，表示检查到分离器处于退出状态
     */
    virtual ME_Result Dispatch( BOOL bExit = FALSE ) = 0;

	/* 2013.9.4 added by 韦珂 */
	virtual int GetType() const = 0;
};

ME_NAME_SPACE_END

#endif // __REACTOR_IMPL_H_

