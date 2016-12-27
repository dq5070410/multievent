/**
* MultiEvent的基本头文件(把一些必须具备的头文件都include进来)
* 

*
* History
*	12/02/2016		Created
*/

#ifndef __BASE_H_
#define __BASE_H_

/**
* 由于以下include的.h有着大量的类型定义
* 所以最安全，也是最推荐的做法是，永远
* 把Base.h放在你头文件的第一行，切记
*/

#include "Platform.h"
#include "ErrorDefines.h"
#include "ErrorNetworkDefines.h"
#include "CommonInclude.h"
#include "ErrorNetworkSysDefines.h"
#include "UtilDefines.h"
#include "TypeDefines.h"
#include "Debug.h"
#include "TraceDefines.h"
#include "TraceFormator.h"
#include "Trace.h"

#endif	// __BASE_H_





