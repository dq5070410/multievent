/**
 *  线程对象管理器实现类声明
 *
 *  History
 *  01/29/2013 create file
 */

#ifndef __THREAD_MANAGER_H_
#define __THREAD_MANAGER_H_

#include "ThreadInterface.h"
#include "ThreadIDManager.h"
#include "LockThreadMutex.h"
#include "SingletonT.h"

ME_NAME_SPACE_BEGIN

class CMEThread;

/**
 *  线程对象管理器实现类
 *
 *  继承并实现IMEThreadManager的所有接口，并扩展仅面向组件内部使用的接口，如初
 *  始化，反初始化等
 *
 */
class CMEThreadManager : public IMEThreadManager
{
public:
    CMEThreadManager();
    virtual ~CMEThreadManager();

    virtual ME_Result CreateThread(
        BOOL bBlock,
        INT iType,
        IMEThreadSink* pThreadSink,
        IMEThread*& pThread );

    virtual ME_Result DestroyThread( IMEThread* pThread );

    virtual ME_Result Join( IMEThread* pThread );

    virtual IMEThread* GetCurrentThread();

    virtual IMEThread* GetNetworkThread();

    virtual IMEThread* GetUserThread();

    /**
     *  初始化，根据传入的线程对象回调和指定模式创建一个用户线程
     *
     *  注意：这是组件内部的接口，我们需要保证在一个进程环境中只调用一次这个接口
     *
     *  <pThreadSink>, 指定给线程对象的用户回调，默认为NULL
     *  <bBlockMode>, 指定线程对象的运行模式，默认为FALSE，即非阻塞模式
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    ME_Result Initialize(
        IMEThreadSink* pThreadSink = NULL,
        BOOL bBlockMode = FALSE );

    /**
     *  反初始化，在这个过程中会清理所有的线程对象，包括组件用户创建的但没有销毁
     *  的线程对象，以及初始化时创建的网络线程对象和用户线程对象
     *
     *  注意：这是组件内部的接口，我们需要保证在调用这个接口之前已经调用JoinAll
     *        停止了所有线程对象
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    ME_Result Finalize();

    /**
     *  根据制定的网络和用户线程数量创建指定数量和类型的线程对象，这些线程对象默
     *  认都是非阻塞的，并且不带有用户回调
     *
     *  注意：目前只允许创建一个用户线程对象，如果在Initialize期间创建了用户线程
     *  对象那么这里将不再创建用户线程对象；这个接口同样不能够多次调用
     *
     *
     *  <iNetworkThreadNum>, 网络线程对象数量
     *  <iUserThreadNum>, 用户线程对象数量，默认值1
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    ME_Result CreateThread(
        INT iNetworkThreadNum,
        INT iUserThreadNum = 1 );

    /**
     *  创建线程对象，通过这个接口创建的线程对象不会接受线程对象管理器的管理，如
     *  果需要线程对象管理器接管线程对象，那么需要调用Register
     *
     *  <bBlock>, 指出线程对象是否以阻塞模式运行
     *            阻塞模式：线程对象会在调用当前接口的线程环境首次运行，当一次工
     *            作周期完成后，将会返回，如果还需要线程对象继续执行，那么就需要
     *            不断的调用线程对象的Run接口
     *            非阻塞模式：线程对象会新建一个线程环境来执行线程逻辑，当启动后
     *            当前接口就会返回，这种情况也不需要另外再调用线程对象的Run接口
     *  <iThreadType>, 必须指定线程对象的类型
     *  <iClassificationType>, 线程对象划分类型
     *  <pThreadSink>, 指定线程对象用户回调，如果创建用户逻辑类型的线程对象则必
     *                 须指定一个线程对象用户回调；创建其他类型的线程对象则不强
     *                 制要求
     *  <pThread>, 输出创建完成并且启动的线程对象
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     *
     *  参考:
     *      Register
     */
    ME_Result CreateThread(
        BOOL bBlock,
        INT iThreadType,
        INT iClassificationType,
        IMEThreadSink* pThreadSink,
        CMEThread*& pThread );

    /**
     *  销毁线程对象
     *
     *  <pThread>, 线程对象实例指针
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    ME_Result DestroyThread( CMEThread* pThread );

    /**
     *  注册，将线程对象注册给管理器，由管理器来管理线程对象
     *
     *  <pThread>, 线程对象实例指针
     *
     *  返回值:
     *      
     */
    ME_Result Register( CMEThread* pThread );

    /**
     *  注销，将线程对象从管理器中注销，注销后的线程对象将无法通过管理器来管理
     *
     *  <pThread>, 线程对象实例指针
     *
     *  返回值:
     *      ME_ERROR_NOT_IMPLEMENTED表示当前未实现
     */
    ME_Result Unregister( CMEThread* pThread );

    /**
     *  等待线程停止，能够指定线程对象的ID以及等待超时时间，需要注意的是，如果等
     *  待超时，将会强制终止线程对象
     *
     *  <pThread>, 线程对象实例指针
     *  <iTimeout>, 等待超时时间
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    ME_Result Join(
        CMEThread* pThread,
        UINT iTimeout );

    /**
     *  阻塞等待所有线程对象停止，这个过程会阻塞等待所有的线程对象停止，包括组件
     *  用户创建的外部线程对象，初始化创建的内部线程对象（网络线程对象和用户线程
     *  对象）
     *
     *  <iTimeout>, 等待超时时间，默认ME_INFINITE，即永不超时
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    ME_Result JoinAll( UINT iTimeout = ME_INFINITE );

    /**
     *  通过线程对象的归类来获取线程对象
     *
     *  <iClassification>, 线程归类
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    IMEThread* GetThread( INT iClassification );

private:
    /* 线程对象分类ID-线程对象实例指针表 */
    typedef std::map<INT, CMEThread*> ThreadMapType;

private:
    /* 线程对象ID管理器 */
    CMEThreadIDManager             m_ThreadIDManager;

    /* 线程对象表 */
    ThreadMapType                   m_mapThreads;

    /* 线程对象管理器线程安全锁 */
    CMELockThreadMutex             m_hThreadMutex;

    /* 线程对象管理器是否允许注册注销操作标识 */
    BOOL                            m_bEnableRegister;
};

typedef CMESingletonT<CMEThreadManager> CMEThreadManagerSingleton;

ME_NAME_SPACE_END

#endif // __THREAD_MANAGER_H_

