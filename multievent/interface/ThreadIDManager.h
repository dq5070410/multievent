/**
 *  线程对象ID管理器声明
 *
 *  History
 *  01/29/2016 create file
 */

#ifndef __THREAD_ID_MANAGER_H_
#define __THREAD_ID_MANAGER_H_

#include "Base.h"
#include "AtomicOperationT.h"

ME_NAME_SPACE_BEGIN

/**
 *  线程对象ID管理器
 *
 *  用于生成线程对象的ID，不同的线程对象分类采用不同的ID分段
 *
 */
class CMEThreadIDManager
{
public:
    /**
     *  对线程对象进行归类，划分方式采用ID分段的方式，其中网络线程和用户线程都属
     *  于组件的内部线程，外部线程指组件用户通过线程对象管理器创建的线程对象
     */
    enum ME_THREAD_ID_CLASSIFICATION
    {
        /* 网络线程的ID范围是[1, 100] */
        ME_NETWORK_THREAD_ID_BEGIN = 1,

        /* 用户线程的ID范围是[101, 2000] */
        ME_USER_THREAD_ID_BEGIN = 101,

        /* 外部线程的ID范围是[2001, +∞] */
        ME_EXTERNAL_THREAD_ID_BEGIN = 2001,
    };

    enum CLASSIFICATION_TYPE
    {
        /* 网络线程 */
        CLASSIFICATION_TYPE_NETWORK = 1,

        /* 用户线程 */
        CLASSIFICATION_TYPE_USER = 2,

        /* 外部线程 */
        CLASSIFICATION_TYPE_EXTERNAL = 3,
    };

    CMEThreadIDManager();
    ~CMEThreadIDManager();

    /**
     *  根据线程对象的划分类型生成对应的线程ID
     *
     *  <iClassificationType>, 线程对象划分类型
     *
     *  返回值:
     *      1-100表示网络线程对象ID
     *      101-2000表示用户线程对象ID
     *      >=2001表示外部线程对象ID
     *      0表示当前划分类型的线程对象已经没有可用的ID
     */
    INT GenerateThreadID( INT iClassificationType );

    /**
     *  判断线程对象是否为外部线程
     *
     *  <iThreadID>, 要判断的线程对象ID
     *
     *  返回值:
     *      TRUE表示外部线程对象; FALSE表示内部线程对象
     */
    BOOL IsExternalThread( INT iThreadID );

    /**
     *  获取线程对象的划分类型
     *
     *  <iThreadID>, 要判断的线程对象ID
     *
     *  返回值:
     *      CLASSIFICATION_TYPE枚举中的一种
     */
    INT GetThreadClassificationType( INT iThreadID );

private:
    CMEAtomicOperationT<CMELockMutex> m_iNewNetworkThreadID;
    CMEAtomicOperationT<CMELockMutex> m_iNewUserThreadID;
    CMEAtomicOperationT<CMELockMutex> m_iExternalThreadID;
};


ME_NAME_SPACE_END

#endif // __THREAD_ID_MANAGER_H_

