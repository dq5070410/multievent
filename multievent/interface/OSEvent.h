/**
* Event��ز�����API
* 
*
* History
*	2/18/2016		Created
*/

#ifndef __OS_EVENT_H_
#define __OS_EVENT_H_

#include "Base.h"
#include "TimeValue.h"

NAME_SPACE_OS_BEGIN

/**
* ����һ���¼�
*
* <pEvent>��out���ɹ�Ӧ��Ϊ��NULL
* <pszName>���¼�����
* <bManualReset>���Ƿ���Ҫ�ֶ�����״̬
* <bSignaled>���Ƿ����ź�
*
* ����ֵ��
*		ME_OKΪ�ɹ�������ʧ��
*/
ME_Result ME_OS_EXPORT EventCreate(
	ME_EVENT& pEvent,
	const char* pszName = NULL,
	BOOL bManualReset = FALSE,
	BOOL bSignaled = FALSE );

/**
* ����һ���¼�
*
* <pEvent>���ĸ��¼���Ҫ����
*
* ����ֵ��
*		ME_OKΪ�ɹ�������ʧ��
*/
ME_Result ME_OS_EXPORT EventDestroy( ME_EVENT& pEvent );

/**
* ����һ���¼�
*
* <pEvent>���ĸ��¼���Ҫ����
*
* ����ֵ��
*		ME_OKΪ�ɹ�������ʧ��
*/
ME_Result ME_OS_EXPORT EventWait( ME_EVENT pEvent );

/**
* �ȴ�һ���¼���Ϊ�����źš���״̬
*
* �������������
*
* <pEvent>���ĸ��¼�
* <htvTime>���ȴ���ʱʱ��
*
* ����ֵ��
*		ME_OKΪ�ɹ�������ʧ��
*/
ME_Result ME_OS_EXPORT EventTimeWait( 
	ME_EVENT pEvent,
	const CTimeValue& htvTime );

/**
* ��һ���¼���Ϊ�����źš���״̬
*
* <pEvent>���ĸ��¼�
*
* ����ֵ��
*		ME_OKΪ�ɹ�������ʧ��
*/
ME_Result ME_OS_EXPORT EventSignal( ME_EVENT pEvent );

/**
* ��һ���¼���Ϊ�����źš���״̬
*
* <pEvent>���ĸ��¼�
*
* ����ֵ��
*		ME_OKΪ�ɹ�������ʧ��
*/
ME_Result ME_OS_EXPORT EventReset( ME_EVENT pEvent );

NAME_SPACE_OS_END

#endif	// __OS_EVENT_H_
