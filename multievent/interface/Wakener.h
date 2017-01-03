/**
 *  分离器唤醒者接口类
 *
 *
 *  History
 *  01/28/2016 create file
 */

#ifndef __WAKENER_H_
#define __WAKENER_H_

#include "IO.h"

ME_NAME_SPACE_BEGIN

/**
 *  分离器唤醒者类
 *
 *  用于在分离器等待的时候通知它跳出等待，当然，因为不同的分离器采用的多路分离机
 *  制不一样，唤醒者也需要根据所采用的多路分离机制而分别实现
 *
 */
class IMEWakener : public IMEIO
                  , public IMEIOSink
{
public:
    virtual ~IMEWakener() {}

    /**
     *  通知分离器跳出等待
     *
     *  返回值: 
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result Notify() = 0;

    /**
     *  初始化唤醒者
     *
     *  将唤醒者的资源（SocketPair或Windows事件对象等）创建工作集中在这里完成，
     *  这样比较容易获知这些资源是否创建成功
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result Initialize() = 0;

    /**
     *  反初始化唤醒者
     *
     *  将唤醒者的资源销毁工作集中在这里完成
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result Finalize() = 0;
};

ME_NAME_SPACE_END

#endif // __WAKENER_H_

