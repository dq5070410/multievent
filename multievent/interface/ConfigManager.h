/**
* MultiEvent���ù�����
*
* ��������һЩ�������ԣ������߳�������
* ���⣬��Ҳ���ڴ���һЩ����MultiEvent��ܳ�ʼ����Ҫ
* �Ķ����������û�/�����̣߳�Reactor�ȵ�
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





