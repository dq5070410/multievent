/**
* platform system
* 
*
* History
*	2016/11/30
*/

#ifndef __COMMON_INCLUDE_H_
#define __COMMON_INCLUDE_H_

#include "Platform.h"

#ifdef WIN

	/*set the max select handle is 2048,default is 64*/
	#ifdef FD_SETSIZE
		#undef FD_SETSIZE
	#endif	// FD_SETSIZE

	#define FD_SETSIZE 2048L
	#include <winsock2.h>

	#include <string.h>
	#include <direct.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <time.h>
	#include <limits.h>
	#include <stddef.h>
	#include <stdarg.h>
	#include <signal.h>
	#include <errno.h>
	#include <wchar.h>

	#include <crtdbg.h>
	#include <process.h>

	#include <WS2tcpip.h>
	#include <MSWSock.h>
#endif	// WIN

#ifdef LINUX
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
	#include <errno.h>
	#include <limits.h>
	#include <stdarg.h>
	#include <time.h>
	#include <signal.h>
	#include <sys/stat.h>
	#include <sys/fcntl.h>
	#include <pthread.h>
	#include <fcntl.h>
	#include <sys/types.h>
	#include <sys/ioctl.h>
	#include <sys/socket.h>
	#include <sys/epoll.h>
	#include <sys/time.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <ctype.h>
	#include <assert.h>
	#include <netinet/tcp.h>
	#include <semaphore.h>
	#include <dlfcn.h>
#endif	// LINUX

#include <fcntl.h>
#include <memory>
#include <utility>
#include <algorithm>
#include <utility>
#include <set> 
#include <map>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <string>

#endif	// __COMMON_INCLUDE_H_



