/**
* �첽IO���͵�Reactor
*
* ʹ��win32����ɶ˿ڻ���ʵ��
* 
*
* History
*	9/07/2016		Created
*/
#ifndef __REACTOR_ASYCH_IOCP_H_
#define __REACTOR_ASYCH_IOCP_H_

#include "ReactorImpl.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

class CMEWakenerAsyncIOCP;

/**
* �첽IO���͵�Reactor
*
* ʹ��win32����ɶ˿ڻ���ʵ��
*/
class CMEReactorAsyncIOCP : public IMEReactorImpl
{
public:
	/**
	* һ������ֻ����һ��CMEReactorAsyncIOCPʵ���Ĵ���
	*
	* ����ֵ:
	*	CMEReactorAsyncIOCPָ��	
	*/
	static CMEReactorAsyncIOCP* Instance();

public:
	CMEReactorAsyncIOCP();

	CMEReactorAsyncIOCP( 
		CMEWakenerAsyncIOCP* pWakener,
		ME_HANDLE hInterface,
		DWORD dwThreadNum = 0 );

	virtual ~CMEReactorAsyncIOCP();

public:
	/**
     *  ֪ͨ�����������ȴ���������ͨ��ʹ�ò���ϵͳ�ṩ�Ķ�·�������������IO�¼�
     *  ���ԣ����������ڲ���ʱ�ȴ��¼������������ʱ��Ҫ���߷��������Զ����¼���
     *  ���/ȡ����ʱ�ӣ���ô����Ҫ��������ӿ��÷�������ʱ�ӵȴ��л��ѣ�֪ͨ��
     *  �����л�ʱ�Ӷ���ȥ���ʹ����Զ����¼���ʱ���¼������⣬���˳�ʱ��Ҳ��Ҫ
     *  ֪ͨ�����������ٵȴ�����ʱ�����˳�����
     *
     *  <iNotifyType>, ֪ͨ���ͣ����ѻ�ֹͣ
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     *
     *  �ο�:
     *      CMEReactor::NOTIFY_TYPE
     */
	virtual ME_Result Notify( INT iNotifyType );

    /**
     *  ע���ע��IO����IO�¼������Լ��¼�����ص���ע��ɹ��󣬷��������Ḻ��
     *  �Թ�ע��IO�����IO�¼����в��񣬲���IO�¼��󣬽������ע��ʱ������¼�
     *  ����ص���֪ͨ��ע��
     *
     *  ��Ҫע����ǣ��ܹ��ظ�ע��ͬһ��IO���󣬲��Ҳ��ø���ԭֵ�ķ�ʽ�޸���ע��
     *  ��IO�¼����ͣ����������Ҫ��ע�ڸ߲�������µ�Ч������
     *
     *  <pIO>, ��ע��IO����
     *  <pIOSink>, IO������¼�����ص�
     *  <iEvent>, ��ע��IO�¼�
     *  <iIndex>, ����IO�����ڷ�����������ֵ�������Ҫ�޸Ĺ�ע��IO������¼�����
     *            ���ע��ɹ����IO�����ڷ�����������ֵ����ע��ʱ��Ҫ�ṩ������
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�, �������ֵ>0; ��ME_OK��ʾʧ��, �������ֵΪ-1
     *
     *  �ο�:
     *      UnRegister
     */
	virtual ME_Result Register(
        IMEIO* pIO,
        IMEIOSink* pIOSink,
        INT iEvent,
        INT& iIndex );

    /**
     *  ����ע�ᵽ��������IO������¼�ע������ע������û�й�ע���¼����ͣ�������
     *  �����ٲ���ע����IO�����IO�¼�����ȻҲ�����ٵ����¼�����ص�
     *
     *  ��Ҫע����ǣ�ע���ǲ���ȡ���������ڹ�ע����¼�����ʱ������ֻע������һ
     *  �ֶ������������¼����͵Ĺ�ע
     *
     *  <pIO>, IO����
     *  <iEvent>, ��Ҫע�����¼�����
     *  <iIndex>, IO�����ڷ������е��������������ֵ��ע��ʱ��ȡ
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     *
     *  �ο�:
     *      Register
     */
	virtual ME_Result UnRegister(
        IMEIO* pIO,
        INT iEvent,
        INT iIndex );

    /**
     *  ���ӵ��¼����У�ֻ������һ�Σ���һ�γɹ��󣬺����ٳ��Ըò������᷵��
     *  ����
     *
     *  <pEq>, ��Ҫ�����������ӵ��¼�����
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
	virtual ME_Result AttachEventQueue( CMEEventQueue* pEq );

    /**
     *  ���ӵ�ʱ�Ӷ��У�ֻ������һ�Σ���һ�γɹ��󣬺����ٳ��Ըò������᷵��
     *  ����
     *
     *  <pTq>, ��Ҫ�����������ӵ�ʱ�Ӷ���
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
	virtual ME_Result AttachTimerQueue( CMETimerQueue* pTq );

    /**
     *  ����IO�¼������ò���ϵͳ�ṩ�Ķ�·������ƣ��磺select��epoll_wait��
     *  WaitForMultipleObjects�ȡ�
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
	virtual ME_Result HandleEvent();

    /**
     *  �����¼�����ͨ��HandleEvent�����¼��Ժ󣨻��߳�ʱ����ͨ��Dispatch����
     *  ���¼������ɷ����������һ��������״̬ʱ��Ӧ��������Զ����¼����������
     *  ��ʱȥ����IO�¼����������ʱ��Ҫ�����¼����д����¼������������һ������
     *  ��״̬ʱ��IO�����Ѿ�ֹͣ����ʱ����Ҫ��ʱ����н����е��Զ����¼�����һ��
     *  �� ��Ȼ�������¼��ķ�ʽ���¼���������ɣ�������ֻ�����ڲ�ͬ�����������
     *  ��Ӧ��֪ͨ��
     *
     *  <bExit>, ָ���Ƿ����˳�״̬
     *
     *  ����ֵ:
     *      RUN_STATE_RUNNING����ʾ��������������״̬
     *      RUN_STATE_EXIT����ʾ��鵽�����������˳�״̬
     */
	virtual ME_Result Dispatch( BOOL bExit = FALSE );

	/**
	* ��ȡReactor������
	*
	* ����ֵ:
	*	int�ͣ�ֵ����REACTOR_TYPE
	*/
	virtual int GetType() const;

public:
	ME_Result Initialize( 
		ME_HANDLE hInterface, 
		CMEWakenerAsyncIOCP* pWakener, 
		DWORD dwThreadNum );

	CMEReactorAsyncIOCP* operator = ( const CMEReactorAsyncIOCP& targetReactor );

private:
	UINT GetWaitTime();

private:
	typedef CMELockThreadMutex MutexType;

private:
	friend class CMEReactorFactory;

private:
	CMEWakenerAsyncIOCP*			m_pWakener;		// ���ڻ���Reactor
	ME_Result						m_hStatus;		// ��ǰReactor��״̬

	ME_HANDLE						m_hIOCP;		// ��ɶ˿ڵľ��
	DWORD							m_dwThreadNum;	// ����ϵͳ���ض���IOCP������ʹ�ö��ٸ��߳�

	MutexType						m_hQueueMutex;	// �����ڱ���������¼���ʱ�Ӷ���
	CMEEventQueue*                 m_pEventQueue;	// �¼�����
	CMETimerQueue*                 m_pTimerQueue;	// ʱ�Ӷ���
};



ME_NAME_SPACE_END

#endif	// ME_WIN

#endif	// __REACTOR_ASYCH_IOCP_H_

