/**
 *  �����¼����кͷ��������̶߳���������
 *
 *  History
 *  01/29/2013 create file
 */

#ifndef __THREAD_EQR_H_
#define __THREAD_EQR_H_

#include "ThreadReactor.h"

ME_NAME_SPACE_BEGIN

/**
 *  �����¼����кͷ��������̶߳�����
 *
 *  ��ΪCMEThreadEQRû��ʱ�Ӷ��У�������Ҫ���ǻ����еĽӿ�ʵ�֣����ز�֧�֣���
 *  �¼���Ͷ����Ϊ�����Ѿ�ʵ���ˣ�����ֻ�Ǽ̳��˾�OK��
 *
 *  �ο�:
 *      CMEThreadReactor
 */
class CMEThreadEQR : public CMEThreadReactor
{
public:
    /**
     *  ���캯��
     *
     *  <bBlock>, ָ���߳��Ƿ�Ϊ����ģʽ
     *  <iThreadID>, ָ���̶߳����ID
     *  <iType>, ָ���̶߳��������
     *  <pReactor>, ָ��һ����������ʵ��
     *  <pEventQueue>, ָ��һ���¼����е�ʵ��
     *  <pThreadSink>, ָ��һ���û�֪ͨ�ص�������NULL��
     *
     *  ����ֵ:
     *      ��
     */
    CMEThreadEQR(
        BOOL bBlock,
        INT iThreadID,
        INT iType,
        CMEReactor* pReactor,
        CMEEventQueue* pEventQueue,
        IMEThreadSink* pThreadSink );

    /**
     *  ��������
     *
     *  ����ֵ:
     *      ��
     */
    virtual ~CMEThreadEQR();

    /**
     *  ���ʱ�Ӷ��󣬸��ǻ���ʵ�֣����ز�֧�֣���Ϊ�������͵��̶߳��󲻾���ʱ��
     *  ����
     *
     *  <pTimer>, ʱ�Ӷ���ʵ��ָ��
     *  <pSink>, ʱ�ӻص�ʵ��ָ��
     *  <hktInterval>, ʱ��ѭ������
     *  <hktDelay>, ʱ����ʱʱ��
     *  <nLoopTime>, ʱ��ѭ��������Ĭ��ֵ0��ֻ����һ�Σ�ME_INFINITE�������޴�
     *
     *  ����ֵ:
     *      ME_ERROR_NOT_AVAILABLE��ʾ��֧��
     */
    virtual ME_Result ScheduleTimer(
        CMETimer* pTimer,
        IMETimerSink* pSink,
        const CMETimeValue& htvInterval,
        const CMETimeValue& htvDelay,
        UINT nLoopTime = 0 );

    /**
     *  ȡ��ʱ�Ӷ��󣬸��ǻ���ʵ�֣����ز�֧�֣���Ϊ�������͵��̶߳��󲻾���ʱ��
     *  ����
     *
     *  <pTimer>, ʱ�Ӷ���ʵ��ָ��
     *
     *  ����ֵ:
     *      ME_ERROR_NOT_AVAILABLE��ʾ��֧��
     */
    virtual ME_Result CancelTimer( CMETimer* pTimer );
};

ME_NAME_SPACE_END

#endif // __THREAD_EQR_H_

