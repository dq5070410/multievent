/**
 *  �Է�������Ϊ���з�ʽ���̶߳���������
 *
 *  History
 *  01/29/2016 create file
 */

#ifndef __ME_THREAD_REACTOR_H_
#define __ME_THREAD_REACTOR_H_

#include "Thread.h"

ME_NAME_SPACE_BEGIN

/* Ԥ���� */
class CMEReactor;


/**
 *  �Է�������Ϊ���з�ʽ���̶߳�����
 *
 *  �Է�������Ϊ���з�ʽ����˼�������̶߳���Ĺ���������Ҫ�����з������Ĳ��񡪡���
 *  �ɹ���Ϊ������Ȼ���з��������̶߳�����3�֣��������еķ�ʽ��һ���ģ������ɷ�
 *  �����������¼��Ĳ���ͷ��ɵģ���������������һ�����࣬ʵ����������Ѿ�������
 *  �����еĹ��ܣ�������������������ԵĲ�ͬ��ֻ���в��ֵĹ���
 *
 *  ע�⣺�ڹ����̶߳����ʱ��Ӧ�ñ�֤����Ĳ�������ȷ�ģ��̶߳��󹤳�����Ҫ����
 *        ��һ��
 *
 *  �ο�:
 *      CMEReactorEQR
 *      CMEReactorTQR
 *      CMEReactorEQTQR
 */
class CMEThreadReactor : public CMEThread
{
public:
    /**
     *  ������ʽ�¼�Ͷ�ݣ����з��������̶߳�����Ͷ���¼��ɹ�����Ҫ֪ͨ������
     *  �����ȴ�
     *
     *  <pEvent>, �Զ����¼�ʵ��ָ��
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result PostEvent( IMECustomizeEvent* pEvent );

    /**
     *  ����ʽ�¼�Ͷ�ݣ����з��������̶߳�����Ͷ���¼��ɹ���֪ͨ������������
     *  ����Ȼ���ٵȴ��¼�ִ�вŷ���
     *
     *  <pEvent>, �Զ����¼�ʵ��ָ��
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result SendEvent( IMECustomizeEvent* pEvent );

    /**
     *  ���ʱ�Ӷ�����ʱ�Ӷ������ʱ�Ӷ���󣬻���Ҫ֪ͨ�����������ȴ�
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
        IMETimerSink* pSink,
        const CMETimeValue& htvInterval,
        const CMETimeValue& htvDelay,
        UINT nLoopTime = 0 );

    /**
     *  ȡ��ʱ�Ӷ��󣬸���ʱ�ӵ�������ȡ��ʱ�Ӷ����е�ʱ�Ӷ���ͬ����Ҫ֪ͨ����
     *  �������ȴ�
     *
     *  <pTimer>, ʱ�Ӷ���ʵ��ָ��
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result CancelTimer( CMETimer* pTimer );

    /**
     *  ��ȡ�̶߳����еķ�����
     *
     *  ����ֵ:
     *      ������ʵ��ָ��
     */
    virtual CMEReactor* GetReactor();

    /**
     *  �ȴ��̶߳���ֹͣ�����ǻ���CMEReactor��ʵ�֣�����֪ͨ������ֹͣ���÷���
     *  �������˳����̣�Ȼ����û���CMEReactor��Join���ȴ��̶߳���ֹͣ
     *
     *  <iTimeout>, �ȴ���ʱʱ��
     *  <bTerminate>, �Ƿ�����ǿ����ֹ�̶߳���
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result Join(
        UINT iTimeout,
        BOOL bTerminate );

protected:
    /**
     *  ���캯��������Ϊ�ܱ����ģ�CMEReactorR�ǲ��ܹ��������ģ���ֻ�Ǵ�����һ��
     *  �̶߳��������ģʽ��������Ĳ�����������ģʽ���̶߳�������������������
     *
     *  <bBlock>, ָ���߳��Ƿ�Ϊ����ģʽ
     *  <iThreadID>, ָ���̶߳����ID
     *  <iType>, ָ���̶߳��������
     *  <pReactor>, ָ��һ����������ʵ��
     *  <pTimerQueue>, ָ��һ��ʱ�Ӷ��е�ʵ��
     *  <pEventQueue>, ָ��һ���¼����е�ʵ��
     *  <pThreadSink>, ָ��һ���û�֪ͨ�ص�������NULL��
     *
     *  ����ֵ:
     *      ��
     */
    CMEThreadReactor(
        BOOL bBlock,
        INT iThreadID,
        INT iType,
        CMEReactor* pReactor,
        CMETimerQueue* pTimerQueue,
        CMEEventQueue* pEventQueue,
        IMEThreadSink* pThreadSink );

    /**
     *  ��������
     *
     *  ����ֵ:
     *      ��
     */
    virtual ~CMEThreadReactor();

    /**
     *  �̶߳��������̣����ڴ��з��������̶߳������ͣ���Ҫ�Ĺ������̾��ǵ��÷�
     *  ������Run�ӿڣ������������Ĺ������̣����������Ĳ��񡪡����ɹ��̣�
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_THREAD_RETURN_TYPE Worker();

private:
    friend class CMEThreadFactory;

    // ������ʵ��ָ��
    CMEReactor*                    m_pReactor;

    // ʱ�Ӷ���ʵ��ָ��
    CMETimerQueue*                 m_pTimerQueue;

    // �¼�����ʵ��ָ��
    CMEEventQueue*                 m_pEventQueue;
};

ME_NAME_SPACE_END

#endif // __THREAD_REACTOR_H_

