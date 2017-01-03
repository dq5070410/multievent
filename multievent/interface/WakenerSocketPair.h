/**
 *  基于Socket Pair的唤醒者类声明
 *
 *
 *  History
 *  01/28/2016
 */

#ifndef __WAKENER_SOCKET_PAIR_H_
#define __WAKENER_SOCKET_PAIR_H_

#include "Wakener.h"
#include "LockThreadMutex.h"
#include "AsyncIoResult.h"

ME_NAME_SPACE_BEGIN

class CMESocketTcp;

/**
 *  基于Socket Pair的唤醒者类
 *
 *  对应于使用select方式的分离器：CMEReactorSelect，理论上也支持未来epoll_wait
 *  等方式的分离器
 *
 */
class CMEWakenerSocketPair 
	: public IMEWakener

#ifdef ME_LINUX
	, public CMEAsyncIoResult

#endif	// ME_LINUX
{
public:
    virtual void SetHandle( ME_HANDLE hHandle );

    virtual ME_HANDLE GetHandle() const;

    virtual void OnInput(
        ME_HANDLE hHandle,
        int iEvent );

    virtual void OnOutput(
        ME_HANDLE hHandle,
        int iEvent );

    virtual void OnClose(
        ME_HANDLE hHandle,
        int iEvent );

    virtual ME_Result Notify();

    virtual ME_Result Initialize();

    virtual ME_Result Finalize();

private:
    friend class CMEReactorFactory;
    friend class CMEWakenerSocketPairTester;

public:
    CMEWakenerSocketPair();
    virtual ~CMEWakenerSocketPair();

#ifdef ME_LINUX
	virtual void OnCompleteImpl( 
		BOOL bResult,
		int iActiveEvent,
		ME_HANDLE hInterface );

	virtual void OnDestroy();

#endif	// ME_LINUX

private:
    CMESocketTcp*                  m_pSocketTcpReader;
    CMESocketTcp*                  m_pSocketTcpWriter;

    /* 用来控制同一时间内只允许发出一个信号，信号收掉才能再发一个 */
    BOOL                            m_bSignalOn;
    CMELockThreadMutex             m_hThreadMutex;

    static const std::string        m_sStrLoopBackIP;
};

ME_NAME_SPACE_END

#endif // __WAKENER_SOCKET_PAIR_H_

