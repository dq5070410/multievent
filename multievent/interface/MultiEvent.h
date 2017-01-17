/**
* MultiEvent��C�ӿ�
*
* ���κ�ʱ����ʹ��MultiEvent�������κζ���֮ǰ
* �㶼Ӧ�����ȵ������½ӿ�
* 
*
* History
*	12/26/2016		Created
*/

#ifndef __MULTI_EVENT_H_
#define __MULTI_EVENT_H_

#include "Base.h"
#include "ThreadInterface.h"

ME_NAME_SPACE_BEGIN

extern "C" 
{
	/**
	* ��ʼ��MultiEvent
	*
	* <pUserThreadSink>���û��̻߳ص�����
	* <bUserThreadWorkInBlockMode>���û��߳��Ƿ���Ҫ����������ģʽ
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������Ϊʧ��
	*/
	ME_Result ME_OS_EXPORT Initialize(
		IMEThreadSink* pUserThreadSink = NULL,
		BOOL bUserThreadWorkInBlockMode = FALSE );

	/**
	* ж��MultiEvent
	*
	* ����ֵ��
	*		ME_OKΪ�ɹ�������Ϊʧ��
	*/
	ME_Result ME_OS_EXPORT UnInitialize();

	/**
	* �����û��߳�(һ��)
	*
	* ���ҽ����û��̹߳���������ģʽ�£��������������
	*
	* ����ֵ��
	*		��
	*/
	void ME_OS_EXPORT Run();
}

ME_NAME_SPACE_END

#endif	// __MULTI_EVENT_H_



