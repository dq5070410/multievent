/**
* 自定义事件的一个实现类, 简化事件投递需要的重复代码
* 
*
* History
*	12/14/2016		Created
*/

#ifndef __CUSTOMIZE_EVENT_RUN_H_
#define __CUSTOMIZE_EVENT_RUN_H_

#include "Base.h"
#include "CustomizeEvent.h"

ME_NAME_SPACE_BEGIN

class IMEThread;

/*
* 用于简化事件投递操作的类
*/
class CEventRun : public ICustomizeEvent
{
public:
	/*
	* 运行事件
	* 
	* <pThread>, 在哪个线程运行事件
	* <bSync>, 是否需要同步执行事件
	*
	* 返回值:
	*		ME_OK为成功; 其他为失败
	*/
	ME_Result Run(
		IMEThread* pThread,
		BOOL bSync = FALSE );

protected:
	CEventRun();

	virtual ~CEventRun();
};

ME_NAME_SPACE_END


#endif	// __CUSTOMIZE_EVENT_RUN_H_







