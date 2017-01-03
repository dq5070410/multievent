/**
 *  ����Socket Pair�Ļ�����������
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
 *  ����Socket Pair�Ļ�������
 *
 *  ��Ӧ��ʹ��select��ʽ�ķ�������CMEReactorSelect��������Ҳ֧��δ��epoll_wait
 *  �ȷ�ʽ�ķ�����
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

    /* ��������ͬһʱ����ֻ������һ���źţ��ź��յ������ٷ�һ�� */
    BOOL                            m_bSignalOn;
    CMELockThreadMutex             m_hThreadMutex;

    static const std::string        m_sStrLoopBackIP;
};

ME_NAME_SPACE_END

#endif // __WAKENER_SOCKET_PAIR_H_

