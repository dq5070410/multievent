/**
 *  �û��Զ����߼��̶߳���������
 *
 *  History
 *  01/29/2016 create file
 */

#ifndef __THREAD_CUSTOM_LOGIC_H_
#define __THREAD_CUSTOM_LOGIC_H_

#include "Thread.h"

ME_NAME_SPACE_BEGIN

/**
 *  �û��Զ����߼��̶߳�����
 *
 *  �����û��Զ����߼����̶߳���
 *
 */
class CMEThreadCustomLogic : public CMEThread
{
public:
    /**
     *  ���캯��
     *
     *  <bBlock>, ָ���߳��Ƿ�Ϊ����ģʽ
     *  <iThreadID>, ָ���̶߳����ID
     *  <iType>, ָ���̶߳��������
     *  <pThreadSink>, ָ��һ���û�֪ͨ�ص�������NULL��
     *
     *  ����ֵ:
     *      ��
     */
    CMEThreadCustomLogic(
        BOOL bBlock,
        INT iThreadID,
        INT iType,
        IMEThreadSink* pThreadSink );

    /**
     *  ��������
     *
     *  ����ֵ:
     *      ��
     */
    virtual ~CMEThreadCustomLogic();

protected:
    virtual ME_THREAD_RETURN_TYPE Worker();
};

ME_NAME_SPACE_END

#endif // __ME_THREAD_CUSTOM_LOGIC_H_

