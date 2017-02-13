/**
 *  �̶߳����������
 *
 *  History
 *  01/29/2016 create file
 */

#ifndef __ME_THREAD_H_
#define __ME_THREAD_H_

#include "ThreadInterface.h"
#include "EventQueueT.h"
#include "TimerQueueT.h"
#include "LockThreadMutex.h"

#include "LockEvent.h"

ME_NAME_SPACE_BEGIN

// Ԥ����
class ITimerSink;
class CTimeValue;
class CMETimer;
class CMEReactor;

/**
 *  �̶߳������
 *
 *  ���ֲ�ͬ�����̶߳���Ļ��࣬�̳в�ʵ��IMEThread�Ľӿڣ�����չ������ʱ�ӣ���
 *  ��ֹͣ�Ƚӿڣ���������ڲ��Ŀ����� ��Ҳ�е�����ͬ��������ͳһ��ʵ�ַ�ʽ����Ҫ
 *  ע����������������¼����л���ʱ�Ӷ�����Щ���ԣ��¼�Ͷ�ݻ�ʱ�ӵĲ������᷵
 *  �ز�֧�֣���֧����Щ��������������Ҫ���ǻ����ʵ��
 *
 */
class CMEThread : public IMEThread
{
public:
    /* �̶߳����״̬ */
    enum RUN_STATE
    {
        RUN_STATE_NONE = 300,

        // ��ʾ�̶߳�����������������״̬
        RUN_STATE_RUNNING,

        // ��ʾ�̶߳������˳�״̬���Ѿ��˳�
        RUN_STATE_EXIT,
    };

    CMEThread(
        BOOL bBlock,
        INT iThreadID,
        INT iType,
        IMEThreadSink* pThreadSink );

    virtual ~CMEThread();

    /**
     *  ����̶߳���ID����ȡ�̶߳�����MultiEvent����е�ID��ע����Ϊ�����ǲ���ϵ
     *  ͳ�е��߳�ID�����Բ��ܳ��Ե��ò���ϵͳ���̹߳���ӿ����������ID
     *
     *  ����ֵ:
     *      >= 1���̶߳���ID
     */
    virtual INT GetThreadID() const;

    /**
     *  ����̶߳������ͣ���ȡ�̶߳�������ͣ�������ͺʹ���ʱָ����������ͬ
     *
     *  ����ֵ:
     *      �̶߳�������
     *
     *  �ο�:
     *      IMEThreadManager::ME_THREAD_OPTION
     */
    virtual INT GetThreadType() const;

    /**
     *  �����̶߳���ע�⣬����Ƿ�����ģʽ���̶߳�������Ҫ������ô˽ӿڣ�
     *  �̶߳���������ڴ����̶߳������Զ������̶߳������������ģʽ���߳�
     *  ������ô����Ҫ���ϵ���Run���̶߳��󲻶�����
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result Run();

    /**
     *  �ȴ��߳�ֹͣ
     *
     *  <iTimeout>, �ȴ���ʱʱ�䣬��λ������
     *  <bTerminate>, �Ƿ��ڵȴ���ʱ��ǿ����ֹ�߳�
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ME_ERROR_TIMEOUT��ʾ��ʱ; ������ʾʧ��
     */
    virtual ME_Result Join(
        UINT iTimeout,
        BOOL bTerminate );

    /**
     *  ������ʽ�¼�Ͷ�ݣ�����̶߳�������¼����У���ô�ڵ�������ӿں����¼�
     *  ���ɹ�Ͷ�ݵ��¼����к�ͻ᷵�أ�����̶߳��󲻾����¼����У���ô�������
     *  �ӿڻ᷵�ش���
     *
     *  <pEvent>, �Զ����¼�����ʵ��ָ��
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result PostEvent( ICustomizeEvent* pEvent );

    /**
     *  ����ʽ�¼�Ͷ�ݣ�����̶߳�������¼����У���ô�ڵ�������ӿں����¼���
     *  �ɹ�Ͷ�ݵ��¼����к󣬻�һֱ�ȴ��¼���ִ�У�����̶߳��󲻾����¼����У�
     *  ��ô��������ӿڻ᷵�ش���
     *  ��Ҫ��ע���ǣ������ͬһ���̶߳���Ļ����е��ô˽ӿڣ�Ҳ�᷵�ش���
     *
     *  <pEvent>, �Զ����¼�����ʵ��ָ��
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result SendEvent( ICustomizeEvent* pEvent );

    /**
     *  ���̶߳����ʱ�Ӷ������һ��ʱ�Ӷ���
     *
     *  <pTimer>, ʱ�Ӷ���ʵ��ָ��
     *  <pSink>, ʱ�ӻص�ʵ��ָ��
     *  <hktInterval>, ʱ��ѭ������
     *  <hktDelay>, ʱ����ʱʱ��
     *  <nLoopTime>, ʱ��ѭ��������Ĭ��ֵ0��ֻ����һ�Σ�ME_INFINITE�������޴�
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result ScheduleTimer(
        CMETimer* pTimer,
        ITimerSink* pSink,
        const CTimeValue& htvInterval,
        const CTimeValue& htvDelay,
        UINT nLoopTime = 0 );

    /**
     *  ���̶߳����ʱ�Ӷ�����ȡ��һ��ʱ�Ӷ���
     *
     *  <pTimer>, ʱ�Ӷ���ʵ��ָ��
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result CancelTimer( CMETimer* pTimer );

    /**
     *  ��ȡ�̶߳����ϵķ�����ʵ��
     *
     *  ע�⣬�����ǲ����з������ģ�����Ĭ�Ϸ���NULL
     *
     *  ����ֵ:
     *      �ǿգ�������ʵ��ָ��; NULL����ǰ�̶߳��󲻴��з�����
     */
    virtual CMEReactor* GetReactor();

protected:
    /**
     *  �̻߳ص��������ڷ�����ģʽ�´����߳�ʹ��
     *
     *  <pParam>, �̻߳ص��û�����ָ�룬����Ӧ�þ����̶߳�����
     *
     *  ����ֵ:
     *      NULL
     */
    static ME_THREAD_RETURN_TYPE ME_STDCALL ThreadFunc( LPVOID pParam );

    /**
     *  �̶߳��������̺�������ͬ���з�ʽ���̶߳������Ͷ�Ҫ�̳в�ʵ������ӿ�
     *
     *  ����ֵ:
     *      NULL
     */
    virtual ME_THREAD_RETURN_TYPE Worker() = 0;

public:
    /**
     *  ��ȡ�̶߳���ʵ�ʵ��߳�ID
     *
     *  ����ֵ:
     *      �̶߳���ʵ�ʵ��߳�ID
     */
    ME_THREAD_ID GetThreadOSID() const;

    /**
     *  ��ȡ�̶߳���ʵ�ʵľ��
     *
     *  ����ֵ:
     *      �̶߳���ʵ�ʵľ��
     */
    ME_THREAD_HANDLE GetThreadHandle();

protected:
    /* ��ʶ�̶߳����Ƿ�Ϊ����ģʽ */
    BOOL                            m_bBlock;

    /* ��ʶ�̶߳��������״̬ */
    INT                             m_iStatus;

    /* �û��ص�����ָ�� */
    IMEThreadSink*                   m_pThreadSink;

    /* �߳̾��(��ӦOS�е��߳̾��) */
    ME_THREAD_HANDLE               m_hThreadHandle;

    /* �߳�ID(��ӦOS�е��߳�ID) */
    ME_THREAD_ID                   m_ThreadID;

    /* �߳��� */
    CMELockThreadMutex             m_hThreadMutex;

private:
    /* �����̶߳��������Ϊ�̶߳������Ԫ�� */
    friend class CMEThreadManager;

    /* �̶߳���ID(�̶߳�������������ID������OS�е��߳�ID) */
    INT                             m_iThreadID;

    /* �̶߳������� */
    INT                             m_iThreadType;

	CLockEvent					m_lockEvent;	// �߳�����������֮��֪ͨ�����ȴ��߳�
};

ME_NAME_SPACE_END

#endif // __ME_THREAD_H_

