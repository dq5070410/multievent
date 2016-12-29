/**
 *  ����ʱ�Ӷ��кͷ��������̶߳���������
 *
 *  History
 *  01/29/2016 create file
 */

#ifndef __THREAD_TQR_H_
#define __THREAD_TQR_H_

#include "ThreadReactor.h"

ME_NAME_SPACE_BEGIN

/**
 *  ����ʱ�Ӷ��кͷ��������̶߳�����
 *
 *  ��ΪCMEThreadTQRû���¼����У���˲�֧���¼���Ͷ�ݣ���Ҫ���ǻ����е��¼�Ͷ
 *  �ݵ�ʵ�֣����ز�֧�֣���ʱ�ӵ���Ӻ�ȡ����Ϊ������Ѿ�ʵ���ˣ�����ֻ��Ҫ�̳�
 *  ��OK��
 *
 *  �ο�:
 *      CMEThreadReactor
 */
class CMEThreadTQR : public CMEThreadReactor
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
     *  <pThreadSink>, ָ��һ���û�֪ͨ�ص�������NULL��
     *
     *  ����ֵ:
     *      ��
     */
    CMEThreadTQR(
        BOOL bBlock,
        INT iThreadID,
        INT iType,
        CMEReactor* pReactor,
        CMETimerQueue* pTimerQueue,
        IMEThreadSink* pThreadSink );

    /**
     *  ��������
     *
     *  ����ֵ:
     *      ��
     */
    virtual ~CMEThreadTQR();

    /**
     *  ������ʽ�¼�Ͷ�ݣ���ǰ�̶߳���֧�֣���Ҫ���ǻ���CMEReactorR��ʵ�֣���
     *  �ز�֧��
     *
     *  <pEvent>, �¼�����ʵ��ָ��
     *
     *  ����ֵ:
     *      ME_ERROR_NOT_AVAILABLE��ʾ��֧��
     */
    virtual ME_Result PostEvent( IMECustomizeEvent* pEvent );

    /**
     *  ����ʽ�¼�Ͷ�ݣ���ǰ�̶߳���֧�֣���Ҫ���ǻ���CMEReactorR��ʵ�֣�����
     *  ��֧��
     *
     *  <pEvent>, �¼�����ʵ��ָ��
     *
     *  ����ֵ:
     *      ME_ERROR_NOT_AVAILABLE��ʾ��֧��
     */
    virtual ME_Result SendEvent( IMECustomizeEvent* pEvent );
};

ME_NAME_SPACE_END

#endif // __THREAD_TQR_H_

