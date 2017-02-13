/**
* Condition variable API
* 
* OSConditionVariable.h
*
*/

#ifndef __OS_ME_CONDITION_VARIABLE_H_
#define __OS_ME_CONDITION_VARIABLE_H_

#include "Base.h"

ME_NAME_SPACE_BEGIN
class CTimeValue;
ME_NAME_SPACE_END

ME_NAME_SPACE_OS_BEGIN

/**create condition variable
*
* <pConditionVariable>,out,if success,it not be NULL 
* <pszName>��condition variable name
*
* return value��
*		ME_OK is success��other is error
*/
ME_Result ME_OS_EXPORT ConditionVariableCreate( 
	ME_CONDITION_VARIABLE* pConditionVariable,
	const char* pszName );

/**
* ɾ��һ����������
*
* <pConditionVariable>��Ҫɾ���ĸ���������
*
* ����ֵ��
*		ME_OKΪ�ɹ�������ʧ��
*/
ME_Result ME_OS_EXPORT ConditionVariableDestroy( ME_CONDITION_VARIABLE* pConditionVariable );

/**
* ʹ������������һ��signal
*
* <pConditionVariable>����һ����������
*
* ����ֵ��
*		ME_OKΪ�ɹ�������ʧ��
*/
ME_Result ME_OS_EXPORT ConditionVariableSignal( ME_CONDITION_VARIABLE* pConditionVariable );

/**
* ʹ���������㲥
*
* <pConditionVariable>����һ����������
*
* ����ֵ��
*		ME_OKΪ�ɹ�������ʧ��
*/
ME_Result ME_OS_EXPORT ConditionVariableBroadcast( ME_CONDITION_VARIABLE* pConditionVariable );

/**
* �ȴ������������ź�
*
* �������������
*
* <pConditionVariable>����һ����������
* <pMutex>�����������ı�����
* <pTimeout>���ȴ���ʱʱ��
*
* ����ֵ��
*		ME_OKΪ�ɹ�������ʧ��
*/
ME_Result ME_OS_EXPORT ConditionVariableWait( 
	ME_CONDITION_VARIABLE* pConditionVariable, 
	THREAD_MUTEX* pMutex,
	CTimeValue* pTimeout = NULL );

ME_NAME_SPACE_OS_END

#endif	// __OS_ME_CONDITION_VARIABLE_H_

