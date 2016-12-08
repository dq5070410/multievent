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
* <pszName>，condition variable name
*
* return value：
*		R_OK is success；other is error
*/
Result OS_EXPORT ConditionVariableCreate( 
	CONDITION_VARIABLE* pConditionVariable,
	const char* pszName );

/**
* 删除一个条件变量
*
* <pConditionVariable>，要删除哪个条件变量
*
* 返回值：
*		R_OK为成功；其他失败
*/
Result OS_EXPORT ConditionVariableDestroy( CONDITION_VARIABLE* pConditionVariable );

/**
* 使条件变量发出一个signal
*
* <pConditionVariable>，哪一个条件变量
*
* 返回值：
*		R_OK为成功；其他失败
*/
Result OS_EXPORT ConditionVariableSignal( CONDITION_VARIABLE* pConditionVariable );

/**
* 使条件变量广播
*
* <pConditionVariable>，哪一个条件变量
*
* 返回值：
*		R_OK为成功；其他失败
*/
Result OS_EXPORT ConditionVariableBroadcast( CONDITION_VARIABLE* pConditionVariable );

/**
* 等待条件变量有信号
*
* 这个方法会阻塞
*
* <pConditionVariable>，哪一个条件变量
* <pMutex>，条件变量的保护锁
* <pTimeout>，等待超时时间
*
* 返回值：
*		R_OK为成功；其他失败
*/
Result OS_EXPORT ConditionVariableWait( 
	CONDITION_VARIABLE* pConditionVariable, 
	THREAD_MUTEX* pMutex,
	CTimeValue* pTimeout = NULL );

NAME_SPACE_OS_END

#endif	// __OS_CONDITION_VARIABLE_H_

