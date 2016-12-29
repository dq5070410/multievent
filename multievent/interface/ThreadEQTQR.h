/**
 *  �����¼����У�ʱ�Ӷ����Լ����������̶߳���������
 *
 *  History
 *  01/29/2016 create file
 */

#ifndef __THREADEQTQR_H_
#define __THREADEQTQR_H_

#include "HikThreadReactor.h"

ME_NAME_SPACE_BEGIN

/**
 *  �����¼����У�ʱ�Ӷ����Լ����������̶߳�����
 *
 *  �������ڵ����������ں�CMEThreadEQR��CMEThreadTQR������ͬ����Ʋ���ϣ���
 *  �Ĺ���HikThreadReactor����ʵ�֣�CMEThreadEQTQR�����ṩ����������������ܹ�
 *  ���������¼����С�ʱ�Ӷ��кͷ��������̶߳�������
 *
 *  �ο�:
 *      CMEThreadReactor
 */
class CMEThreadEQTQR : public CMEThreadReactor
{
public:
    /**
     *  ���캯��
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
    CMEThreadEQTQR(
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
    virtual ~CMEThreadEQTQR();
};

ME_NAME_SPACE_END

#endif // __THREADEQTQR_H_

