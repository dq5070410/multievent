/**
 *  �¼���·������ʵ����ӿڶ���
 *
 *
 *  History
 *  01/26/2016          create file
 */


#ifndef __REACTOR_IMPL_H_
#define __REACTOR_IMPL_H_

#include "Base.h"
#include "EventQueueT.h"
#include "TimerQueueT.h"
#include "LockThreadMutex.h"

ME_NAME_SPACE_BEGIN

class IMEIO;
class IMEIOSink;
class IMECustomizeEvent;
class CMETimer;
class ITimerSink;


/**
 *  �¼���·������ʵ����ӿ�
 *
 *  �����¼���·��������ʵ�ֽӿڣ������¼���·��������ʵ���඼Ӧ�ô������̳�
 *
 *  note
 */
class IMEReactorImpl
{
public:
    typedef CMEEventQueueT<IMECustomizeEvent, CMELockThreadMutex> CMEEventQueue;
    typedef CMEEventQueue::NodeType::CMESyncEvent CMESyncEventType;
    typedef CMETimerQueueT<CMETimer, ITimerSink, CMELockThreadMutex> CMETimerQueue;

public:
    virtual ~IMEReactorImpl() {}

    /**
     *  ֪ͨ�����������ȴ���������ͨ��ʹ�ò���ϵͳ�ṩ�Ķ�·�������������IO�¼�
     *  ���ԣ����������ڲ���ʱ�ȴ��¼������������ʱ��Ҫ���߷��������Զ����¼���
     *  ���/ȡ����ʱ�ӣ���ô����Ҫ��������ӿ��÷�������ʱ�ӵȴ��л��ѣ�֪ͨ��
     *  �����л�ʱ�Ӷ���ȥ���ʹ����Զ����¼���ʱ���¼������⣬���˳�ʱ��Ҳ��Ҫ
     *  ֪ͨ�����������ٵȴ�����ʱ�����˳�����
     *
     *  <iNotifyType>, ֪ͨ���ͣ����ѻ�ֹͣ
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     *
     *  �ο�:
     *      CMEReactor::NOTIFY_TYPE
     */
    virtual ME_Result Notify( INT iNotifyType ) = 0;

    /**
     *  ע���ע��IO����IO�¼������Լ��¼�����ص���ע��ɹ��󣬷��������Ḻ��
     *  �Թ�ע��IO�����IO�¼����в��񣬲���IO�¼��󣬽������ע��ʱ������¼�
     *  ����ص���֪ͨ��ע��
     *
     *  ��Ҫע����ǣ��ܹ��ظ�ע��ͬһ��IO���󣬲��Ҳ��ø���ԭֵ�ķ�ʽ�޸���ע��
     *  ��IO�¼����ͣ����������Ҫ��ע�ڸ߲�������µ�Ч������
     *
     *  <pIO>, ��ע��IO����
     *  <pIOSink>, IO������¼�����ص�
     *  <iEvent>, ��ע��IO�¼�
     *  <iIndex>, ����IO�����ڷ�����������ֵ�������Ҫ�޸Ĺ�ע��IO������¼�����
     *            ���ע��ɹ����IO�����ڷ�����������ֵ����ע��ʱ��Ҫ�ṩ������
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�, �������ֵ>0; ��ME_OK��ʾʧ��, �������ֵΪ-1
     *
     *  �ο�:
     *      UnRegister
     */
    virtual ME_Result Register(
        IMEIO* pIO,
        IMEIOSink* pIOSink,
        INT iEvent,
        INT& iIndex ) = 0;

    /**
     *  ����ע�ᵽ��������IO������¼�ע������ע������û�й�ע���¼����ͣ�������
     *  �����ٲ���ע����IO�����IO�¼�����ȻҲ�����ٵ����¼�����ص�
     *
     *  ��Ҫע����ǣ�ע���ǲ���ȡ���������ڹ�ע����¼�����ʱ������ֻע������һ
     *  �ֶ������������¼����͵Ĺ�ע
     *
     *  <pIO>, IO����
     *  <iEvent>, ��Ҫע�����¼�����
     *  <iIndex>, IO�����ڷ������е��������������ֵ��ע��ʱ��ȡ
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     *
     *  �ο�:
     *      Register
     */
    virtual ME_Result UnRegister(
        IMEIO* pIO,
        INT iEvent,
        INT iIndex ) = 0;

    /**
     *  ���ӵ��¼����У�ֻ������һ�Σ���һ�γɹ��󣬺����ٳ��Ըò������᷵��
     *  ����
     *
     *  <pEq>, ��Ҫ�����������ӵ��¼�����
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result AttachEventQueue( CMEEventQueue* pEq ) = 0;

    /**
     *  ���ӵ�ʱ�Ӷ��У�ֻ������һ�Σ���һ�γɹ��󣬺����ٳ��Ըò������᷵��
     *  ����
     *
     *  <pTq>, ��Ҫ�����������ӵ�ʱ�Ӷ���
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result AttachTimerQueue( CMETimerQueue* pTq ) = 0;

    /**
     *  ����IO�¼������ò���ϵͳ�ṩ�Ķ�·������ƣ��磺select��epoll_wait��
     *  WaitForMultipleObjects�ȡ�
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result HandleEvent() = 0;

    /**
     *  �����¼�����ͨ��HandleEvent�����¼��Ժ󣨻��߳�ʱ����ͨ��Dispatch����
     *  ���¼������ɷ����������һ��������״̬ʱ��Ӧ��������Զ����¼����������
     *  ��ʱȥ����IO�¼����������ʱ��Ҫ�����¼����д����¼������������һ������
     *  ��״̬ʱ��IO�����Ѿ�ֹͣ����ʱ����Ҫ��ʱ����н����е��Զ����¼�����һ��
     *  �� ��Ȼ�������¼��ķ�ʽ���¼���������ɣ�������ֻ�����ڲ�ͬ�����������
     *  ��Ӧ��֪ͨ��
     *
     *  <bExit>, ָ���Ƿ����˳�״̬
     *
     *  ����ֵ:
     *      RUN_STATE_RUNNING����ʾ��������������״̬
     *      RUN_STATE_EXIT����ʾ��鵽�����������˳�״̬
     */
    virtual ME_Result Dispatch( BOOL bExit = FALSE ) = 0;

	/* 2013.9.4 added by Τ�� */
	virtual int GetType() const = 0;
};

ME_NAME_SPACE_END

#endif // __REACTOR_IMPL_H_

