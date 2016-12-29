/**
 *  �̶߳���ID����������
 *
 *  History
 *  01/29/2016 create file
 */

#ifndef __THREAD_ID_MANAGER_H_
#define __THREAD_ID_MANAGER_H_

#include "Base.h"
#include "AtomicOperationT.h"

ME_NAME_SPACE_BEGIN

/**
 *  �̶߳���ID������
 *
 *  ���������̶߳����ID����ͬ���̶߳��������ò�ͬ��ID�ֶ�
 *
 */
class CMEThreadIDManager
{
public:
    /**
     *  ���̶߳�����й��࣬���ַ�ʽ����ID�ֶεķ�ʽ�����������̺߳��û��̶߳���
     *  ��������ڲ��̣߳��ⲿ�߳�ָ����û�ͨ���̶߳���������������̶߳���
     */
    enum ME_THREAD_ID_CLASSIFICATION
    {
        /* �����̵߳�ID��Χ��[1, 100] */
        ME_NETWORK_THREAD_ID_BEGIN = 1,

        /* �û��̵߳�ID��Χ��[101, 2000] */
        ME_USER_THREAD_ID_BEGIN = 101,

        /* �ⲿ�̵߳�ID��Χ��[2001, +��] */
        ME_EXTERNAL_THREAD_ID_BEGIN = 2001,
    };

    enum CLASSIFICATION_TYPE
    {
        /* �����߳� */
        CLASSIFICATION_TYPE_NETWORK = 1,

        /* �û��߳� */
        CLASSIFICATION_TYPE_USER = 2,

        /* �ⲿ�߳� */
        CLASSIFICATION_TYPE_EXTERNAL = 3,
    };

    CMEThreadIDManager();
    ~CMEThreadIDManager();

    /**
     *  �����̶߳���Ļ����������ɶ�Ӧ���߳�ID
     *
     *  <iClassificationType>, �̶߳��󻮷�����
     *
     *  ����ֵ:
     *      1-100��ʾ�����̶߳���ID
     *      101-2000��ʾ�û��̶߳���ID
     *      >=2001��ʾ�ⲿ�̶߳���ID
     *      0��ʾ��ǰ�������͵��̶߳����Ѿ�û�п��õ�ID
     */
    INT GenerateThreadID( INT iClassificationType );

    /**
     *  �ж��̶߳����Ƿ�Ϊ�ⲿ�߳�
     *
     *  <iThreadID>, Ҫ�жϵ��̶߳���ID
     *
     *  ����ֵ:
     *      TRUE��ʾ�ⲿ�̶߳���; FALSE��ʾ�ڲ��̶߳���
     */
    BOOL IsExternalThread( INT iThreadID );

    /**
     *  ��ȡ�̶߳���Ļ�������
     *
     *  <iThreadID>, Ҫ�жϵ��̶߳���ID
     *
     *  ����ֵ:
     *      CLASSIFICATION_TYPEö���е�һ��
     */
    INT GetThreadClassificationType( INT iThreadID );

private:
    CMEAtomicOperationT<CMELockMutex> m_iNewNetworkThreadID;
    CMEAtomicOperationT<CMELockMutex> m_iNewUserThreadID;
    CMEAtomicOperationT<CMELockMutex> m_iExternalThreadID;
};


ME_NAME_SPACE_END

#endif // __THREAD_ID_MANAGER_H_

