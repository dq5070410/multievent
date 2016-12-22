#include "OSProcess.h"

NAME_SPACE_OS_BEGIN

/* 命令行(包含所属路径)最大长度, 如"/sbin/dhclient -1 -q -cf /etc/dhclient-eth0.conf" */
const DWORD ME_OS_PROCESS_CMD_BUF_LEN	= 1024;

/* 进程名(包含所属路径)最大长度 */
const DWORD ME_OS_PROCESS_READ_BUF_LEN = 256;

/* 进程ID最大长度 */
const DWORD ME_OS_PROCESS_PID_MAX_LEN	= 64;

UINT GetCPUCount()
{
#ifdef ME_WIN
	SYSTEM_INFO siInfo;
	::GetSystemInfo( &siInfo );
	return siInfo.dwNumberOfProcessors;

#elif defined (ME_LINUX)
	return ::sysconf( _SC_NPROCESSORS_CONF );

#endif	// ME_WIN
}

UINT GetProcessID()
{
#ifdef ME_WIN
    return ::GetCurrentProcessId();

#elif defined( ME_LINUX )
    return ::getpid();

#endif // ME_WIN
}

const char* GetExecName()
{
	static char s_szCmdName[ME_OS_PROCESS_CMD_BUF_LEN];
	static char* s_pszCmdName = NULL;

	if ( !s_pszCmdName )
	{
		::memset(
			s_szCmdName,
			0,
			ME_OS_PROCESS_CMD_BUF_LEN );
	}
	
#ifndef ME_WIN
	if ( !s_pszCmdName )
	{
		char szPidFile[ME_OS_PROCESS_CMD_BUF_LEN];
		::memset( 
			szPidFile,
			0,
			ME_OS_PROCESS_CMD_BUF_LEN );

		/* 设置进程所在proc路径 */
		::sprintf(
			szPidFile, 
			"/proc/%d", 
			GetProcessID() );

		char szCurrentPath[ME_OS_PROCESS_READ_BUF_LEN];
		::memset(
			szCurrentPath,
			0,
			ME_OS_PROCESS_READ_BUF_LEN );

		char* pszPath = ::getcwd( 
			szCurrentPath, 
			ME_OS_PROCESS_READ_BUF_LEN );

		if( NULL == pszPath )
		{
			return NULL;
		}

		/* 将当前路径设为进程路径 */
		if( -1 != ::chdir(szPidFile) ) 
		{
			::memset( 
				szPidFile,
				0,
				ME_OS_PROCESS_CMD_BUF_LEN );

			//      域1   域2  域3  域4  域5  域6  域7   域8   域9 域10        域11
			// lrwxrwxrwx  1  root  root  0   10月  26  16:35  exe  ->   /sbin/dhclient

			/* 把shell命令执行结果的 "域11" 打印出来, 如上所示, 即命令行"/sbin/dhclient\n" */
			::snprintf(
				szPidFile,
				ME_OS_PROCESS_CMD_BUF_LEN, 
				"ls -l | grep exe | awk '{print $11}'" );


			/* 打开程序对应的pipe */
			FILE* pPipe = NULL;
			pPipe = ::popen(
				szPidFile,
				"r" );

			if( NULL == pPipe ) 
			{
				::chdir(szCurrentPath);
				return NULL; 
			}

			/* 读取shell命令执行结果到字符串<s_szCmdName>中 */
			if( NULL == ::fgets(s_szCmdName, ME_OS_PROCESS_CMD_BUF_LEN, pPipe) ) 
			{ 
				::pclose( pPipe );
				::chdir( szCurrentPath );

				return NULL;
			}

			::pclose( pPipe );     //popen开启的fd必须要pclose关闭 

			const char* pSeprate = "/";
			const char* pszFind = ::strstr( 
				s_szCmdName,
				pSeprate );

			if ( NULL == pszFind )
			{
				::memset( 
					szPidFile,
					0,
					ME_OS_PROCESS_CMD_BUF_LEN );

				//      域1   域2  域3  域4  域5  域6  域7   域8   域9 域10        域11
				// lrwxrwxrwx  1  root  root  0   10月  26  16:35  exe  ->   /sbin/dhclient

				/* 把shell命令执行结果的 "域11" 打印出来, 如上所示, 即命令行"/sbin/dhclient\n" */
				::snprintf(
					szPidFile,
					ME_OS_PROCESS_CMD_BUF_LEN, 
					"ls -l | grep exe | awk '{print $10}'" );

				/* 打开程序对应的pipe */
				pPipe = ::popen(
					szPidFile,
					"r" );

				if( NULL == pPipe ) 
				{
					::chdir( szCurrentPath );
					return NULL; 
				}

				/* 读取shell命令执行结果到字符串<s_szCmdName>中 */
				if( NULL == ::fgets(s_szCmdName, ME_OS_PROCESS_CMD_BUF_LEN, pPipe) ) 
				{ 
					::pclose( pPipe );
					::chdir( szCurrentPath );

					return NULL;
				}

				::pclose( pPipe );	//popen开启的fd必须要pclose关闭 
			} 

			/* 把'\n'从<s_szCmdName>中去掉 */
			DWORD dwLen = ::strnlen(
				s_szCmdName, 
				ME_OS_PROCESS_CMD_BUF_LEN);

			if (  (0 < dwLen) && (0x0A == s_szCmdName[dwLen - 1]) )
			{
				s_szCmdName[dwLen - 1] = 0x00;
			}					
			
			s_pszCmdName = s_szCmdName;
			::chdir(szCurrentPath);
		}
	}
#else
	if ( !s_pszCmdName ) 
	{
		::GetModuleFileNameA(
			NULL, 
			s_szCmdName, 
			sizeof(s_szCmdName) );

		s_pszCmdName = s_szCmdName;
	}

#endif // !SC_WIN

	return s_pszCmdName;
}

const char* GetProcessName()
{
	const char* pszExecName = GetExecName();

	if( pszExecName )
	{
		const char* pchFind = pszExecName + ::strnlen(pszExecName, ME_OS_PROCESS_CMD_BUF_LEN);

		while( (pchFind > pszExecName) && 
			   (ME_OS_SEPARATE != *pchFind) )
		{
			pchFind--;
		}

		if( ME_OS_SEPARATE == *pchFind )
		{
			pchFind++;
		}

		return pchFind;
	}

	return "UnknowProcess";
}

NAME_SPACE_OS_END
