/**
 *  �̶߳��󹤳�������
 *
 *  History
 *  02/19/2013 create file
 */


#ifndef __THREAD_FACTORY_H_
#define __THREAD_FACTORY_H_

#include "Base.h"
#include "SingletonT.h"
#include "LockGuardT.h"

ME_NAME_SPACE_BEGIN

class CMEThread;
class IMEThreadSink;

class CMEThreadFactory
{
public:
    /**
     *  �����̶߳���
     *
     *  <bBlock>, ָ���̶߳����Ƿ�����
     *  <iThreadID>, ָ���̶߳����ID�����IDӦ���̶߳��������ָ����
     *  <iThreadType>, �̶߳��������
     *  <iReactorType>, ���������ͣ�����̶߳��󲻴��з���������ô�����Դ˲���
     *  <pThreadSink>, �û��߼��ص�
     *  <pThread>, ����̶߳���ʵ��ָ�룬����ɹ����������NULL
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    ME_Result Create(
        BOOL bBlock,
        INT iThreadID,
        INT iThreadType,
		INT iClassificationType,	// 2013.9.16 added by Τ��
        INT iReactorType,
        IMEThreadSink* pThreadSink,
        CMEThread*& pThread );

    /**
     *  �����̶߳���ʵ��
     *
     *  <pThread>, �̶߳���ʵ��ָ��
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    ME_Result Destroy( CMEThread*& pThread );

private:
	/**
     *  ���ݲ�ͬ������ʹ�������
     *
     *  <pQueue>, out, ����ʵ��ָ��
	 *  <iType>, �߳�����(�û��߳�/�����߳�/����)
     *
     *  ����ֵ:
     *      ��
     */
	template <class Queue>
	void CreateQueue( 
		Queue*& pQueue,
		int iType );
};

typedef CMESingletonT<CMEThreadFactory> CMEThreadFactorySingleton;

ME_NAME_SPACE_END

#endif // __THREAD_FACTORY_H_

