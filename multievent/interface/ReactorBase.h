/**
 *  事件多路分离器实现基础类声明
 *
 *
 *  History
 *  01/26/2016          create file
 */

#ifndef __REACTOR_BASE_H_
#define __REACTOR_BASE_H_

#include "Base.h"
#include "ReactorImpl.h"
#include "LockThreadMutex.h"

ME_NAME_SPACE_BEGIN

class IMEWakener;

/**
 *  事件多路分离器实现基础类
 *
 *  继承事件多路分离器实现接口，实现事件多路分离器公共的部分，仅留下事件多路分离
 *  部分以及IO对象的部分注册和注销过程由不同的实现完成
 *
 */
class CMEReactorBase : public IMEReactorImpl
{
public:
    virtual ME_Result Notify( INT iNotifyType );

    virtual ME_Result Register(
        IMEIO* pIO,
        IMEIOSink* pIOSink,
        INT iEvent,
        INT& iIndex );

    virtual ME_Result UnRegister(
        IMEIO* pIO,
        INT iEvent,
        INT iIndex );

    virtual ME_Result AttachEventQueue( CMEEventQueue* pEq );

    virtual ME_Result AttachTimerQueue( CMETimerQueue* pTq );

    virtual ME_Result Dispatch( BOOL bExit = FALSE );

	/* 2013.9.4 added by 韦珂 */
	virtual int GetType() const;

protected:
    /**
     *  构造函数
     *
     *  <pWakener>, Reactor的唤醒者
     *  <iType>, Reactor的类型
     *
     */
    CMEReactorBase(
        IMEWakener* pWakener,
        INT iType );

    /**
     *  获取多路分离时的等待超时时间，这个时间会根据事件队列是否存在事件以及时钟
     *  队列最近到期时钟的情况来决定
     *
     *  返回值:
     *      >=0表示等待的时间的具体毫秒数; ME_INFINITE表示等待时间为无限长
     */
    UINT GetWaitTime();

    /**
     *  添加可等待对象，这个过程由具体的分离机制实现类实现，将IO对象转换为它们
     *  能够等待的对象
     *
     *  <pIO>, IO对象
     *  <iEvent>, 关注的事件类型
     *  <iIndex>, IO对象在Reactor的索引编号
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result AddWaitableObject(
        IMEIO* pIO,
        INT iEvent,
        INT iIndex ) = 0;

    /**
     *  移除可等待对象，这个过程由具体的分离机制实现类实现，移除IO对象对应的可等
     *  待对象
     *
     *  <pIO>, IO对象
     *  <iEvent>, 关注的事件类型
     *  <iIndex>, IO对象在Reactor的索引编号
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result RemoveWaitableObject(
        IMEIO* pIO,
        INT iEvent,
        INT iIndex ) = 0;

    // 禁止直接delete CMEReactorBase对象
    virtual ~CMEReactorBase();

private:
    /**
     *  判断分离器是否还有空间用于注册IO对象事件
     *
     *  返回值:
     *      TRUE表示分离器注册空间已满; FALSE表示分离器还有空间用于注册
     */
    BOOL IsFull();

    /* 禁止默认构造函数 */
    CMEReactorBase();

    /* 禁止拷贝 */
    CMEReactorBase( const CMEReactorBase& );
    const CMEReactorBase& operator = ( const CMEReactorBase& );

	// 2013.4.11 edited by 韦珂
//private:
protected:
    /*
     *  IOItem，用来存储注册进来的IO对象，关注的IO事件类型以及IO事件处理回调，当
     *  然，还能够标识出捕获后触发了什么事件
     */ 
    typedef struct tagIOItem
    {
        IMEIO* pIO;                // 受关注的IO对象实例指针
        IMEIOSink* pIOSink;        // 受关注的IO对象事件回调实例指针
        INT iEvent;                 // 受关注的事件类型
        INT iActiveEvent;           // 捕获过程中实际发生的事件类型
    } ME_IO_ITEM;

    /* Index-IOItem Map，用来快速索引IO Item */
    typedef std::map<INT, ME_IO_ITEM> HikIOItemMap;

protected:
    /* 需要由分离器工厂来销毁Wakener */
    friend class CMEReactorFactory;

    /* 捕获到IO事件的IO对象数量 */
    INT                             m_iActiveIOItemNum;

    /* 存放注册的IO对象，IO事件类型以及IO事件回调的Map表 */
    HikIOItemMap                    m_ioItemMap;

    /* 分离器的唤醒者实例指针 */
    IMEWakener*                    m_pWakener;

    /* 分离器的线程安全锁 */
    CMELockThreadMutex             m_hThreadMutex;

private:
    /* 新的IO对象索引，如果有新的IO对象注册进来，那么会用它来产生IO对象索引 */
    INT                             m_iNewIndex;

    /* 分离器的状态 */
    INT                             m_iStatus;

    /* 分离器的类型 */
    INT                             m_iType;

    /* 分离器附加的事件队列 */
    CMEEventQueue*                 m_pEventQueue;

    /* 分离器附加的时钟队列 */
    CMETimerQueue*                 m_pTimerQueue;
};

ME_NAME_SPACE_END

#endif // __REACTOR_BASE_H_

