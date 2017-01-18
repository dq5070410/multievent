/**
 *  IO��׽��������Ŀ�
 * 
 *
 *  History
 *  12/12/2016 create file
 */

#ifndef __REACTOR_H_
#define __REACTOR_H_

#include "Base.h"
#include "EventQueueT.h"
#include "TimerQueueT.h"
#include "LockThreadMutex.h"

ME_NAME_SPACE_BEGIN

class IMEReactorImpl;
class IMEIO;
class IMEIOSink;
class IMECustomizeEvent;
class CMETimer;
class ITimerSink;

class ME_OS_EXPORT CMEReactor
{
public:
    /**
     *  ������������
     */
    enum REACTOR_TYPE
    {
        REACTOR_TYPE_NULL = 1 << 0,

		REACTOR_TYPE_SYNC = 1 << 1,

		REACTOR_TYPE_ASYNC = 1 << 2,

        /* ����select��ʽ�ķ����� */
        REACTOR_TYPE_SELECT = (1 << 3) | REACTOR_TYPE_SYNC,
        
        /* ����WaitForMultipleObjects��ʽ�ķ����� */
        REACTOR_TYPE_WFMO = (1 << 4) | REACTOR_TYPE_SYNC,

		/* �첽ģʽ��win32����Ч��ʹ����ɶ˿� */
		REACTOR_TYPE_ASYCH_IOCP = (1 << 5) | REACTOR_TYPE_ASYNC,

		/* �첽ģʽ��linux��Ч��ʹ��epoll */
		REACTOR_TYPE_ASYCH_EPOLL = (1 << 6) | REACTOR_TYPE_ASYNC,

		/* �첽ģʽ��linux��Ч��ʹ��aioϵ�� */
		REACTOR_TYPE_ASYCH_AIO = (1 << 7) | REACTOR_TYPE_ASYNC,
    };

    /**
     * ���ڱ�ʶReactor������ģʽ
     */
    enum RUN_MODE
    {
        RUN_MODE_NONE = 100,

        /** 
         * ѭ��ģʽ�����ģʽ��������ǰ�߳�
         *
         * �������Run()�����������
         */
        RUN_MODE_LOOP,

        /** 
         * ����ģʽ�������ģʽ�£��̲߳��ᱻ���� 
         *
         * �������Run()������Reactor�������һ���¼���׽��
         * ����֮����Run()��������
         *
         * ���Ե�����Ҫ�����ģʽ�²�ͣ�Ĳ�׽�¼�����ô�����
         * ��Ҫ��ͣ�ĵ���Run()
         */
        RUN_MODE_HEART_BEAT,
    };

    enum NOTIFY_TYPE
    {
        NOTIFY_TYPE_NONE = 200,

        /* ���ڻ���Reactor */
        NOTIFY_TYPE_WAKEUP,

        /* ʹReactor�˳����� */
        NOTIFY_TYPE_EXIT,
    };

    enum RUN_STATE
    {
        RUN_STATE_NONE = 300,

        /* ��ʾReactor��������������״̬ */
        RUN_STATE_RUNNING,

        /* ��ʾReactor���ڻ��Ѿ��˳������״̬���κ��¼�Ͷ�ݶ���ʧ�� */
        RUN_STATE_EXIT,
    };

public:
    /**
     * �ο���winsock2����
     */
    enum EVENT_TYPE
    {
        EVENT_TYPE_NULL = 0,
        EVENT_TYPE_READ = 1 << 0,
        EVENT_TYPE_WRITE = 1 << 1,
        EVENT_TYPE_EXCEPTION = 1 << 2,
        EVENT_TYPE_ACCEPT = 1 << 3,
        EVENT_TYPE_CONNECT = 1 << 4,
        EVENT_TYPE_CLOSE = 1 << 5,
        EVENT_TYPE_ABANDONED = EVENT_TYPE_CLOSE,

        /* ���¶����Ƕ��������͵ķ���ͻ��ܣ��������ı�д */
        EVENT_TYPE_ACTIVE = EVENT_TYPE_READ | EVENT_TYPE_WRITE,

        EVENT_TYPE_IN = EVENT_TYPE_READ | EVENT_TYPE_ACCEPT,
        EVENT_TYPE_OUT = EVENT_TYPE_WRITE | EVENT_TYPE_CONNECT,
        EVENT_TYPE_ERR = EVENT_TYPE_CLOSE | EVENT_TYPE_EXCEPTION,

        EVENT_TYPE_ALL = EVENT_TYPE_IN | EVENT_TYPE_OUT | EVENT_TYPE_ERR,
    };

public:
    CMEReactor( IMEReactorImpl* pReactorImpl );

    ~CMEReactor();

public:
    /**
     *  ���з���������������2������ģʽ������ģʽ��ѭ��ģʽ���ֱ��Ӧ���̶߳���
     *  ������ģʽ�ͷ�����ģʽ
     *
     *  <iRunMode>, ָ���̶߳��������ģʽ��Ĭ����ѭ��ģʽ
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     *
     *  �ο�:
     *      RUN_MODE::RUN_MODE_LOOP
     *      RUN_MODE::RUN_MODE_HEART_BEAT
     */
    ME_Result Run( INT iRunMode = RUN_MODE_LOOP );

    /**
     *  ֪ͨ�����������ȴ���һ������������������������й��������Զ����¼�Ͷ�ݵ�
     *  �¼����л������/ȡ��ʱ�ӣ���ô��Ҫ֪ͨ�������ӵ�ǰ�����ȴ��¼���������
     *  ����֪ͨ�¼����л���ʱ�Ӷ��м���Զ����¼�����ʱ���¼�
     *
     *  <iNotifyType>, ֪ͨ����
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    ME_Result Notify( INT iNotifyType );

    /**
     *  ע�����Ȥ��IO����IO�¼����¼�����ص��������������ң��ܹ�ͨ������ӿ�
     *  ���µ�ǰ��ע���IO�����IO�¼�
     *
     *  <pIO>, ��ע��IO����ʵ��ָ��
     *  <pIOSink>, IO�¼��ص�ʵ��ָ��
     *  <iEvent>, ��ע��IO�¼�����
     *  <iIndex>, �����ע���µ�IO������ô����ʹ��-1��ע��ɹ��������IO������
     *            �������е�������������޸���ע��IO������¼����ͣ���ô�ͱ�����
     *            ����ע��IO�����ڷ������е�������������ܻ�ʧ�ܻ����𲻿�Ԥ�ڵ�
     *            ����
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    ME_Result Register( 
        IMEIO* pIO,
        IMEIOSink* pIOSink,
        INT iEvent,
        INT& iIndex );

    /**
     *  ע����ע���IO�����IO�¼���������ע����ע���IO�¼�
     *
     *  <pIO>, ��ע��IO����ʵ��ָ��
     *  <iEvent>, ��Ҫע����IO�¼�����
     *  <iIndex>, ����������ע��IO�����ڷ������е�������������ܻ�ʧ�ܻ����𲻿�
     *            Ԥ�ڵ�����
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    ME_Result UnRegister(
        IMEIO* pIO,
        INT iEvent,
        INT iIndex );

    /**
     *  ���ӷ��������¼����У�Ŀǰ��һ��������ֻ�ܹ����ӵ�һ���¼����У��Ե�һ��
     *  ���ӳɹ�Ϊ׼
     *
     *  <pEq>, ��Ҫ���ӵ��¼�����
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    ME_Result AttachEventQueue( CMEEventQueue* pEq );

    /**
     *  ���ӷ�������ʱ�Ӷ��У�Ŀǰ��һ��������ֻ�ܹ����ӵ�һ��ʱ�Ӷ��У��Ե�һ��
     *  ���ӳɹ�Ϊ׼
     *
     *  <pTq>, ��Ҫ���ӵ�ʱ�Ӷ���
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    ME_Result AttachTimerQueue( CMETimerQueue* pTq );

	/* 2013.9.4 added by Τ�� */
	/**
	* ��ȡReactor������
	*
	* ����ֵ:
	*	int�ͣ�ֵ����REACTOR_TYPE
	*/
	int GetType() const;

private:
    friend class CMEReactorFactory;

    IMEReactorImpl*                m_pReactorImpl; // Reactor������ʵ��������

    CMELockThreadMutex             m_hThreadMutex; // ����Reactor��Run����
};


ME_NAME_SPACE_END

#endif    // __REACTOR_H_











