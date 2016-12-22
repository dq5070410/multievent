#include "OSProcess.h"

NAME_SPACE_OS_BEGIN

/* ������(��������·��)��󳤶�, ��"/sbin/dhclient -1 -q -cf /etc/dhclient-eth0.conf" */
const DWORD ME_OS_PROCESS_CMD_BUF_LEN	= 1024;

/* ������(��������·��)��󳤶� */
const DWORD ME_OS_PROCESS_READ_BUF_LEN = 256;

/* ����ID��󳤶� */
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

		/* ���ý�������proc·�� */
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

		/* ����ǰ·����Ϊ����·�� */
		if( -1 != ::chdir(szPidFile) ) 
		{
			::memset( 
				szPidFile,
				0,
				ME_OS_PROCESS_CMD_BUF_LEN );

			//      ��1   ��2  ��3  ��4  ��5  ��6  ��7   ��8   ��9 ��10        ��11
			// lrwxrwxrwx  1  root  root  0   10��  26  16:35  exe  ->   /sbin/dhclient

			/* ��shell����ִ�н���� "��11" ��ӡ����, ������ʾ, ��������"/sbin/dhclient\n" */
			::snprintf(
				szPidFile,
				ME_OS_PROCESS_CMD_BUF_LEN, 
				"ls -l | grep exe | awk '{print $11}'" );


			/* �򿪳����Ӧ��pipe */
			FILE* pPipe = NULL;
			pPipe = ::popen(
				szPidFile,
				"r" );

			if( NULL == pPipe ) 
			{
				::chdir(szCurrentPath);
				return NULL; 
			}

			/* ��ȡshell����ִ�н�����ַ���<s_szCmdName>�� */
			if( NULL == ::fgets(s_szCmdName, ME_OS_PROCESS_CMD_BUF_LEN, pPipe) ) 
			{ 
				::pclose( pPipe );
				::chdir( szCurrentPath );

				return NULL;
			}

			::pclose( pPipe );     //popen������fd����Ҫpclose�ر� 

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

				//      ��1   ��2  ��3  ��4  ��5  ��6  ��7   ��8   ��9 ��10        ��11
				// lrwxrwxrwx  1  root  root  0   10��  26  16:35  exe  ->   /sbin/dhclient

				/* ��shell����ִ�н���� "��11" ��ӡ����, ������ʾ, ��������"/sbin/dhclient\n" */
				::snprintf(
					szPidFile,
					ME_OS_PROCESS_CMD_BUF_LEN, 
					"ls -l | grep exe | awk '{print $10}'" );

				/* �򿪳����Ӧ��pipe */
				pPipe = ::popen(
					szPidFile,
					"r" );

				if( NULL == pPipe ) 
				{
					::chdir( szCurrentPath );
					return NULL; 
				}

				/* ��ȡshell����ִ�н�����ַ���<s_szCmdName>�� */
				if( NULL == ::fgets(s_szCmdName, ME_OS_PROCESS_CMD_BUF_LEN, pPipe) ) 
				{ 
					::pclose( pPipe );
					::chdir( szCurrentPath );

					return NULL;
				}

				::pclose( pPipe );	//popen������fd����Ҫpclose�ر� 
			} 

			/* ��'\n'��<s_szCmdName>��ȥ�� */
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
