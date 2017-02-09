/**
* 条件变量的实现类
*
* 目前只在linux环境下起作用，win32环境暂未实现
* 
*
* History
*	2/18/2016		Created
*/

#ifndef __LOCK_CONDITION_VARIABLE_H_
#define __LOCK_CONDITION_VARIABLE_H_

#include "Base.h"
#include "AtomicOperationT.h"

ME_NAME_SPACE_BEGIN

class CTimeValue;

/**
* 条件变量的实现类
*
* 目前只在linux环境下起作用，win32环境暂未实现
*/
class ME_OS_EXPORT CMELockConditionVariable
{
public:
	explicit CMELockConditionVariable( 
		const char* pszName = NULL,
		BOOL bActAsEvent = FALSE );

	~CMELockConditionVariable();

public:
	/**
	* 锁定条件变量本身
	*
	* 这个方法会阻塞
	*
	* 返回值：
	*		ME_OK为成功；其他失败
	*/
	ME_Result Lock();

	/**
	* 解除锁定条件变量
	*
	* 返回值：
	*		ME_OK为成功；其他失败
	*/
	ME_Result UnLock();

	/**
	* 发出一个信号
	*
	* 这个函数会发出一个信号并且唤醒一个线程并处理这个信号
	* 例如，你有三个线程在等待这个条件变量有信号，这个函数会
	* 唤醒其中一条并让它处理这个信号，“唤醒”哪一条是由系统决定
	*
	*/
	ME_Result Signal();

	/**
	* 广播信号
	*
	* 这个函数会以广播的形式唤醒所有等待这个条件变量的线程
	*/
	ME_Result Broadcast();

	/**
	* 等待当前的条件变量有信号
	*
	* 这个函数会阻塞
	*
	* <pTimerout>，等待多长时间
	*
	* 返回值：
	*		ME_OK为成功；ME_ERROR_TIMEOUT为超时；其他失败
	*/
	ME_Result Wait( CTimeValue* pTimeout = NULL );

private:
	void operator = ( const CMELockConditionVariable& );
	CMELockConditionVariable( const CMELockConditionVariable& );

private:
	ME_CONDITION_VARIABLE				m_ConditionVariable;
	ME_THREAD_MUTEX					m_Mutex;

	/**
	* 条件变量在signal之后再wait，是wait不出来的，因为信号不会存储，或者说
	* signal不是一个状态(状态在没有被重置之前应该一直存在)，但为了使条件变量
	* 能够像win32的event那样工作 -- CMELockEvent中在linux环境下是使用条件变量
	* 来实现，因此，需要用一个计数来记录条件变量有没有signal或broadcast过，
	* 以便于解决上边所说，wait在signal之前的无响应的问题
	*/
	CMEAtomicOperationT<CMELockMutex>	m_Signal;

	/**
	* 用于标识是否需要条件变量像event一样工作
	*
	* 如果<m_bActAsEvent>等于FALSE，那么，条件变量就本色出演 -- <m_Signal>将不会起任何作用
	* 如果等于TRUE，<m_Signal>起作用，条件变量就像win32的event那样的工作
	*/
	BOOL								m_bActAsEvent;
};

ME_NAME_SPACE_END

#endif	// __LOCK_CONDITION_VARIABLE_H_


