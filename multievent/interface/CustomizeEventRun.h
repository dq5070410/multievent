/**
* �Զ����¼���һ��ʵ����, ���¼�Ͷ����Ҫ���ظ�����
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
* ���ڼ��¼�Ͷ�ݲ�������
*/
class CEventRun : public ICustomizeEvent
{
public:
	/*
	* �����¼�
	* 
	* <pThread>, ���ĸ��߳������¼�
	* <bSync>, �Ƿ���Ҫͬ��ִ���¼�
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��
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







