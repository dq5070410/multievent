#include "Trace.h"
#include "TraceDefines.h"
#include "TraceFormator.h"
#include "OSProcess.h"
#include "OSThread.h"
#include "TimeValue.h"
#include "SingletonT.h"
#include "LockThreadMutex.h"
#include "LockGuardT.h"
//#include "../dependency/hlog/h/hlog.h"



ME_NAME_SPACE_BEGIN

static HLOG_LEVEL s_hlLevels[] = 
{
	HLOG_LEVEL_INFO,
	HLOG_LEVEL_WARN,
	HLOG_LEVEL_ERROR,
};

static const char* ME_UTILITY_TRACE_LEVEL[] =
{
	"[INFO]",
	"[WARNING]",
	"[ERROR]"
};

CMETrace* CMETrace::Instance()
{
	return CMESingletonT<CMETrace>::Instance();
}

CMETrace::CMETrace()
	: m_pLock( new CMELockThreadMutex )
{

}

CMETrace::~CMETrace()
{
	ME_DELETE( m_pLock );
}

#ifdef ME_WIN

void CMETrace::Output( 
	int iLevel, 
	const char* pszData )
{
	CMELockGuardT<CMELockThreadMutex> Guard( m_pLock );

	char szBuf[ME_TRACE_MAX_TRACE_LEN + 1];
	::memset(
		szBuf,
		0,
		ME_TRACE_MAX_TRACE_LEN + 1 );

	FormatStr( 
		szBuf, 
		ME_TRACE_MAX_TRACE_LEN + 1, 
		iLevel, 
		pszData );

	::OutputDebugStringA( szBuf );
}

#endif	// ME_WIN

void CMETrace::WriteLog( 
	int iLevel, 
	const char* pszData )
{
	CMELockGuardT<CMELockThreadMutex> Guard( m_pLock );

	char szBuf[ME_TRACE_MAX_TRACE_LEN + 1];
	::memset(
		szBuf,
		0,
		ME_TRACE_MAX_TRACE_LEN + 1 );

	FormatStr( 
		szBuf, 
		ME_TRACE_MAX_TRACE_LEN + 1, 
		iLevel, 
		pszData );

	//hlog_format( 
	//	s_hlLevels[iLevel], 
	//	"HikUtility", 
	//	szBuf );
		//OS::GetProcessName(), 
}

void CMETrace::FormatStr( 
	char* pBuffer, 
	int iBufferLength,
	int iLevel,
	const char* pszData )
{
#ifdef ME_WIN
	time_t timeVal;
	struct tm tmVar;

#else
	struct timeval timeVal;
	struct tm tmVar;

#endif	// ME_WIN

	CTimeValue tvCur = CTimeValue::TimeOfDay();

#ifdef ME_WIN
	timeVal = tvCur.GetSecond();
	::localtime_s( 
		&tmVar, 
		(const time_t*)&timeVal );

#else
	::gettimeofday(
		&timeVal, 
		NULL );

	::localtime_r(
		(const time_t*)&timeVal.tv_sec, 
		&tmVar );

#endif	// ME_WIN

	/* 格式化输出内容 */
	CMETraceFormator::Printf(
		pBuffer, 
		ME_TRACE_MAX_TRACE_LEN,
		ME_TRACE_FORMAT, 
		tmVar.tm_mon + 1,				// 月
		tmVar.tm_mday,					// 日
		tmVar.tm_year + 1900,			// 年
		tmVar.tm_hour,					// 小时
		tmVar.tm_min,					// 分钟
		tmVar.tm_sec,					// 秒
		tvCur.GetMicrosecond() / 1000,	// 毫秒
		OS::GetProcessID(),				// 进程ID
		OS::GetThreadID(),				// 线程ID
		ME_UTILITY_TRACE_LEVEL[iLevel],// 内容类别
		pszData );						// 内容
}

ME_NAME_SPACE_END








