/**
* MultiEvent配置管理器
*
* 用于设置一些运行属性，比如线程数限制
* 另外，它也用于创建一些整个MultiEvent框架初始化必要
* 的东西，比如用户/网络线程，Reactor等等
* 
*
* History
*	2/26/2016		Created
*/

#ifndef __CONFIG_MANAGER_H_
#define __CONFIG_MANAGER_H_

#include "Base.h"
#include "LockThreadMutex.h"
#include "SingletonT.h"

ME_NAME_SPACE_BEGIN

class IMEThreadSink;

class CConfigManager
{
public:
	ME_Result Initialize(
		IMEThreadSink* pUserThreadSink = NULL,
		BOOL bUserThreadWorkInBlockMode = FALSE );

	ME_Result UnInitialize();
};

typedef CSingletonT<CConfigManager> CConfigManagerSingleton;

ME_NAME_SPACE_END

#endif	// __ME_CONFIG_MANAGER_H_





