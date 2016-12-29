/**
 *  用户自定义逻辑线程对象类声明
 *
 *  History
 *  01/29/2016 create file
 */

#ifndef __THREAD_CUSTOM_LOGIC_H_
#define __THREAD_CUSTOM_LOGIC_H_

#include "Thread.h"

ME_NAME_SPACE_BEGIN

/**
 *  用户自定义逻辑线程对象类
 *
 *  运行用户自定义逻辑的线程对象
 *
 */
class CMEThreadCustomLogic : public CMEThread
{
public:
    /**
     *  构造函数
     *
     *  <bBlock>, 指定线程是否为阻塞模式
     *  <iThreadID>, 指定线程对象的ID
     *  <iType>, 指定线程对象的类型
     *  <pThreadSink>, 指定一个用户通知回调（允许NULL）
     *
     *  返回值:
     *      无
     */
    CMEThreadCustomLogic(
        BOOL bBlock,
        INT iThreadID,
        INT iType,
        IMEThreadSink* pThreadSink );

    /**
     *  析构函数
     *
     *  返回值:
     *      无
     */
    virtual ~CMEThreadCustomLogic();

protected:
    virtual ME_THREAD_RETURN_TYPE Worker();
};

ME_NAME_SPACE_END

#endif // __ME_THREAD_CUSTOM_LOGIC_H_

