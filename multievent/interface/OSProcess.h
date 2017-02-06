/**
* 进程相关API
* 
*/

#ifndef __OS_PROCESS_H_
#define __OS_PROCESS_H_

#include "Base.h"

ME_NAME_SPACE_OS_BEGIN

UINT ME_OS_EXPORT GetCPUCount();

UINT ME_OS_EXPORT GetProcessID();

const ME_OS_EXPORT char* GetExecName();

const ME_OS_EXPORT char* GetProcessName();

ME_NAME_SPACE_OS_END

#endif // __OS_PROCESS_H_
