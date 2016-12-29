/**
 *  �̶߳��������ʵ��������
 *
 *  History
 *  01/29/2013 create file
 */

#ifndef __THREAD_MANAGER_H_
#define __THREAD_MANAGER_H_

#include "ThreadInterface.h"
#include "ThreadIDManager.h"
#include "LockThreadMutex.h"
#include "SingletonT.h"

ME_NAME_SPACE_BEGIN

class CMEThread;

/**
 *  �̶߳��������ʵ����
 *
 *  �̳в�ʵ��IMEThreadManager�����нӿڣ�����չ����������ڲ�ʹ�õĽӿڣ����
 *  ʼ��������ʼ����
 *
 */
class CMEThreadManager : public IMEThreadManager
{
public:
    CMEThreadManager();
    virtual ~CMEThreadManager();

    virtual ME_Result CreateThread(
        BOOL bBlock,
        INT iType,
        IMEThreadSink* pThreadSink,
        IMEThread*& pThread );

    virtual ME_Result DestroyThread( IMEThread* pThread );

    virtual ME_Result Join( IMEThread* pThread );

    virtual IMEThread* GetCurrentThread();

    virtual IMEThread* GetNetworkThread();

    virtual IMEThread* GetUserThread();

    /**
     *  ��ʼ�������ݴ�����̶߳���ص���ָ��ģʽ����һ���û��߳�
     *
     *  ע�⣺��������ڲ��Ľӿڣ�������Ҫ��֤��һ�����̻�����ֻ����һ������ӿ�
     *
     *  <pThreadSink>, ָ�����̶߳�����û��ص���Ĭ��ΪNULL
     *  <bBlockMode>, ָ���̶߳��������ģʽ��Ĭ��ΪFALSE����������ģʽ
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    ME_Result Initialize(
        IMEThreadSink* pThreadSink = NULL,
        BOOL bBlockMode = FALSE );

    /**
     *  ����ʼ��������������л��������е��̶߳��󣬰�������û������ĵ�û������
     *  ���̶߳����Լ���ʼ��ʱ�����������̶߳�����û��̶߳���
     *
     *  ע�⣺��������ڲ��Ľӿڣ�������Ҫ��֤�ڵ�������ӿ�֮ǰ�Ѿ�����JoinAll
     *        ֹͣ�������̶߳���
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    ME_Result Finalize();

    /**
     *  �����ƶ���������û��߳���������ָ�����������͵��̶߳�����Щ�̶߳���Ĭ
     *  �϶��Ƿ������ģ����Ҳ������û��ص�
     *
     *  ע�⣺Ŀǰֻ������һ���û��̶߳��������Initialize�ڼ䴴�����û��߳�
     *  ������ô���ｫ���ٴ����û��̶߳�������ӿ�ͬ�����ܹ���ε���
     *
     *
     *  <iNetworkThreadNum>, �����̶߳�������
     *  <iUserThreadNum>, �û��̶߳���������Ĭ��ֵ1
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    ME_Result CreateThread(
        INT iNetworkThreadNum,
        INT iUserThreadNum = 1 );

    /**
     *  �����̶߳���ͨ������ӿڴ������̶߳��󲻻�����̶߳���������Ĺ�����
     *  ����Ҫ�̶߳���������ӹ��̶߳�����ô��Ҫ����Register
     *
     *  <bBlock>, ָ���̶߳����Ƿ�������ģʽ����
     *            ����ģʽ���̶߳�����ڵ��õ�ǰ�ӿڵ��̻߳����״����У���һ�ι�
     *            ��������ɺ󣬽��᷵�أ��������Ҫ�̶߳������ִ�У���ô����Ҫ
     *            ���ϵĵ����̶߳����Run�ӿ�
     *            ������ģʽ���̶߳�����½�һ���̻߳�����ִ���߳��߼�����������
     *            ��ǰ�ӿھͻ᷵�أ��������Ҳ����Ҫ�����ٵ����̶߳����Run�ӿ�
     *  <iThreadType>, ����ָ���̶߳��������
     *  <iClassificationType>, �̶߳��󻮷�����
     *  <pThreadSink>, ָ���̶߳����û��ص�����������û��߼����͵��̶߳������
     *                 ��ָ��һ���̶߳����û��ص��������������͵��̶߳�����ǿ
     *                 ��Ҫ��
     *  <pThread>, ���������ɲ����������̶߳���
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     *
     *  �ο�:
     *      Register
     */
    ME_Result CreateThread(
        BOOL bBlock,
        INT iThreadType,
        INT iClassificationType,
        IMEThreadSink* pThreadSink,
        CMEThread*& pThread );

    /**
     *  �����̶߳���
     *
     *  <pThread>, �̶߳���ʵ��ָ��
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    ME_Result DestroyThread( CMEThread* pThread );

    /**
     *  ע�ᣬ���̶߳���ע������������ɹ������������̶߳���
     *
     *  <pThread>, �̶߳���ʵ��ָ��
     *
     *  ����ֵ:
     *      
     */
    ME_Result Register( CMEThread* pThread );

    /**
     *  ע�������̶߳���ӹ�������ע����ע������̶߳����޷�ͨ��������������
     *
     *  <pThread>, �̶߳���ʵ��ָ��
     *
     *  ����ֵ:
     *      ME_ERROR_NOT_IMPLEMENTED��ʾ��ǰδʵ��
     */
    ME_Result Unregister( CMEThread* pThread );

    /**
     *  �ȴ��߳�ֹͣ���ܹ�ָ���̶߳����ID�Լ��ȴ���ʱʱ�䣬��Ҫע����ǣ������
     *  ����ʱ������ǿ����ֹ�̶߳���
     *
     *  <pThread>, �̶߳���ʵ��ָ��
     *  <iTimeout>, �ȴ���ʱʱ��
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    ME_Result Join(
        CMEThread* pThread,
        UINT iTimeout );

    /**
     *  �����ȴ������̶߳���ֹͣ��������̻������ȴ����е��̶߳���ֹͣ���������
     *  �û��������ⲿ�̶߳��󣬳�ʼ���������ڲ��̶߳��������̶߳�����û��߳�
     *  ����
     *
     *  <iTimeout>, �ȴ���ʱʱ�䣬Ĭ��ME_INFINITE����������ʱ
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    ME_Result JoinAll( UINT iTimeout = ME_INFINITE );

    /**
     *  ͨ���̶߳���Ĺ�������ȡ�̶߳���
     *
     *  <iClassification>, �̹߳���
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    IMEThread* GetThread( INT iClassification );

private:
    /* �̶߳������ID-�̶߳���ʵ��ָ��� */
    typedef std::map<INT, CMEThread*> ThreadMapType;

private:
    /* �̶߳���ID������ */
    CMEThreadIDManager             m_ThreadIDManager;

    /* �̶߳���� */
    ThreadMapType                   m_mapThreads;

    /* �̶߳���������̰߳�ȫ�� */
    CMELockThreadMutex             m_hThreadMutex;

    /* �̶߳���������Ƿ�����ע��ע��������ʶ */
    BOOL                            m_bEnableRegister;
};

typedef CMESingletonT<CMEThreadManager> CMEThreadManagerSingleton;

ME_NAME_SPACE_END

#endif // __THREAD_MANAGER_H_

