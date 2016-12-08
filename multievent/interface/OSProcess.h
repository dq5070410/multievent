/**
* 进程相关API
* 
*/

#ifndef __OS_PROCESS_H_
#define __OS_PROCESS_H_

#include "Base.h"

NAME_SPACE_OS_BEGIN

UINT OS_EXPORT GetCPUCount();

UINT OS_EXPORT GetProcessID();

const OS_EXPORT char* GetExecName();

const OS_EXPORT char* GetProcessName();

NAME_SPACE_OS_END

#endif // __OS_PROCESS_H_
