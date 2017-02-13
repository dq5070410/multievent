/**
 *  �������¼����е��̶߳���������
 *
 *  History
 *  01/29/2016 create file
 */

#ifndef __THREAD_EQ_H_
#define __THREAD_EQ_H_

#include "Thread.h"

ME_NAME_SPACE_BEGIN


/**
 *  �������¼����е��̶߳�����
 *
 *  �������¼����У�֧���¼�Ͷ�ݣ�����Ҫע������������͵��̶߳���᲻��ϵ�֪ͨ
 *  �¼����м�鲢�����¼������ܻᵼ�½ϸߵ�CPUռ��
 *
 */
class CMEThreadEQ : public CMEThread
{
public:
    /**
     *  ���캯��
     *
     *  <bBlock>, ָ���߳��Ƿ�Ϊ����ģʽ
     *  <iThreadID>, ָ���̶߳����ID
     *  <iType>, ָ���̶߳��������
     *  <pEventQueue>, ָ��һ���¼����е�ʵ��
     *  <pThreadSink>, ָ��һ���û�֪ͨ�ص�������NULL��
     *
     *  ����ֵ:
     *      ��
     */
    CMEThreadEQ(
        BOOL bBlock,
        INT iThreadID,
        INT iType,
        CEventQueue* pEventQueue,
        IMEThreadSink* pThreadSink );

    /**
     *  ��������
     *
     *  ����ֵ:
     *      ��
     */
    virtual ~CMEThreadEQ();

    /**
     *  ������ʽ�¼�Ͷ�ݣ����ǻ���ʵ��
     *
     *  <pEvent>, �Զ����¼�ʵ��ָ��
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result PostEvent( ICustomizeEvent* pEvent );

    /**
     *  ����ʽ�¼�Ͷ�ݣ����ǻ���ʵ��
     *
     *  <pEvent>, �Զ����¼�ʵ��ָ��
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result SendEvent( ICustomizeEvent* pEvent );

    /**
     *  �ȴ��߳�ֹͣ�����ǻ���ʵ�֣�����ʵ���ϻ��ǻ���û����Join����������֮ǰ
     *  ���޸��̶߳�������б�ʶ����Ͷ��ֹͣ�¼���ʹ�̶߳������ֹͣ����
     *
     *  <iTimeout>, �ȴ���ʱʱ�䣬��λ������
     *  <bTerminate>, �Ƿ��ڵȴ���ʱ��ǿ����ֹ�߳�
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result Join(
        UINT iTimeout,
        BOOL bTerminate );

protected:
    /**
     *  �̶߳��������̣��������͵��̶߳�����ǲ��ϵ�֪ͨ�¼����м�鲢�����¼�
     *
     *  ����ֵ:
     *      NULL
     */
    virtual ME_THREAD_RETURN_TYPE Worker();

private:
    friend class CMEThreadFactory;

    /* �¼�����ʵ��ָ�� */
    CEventQueue*                 m_pEventQueue;
};

ME_NAME_SPACE_END

#endif // __THREAD_EQ_H_

