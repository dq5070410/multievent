/**
 *  �̶߳����̶߳���������ӿ�
 *
 *
 *  History
 *  01/28/2013 create file
 */

#ifndef __THREAD_INTERFACE_H_
#define __HREAD_INTERFACE_H_

#include "Base.h"

NAME_SPACE_BEGIN

/* Ԥ���� */
class IThread;
class IThreadManager;
class IThreadSink;
class ICustomizeEvent;


/**
 *  �̶߳�����
 *
 *  MultiEvent�����ṩ���̶߳����࣬���ƽӿں�������¼�Ͷ���Լ���ȡ�̶߳������
 *  ��Ϣ�Ľӿ�
 *
 */
class OS_EXPORT IThread
{
public:
    /**
     *  �����̶߳���ע�⣬����Ƿ�����ģʽ���̶߳�������Ҫ������ô˽ӿڣ�
     *  �̶߳���������ڴ����̶߳������Զ������̶߳������������ģʽ���߳�
     *  ������ô����Ҫ���ϵ���Run���̶߳��󲻶�����
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     *
     *  �ο�:
     *      IThreadManager::CreateThread
     */
    virtual ME_Result Run() = 0;

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
    virtual ME_Result PostEvent( ICustomizeEvent* pEvent ) = 0;

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
    virtual ME_Result SendEvent( ICustomizeEvent* pEvent ) = 0;

    /**
     *  ����̶߳���ID����ȡ�̶߳�����MultiEvent����е�ID��ע�⣬��Ϊ�����ǲ���
     *  ϵͳ�е��߳�ID�����Բ��ܳ��Ե��ò���ϵͳ���̹߳����ӿ����������ID
     *
     *  ����ֵ:
     *      >= 1���̶߳���ID
     */
    virtual INT GetThreadID() const = 0;

    /**
     *  ����̶߳������ͣ���ȡ�̶߳�������ͣ�������ͺʹ���ʱָ����������ͬ
     *
     *  ����ֵ:
     *      �̶߳�������
     *
     *  �ο�:
     *      IThreadManager::THREAD_OPTION
     */
    virtual INT GetThreadType() const = 0;

protected:
    /**
     *  ����������������Ϊ�ܱ����ģ����Ҫ�����̶߳��󣬾ͱ���ʹ���̶߳��������
     *  ��Ӧ�Ĳ�����ִ�У����ܹ�����delete�������̶߳���
     *
     *  ����ֵ:
     *      void
     *
     *  �ο�:
     *      IThreadManager::DestroyThread
     */
    virtual ~IThread() {}
};

/**
 *  �̶߳����û��ص���
 *
 *  ���ϣ���̶߳���������������˳�ǰִ���Լ�������߼�����ô�ͼ̳�����ʵ��
 *  OnStart��OnStop�ӿ�
 *
 *  ע��:
 *      ����Ǵ����û��߼����͵��̶߳��󣬱����ڴ���ʱָ������һ���ص������
 *
 *  �ο�:
 *      IThreadManager::CreateThread
 */
class  IThreadSink
{
public:
    virtual ~IThreadSink() {}

    /**
     *  �̶߳���������ִ�еĹ��̣�����������û��߼����͵��̶߳�����ô��Ӧ����
     *  ����ӿ�ʵ����Ҫ�̶߳�����ִ�е��û��߼�
     *
     *  ����ֵ:
     *      void
     */
    virtual void OnStart() = 0;

    /**
     *  �̶߳���ֹͣǰִ�еĹ��̣������Ҫ���߳��˳�ǰ��һЩ�ƺ�������ô������
     *  ����ӿ���ʵ��
     *
     *  ����ֵ:
     *      void
     */
    virtual void OnStop() = 0;
};

/**
 *  �̶߳��������
 *
 *  ��������е��̶߳����ṩ�̶߳���Ĵ��������������У���ֹͣ�����ٹ��ܣ�����
 *  ֻ��ͨ���̶߳��������������Щ����
 *
 */
class OS_EXPORT IThreadManager
{
public:
    /**
     *  �̶߳��󴴽�ѡ��̶߳����ܹ�����4�ֻ��������ԣ���������4�����Զ��ܶ���
     *  ���ڣ�������4�ֻ���������ϻ��ܹ����������и������Ե��̶߳��󣬽�ö�ٴ�
     *  �ڵ������ʽ������ö�ٷ�Χ�ڵ���϶����ܴ�������Ӧ���̶߳���
     *  
     */
    enum THREAD_OPTION
    {
        /* �����û��߼����ɴ��������������Ե��̶߳��� */
        ME_THREAD_CUSTOM_LOGIC = (1 << 0),

        /* �����Զ����¼����У��ɴ��������������Ե��̶߳��� */
        ME_THREAD_EVENT_QUEUE = (1 << 1),

        /* ����ʱ�Ӷ��У����ɴ��������������Ե��̶߳��� */
        ME_THREAD_TIMER_QUEUE = (1 << 2),

        /* �����¼���·�����������ɴ��������������Ե��̶߳��� */
        ME_THREAD_REACTOR = (1 << 3),

        /* �����¼����к��¼���·���������ɴ�����������ϵ��̶߳��� */
        ME_THREAD_EVENT_QUEUE_REACTOR = ME_THREAD_EVENT_QUEUE \
                                       | ME_THREAD_REACTOR,

        /* ����ʱ�Ӷ��к��¼���·���������ɴ�����������ϵ��̶߳��� */
        ME_THREAD_TIMER_QUEUE_REACTOR = ME_THREAD_TIMER_QUEUE \
                                       | ME_THREAD_REACTOR,

        /* �����¼����С�ʱ�Ӷ��к��¼���·���������ɴ�����������ϵ��̶߳��� */
        ME_THREAD_EVENT_QUEUE_TIMER_QUEUE_REACTOR = ME_THREAD_EVENT_QUEUE \
                                                   | ME_THREAD_TIMER_QUEUE \
                                                   | ME_THREAD_REACTOR,
    };

    /**
     *  ��ȡ�̶߳������������ָ��
     *
     *  ����ֵ:
     *      �̶߳������������ָ��
     */
    static IThreadManager* Instance();

    /**
     *  �����̶߳���ͨ������ӿڴ������̶߳����ܹ�ֱ�ӽ����̶߳���������Ĺ���
     *
     *  <bBlock>, ָ���̶߳����Ƿ�������ģʽ����
     *            ����ģʽ���̶߳�����ڵ��õ�ǰ�ӿڵ��̻߳����״����У���һ�ι�
     *            ��������ɺ󣬽��᷵�أ��������Ҫ�̶߳������ִ�У���ô����Ҫ
     *            ���ϵĵ����̶߳����Run�ӿ�
     *            ������ģʽ���̶߳�����½�һ���̻߳�����ִ���߳��߼�����������
     *            ��ǰ�ӿھͻ᷵�أ��������Ҳ����Ҫ�����ٵ����̶߳����Run�ӿ�
     *  <iType>, ����ָ���̶߳��������
     *  <pThreadSink>, ָ���̶߳����û��ص�����������û��߼����͵��̶߳������
     *                 ��ָ��һ���̶߳����û��ص��������������͵��̶߳�����ǿ
     *                 ��Ҫ��
     *  <pThread>, ���������ɲ����������̶߳���
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result CreateThread(
        BOOL bBlock,
        INT iType,
        IThreadSink* pThreadSink,
        IThread*& pThread ) = 0;

    /**
     *  �����̶߳�������̶߳�����ע�ᵽ����������ô����ǰ���Ὣ��ע��
     *  ע�⣺�޷�����һ�������е��̶߳����ڳ��������̶߳���֮ǰ�������Join��
     *  ���߳�ֹͣ
     *
     *  <pThread>, �̶߳���ʵ��ָ��
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     *
     *  �ο�:
     *      IThreadManager::Join
     */
    virtual ME_Result DestroyThread( IThread* pThread ) = 0;

    /**
     *  �ȴ��̶߳���ֹͣ
     *  ע�⣺�����̶߳���֮ǰ�������������ӿ����ȴ��̶߳���ֹͣ
     *
     *  <pThread>, �̶߳���ʵ��ָ��
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     *
     *  �ο�:
     *      IThreadManager::DestroyThread
     */
    virtual ME_Result Join( IThread* pThread ) = 0;

    /**
     *  ��õ�ǰ���ڵ��̶߳���
     *
     *  ����ֵ:
     *      �ǿձ�ʾ��ǰ���ڵ��̶߳���ʵ��ָ��; NULL��ʾû���ҵ�
     */
    virtual IThread* GetCurrentThread() = 0;

    /**
     *  ��ÿ��õ������̶߳���
     *
     *  ����ֵ:
     *      �ǿձ�ʾ�����̶߳���ʵ��ָ��; NULL��ʾû���ҵ�
     */
    virtual IThread* GetNetworkThread() = 0;

    /**
     *  ��ÿ��õ��û��̶߳���
     *
     *  ����ֵ:
     *      �ǿձ�ʾ�û��̶߳���ʵ��ָ��; NULL��ʾû���ҵ�
     */
    virtual IThread* GetUserThread() = 0;

protected:
    /**
     *  ����������������Ϊ�ܱ����ģ��̶߳������������ֻ�������������
     *
     *  ����ֵ:
     *      ��
     */
    virtual ~IThreadManager() {}
};

NAME_SPACE_END

#endif // __THREAD_INTERFACE_H_
