/**
* Condition variable API
* 
* OSConditionVariable.h
*
*/

#ifndef __OS_CONDITION_VARIABLE_H_
#define __OS_CONDITION_VARIABLE_H_

#include "Base.h"

NAME_SPACE_BEGIN
class CTimeValue;
NAME_SPACE_END

NAME_SPACE_OS_BEGIN

/**create condition variable
*
* <pConditionVariable>,out,if success,it not be NULL 
* <pszName>��condition variable name
*
* return value��
*		R_OK is success��other is error
*/
Result OS_EXPORT ConditionVariableCreate( 
	CONDITION_VARIABLE* pConditionVariable,
	const char* pszName );

/**
* ɾ��һ����������
*
* <pConditionVariable>��Ҫɾ���ĸ���������
*
* ����ֵ��
*		R_OKΪ�ɹ�������ʧ��
*/
Result OS_EXPORT ConditionVariableDestroy( CONDITION_VARIABLE* pConditionVariable );

/**
* ʹ������������һ��signal
*
* <pConditionVariable>����һ����������
*
* ����ֵ��
*		R_OKΪ�ɹ�������ʧ��
*/
Result OS_EXPORT ConditionVariableSignal( CONDITION_VARIABLE* pConditionVariable );

/**
* ʹ���������㲥
*
* <pConditionVariable>����һ����������
*
* ����ֵ��
*		R_OKΪ�ɹ�������ʧ��
*/
Result OS_EXPORT ConditionVariableBroadcast( CONDITION_VARIABLE* pConditionVariable );

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
*		R_OKΪ�ɹ�������ʧ��
*/
Result OS_EXPORT ConditionVariableWait( 
	CONDITION_VARIABLE* pConditionVariable, 
	THREAD_MUTEX* pMutex,
	CTimeValue* pTimeout = NULL );

NAME_SPACE_OS_END

#endif	// __OS_CONDITION_VARIABLE_H_

