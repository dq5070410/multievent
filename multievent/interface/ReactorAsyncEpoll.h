/**
* �첽IO���͵�Reactor
*
* ʹ��linux��epoll����ʵ��
* 
*
* History
*	9/07/2016		Created
*/
#ifndef __REACTOR_EPOLL_H_
#define __REACTOR_EPOLL_H_

#include "ReactorImpl.h"

#ifdef ME_LINUX

ME_NAME_SPACE_BEGIN

class IMEWakener;
class CMEAsyncIoResult;

class CMEReactorAsyncEpoll : public IMEReactorImpl
{
public:
	/**
	* һ������ֻ����һ��CMEReactorAsyncEpollʵ���Ĵ���
	*
	* ����ֵ:
	*	CMEReactorAsyncEpollָ��	
	*/
	static CMEReactorAsyncEpoll* Instance();

public:
	CMEReactorAsyncEpoll();

	CMEReactorAsyncEpoll(
		IMEWakener* pWakener,
		ME_HANDLE hFdEpoll,
		DWORD dwThreadNum = 0 );

	virtual ~CMEReactorAsyncEpoll();

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
	ME_Result Initialize();

	CMEReactorAsyncEpoll* operator = ( const CMEReactorAsyncEpoll& targetReactor );

private:
	UINT GetWaitTime();

	ME_Result SetEpollEvent(
		int iOption,
		int iEvent,
		ME_HANDLE hSocket,
		CMEAsyncIoResult* pIoResult,
		BOOL bOneShot = TRUE );

	void SetEpollMask(
		int iEvent,
		epoll_event& eeEvent,
		BOOL bOneShot );

	void RegisterWakener( IMEWakener* pWakener );

private:
	typedef CMELockThreadMutex MutexType;
	typedef std::map<DWORD, CMEAsyncIoResult*> MapType;

private:
	friend class CMEReactorFactory;

private:
	IMEWakener*					m_pWakener;
	ME_Result						m_hStatus;

	ME_HANDLE						m_hFdEpoll;
	DWORD							m_dwThreadNum;	// ����ϵͳ���ض���IOCP������ʹ�ö��ٸ��߳�

	MapType							m_mapHandle;
	UINT							m_nIndex;

	MutexType						m_hQueueMutex;	// �����ڱ���������¼���ʱ�Ӷ���
	CMEEventQueue*                 m_pEventQueue;	// �¼�����
	CMETimerQueue*                 m_pTimerQueue;	// ʱ�Ӷ���
};




ME_NAME_SPACE_END

#endif	// ME_LINUX

#endif	// __REACTOR_EPOLL_H_


