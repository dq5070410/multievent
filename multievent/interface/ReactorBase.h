/**
 *  �¼���·������ʵ�ֻ���������
 *
 *
 *  History
 *  01/26/2016          create file
 */

#ifndef __REACTOR_BASE_H_
#define __REACTOR_BASE_H_

#include "Base.h"
#include "ReactorImpl.h"
#include "LockThreadMutex.h"

ME_NAME_SPACE_BEGIN

class IMEWakener;

/**
 *  �¼���·������ʵ�ֻ�����
 *
 *  �̳��¼���·������ʵ�ֽӿڣ�ʵ���¼���·�����������Ĳ��֣��������¼���·����
 *  �����Լ�IO����Ĳ���ע���ע�������ɲ�ͬ��ʵ�����
 *
 */
class CMEReactorBase : public IMEReactorImpl
{
public:
    virtual ME_Result Notify( INT iNotifyType );

    virtual ME_Result Register(
        IMEIO* pIO,
        IMEIOSink* pIOSink,
        INT iEvent,
        INT& iIndex );

    virtual ME_Result UnRegister(
        IMEIO* pIO,
        INT iEvent,
        INT iIndex );

    virtual ME_Result AttachEventQueue( CMEEventQueue* pEq );

    virtual ME_Result AttachTimerQueue( CMETimerQueue* pTq );

    virtual ME_Result Dispatch( BOOL bExit = FALSE );

	/* 2013.9.4 added by Τ�� */
	virtual int GetType() const;

protected:
    /**
     *  ���캯��
     *
     *  <pWakener>, Reactor�Ļ�����
     *  <iType>, Reactor������
     *
     */
    CMEReactorBase(
        IMEWakener* pWakener,
        INT iType );

    /**
     *  ��ȡ��·����ʱ�ĵȴ���ʱʱ�䣬���ʱ�������¼������Ƿ�����¼��Լ�ʱ��
     *  �����������ʱ�ӵ����������
     *
     *  ����ֵ:
     *      >=0��ʾ�ȴ���ʱ��ľ��������; ME_INFINITE��ʾ�ȴ�ʱ��Ϊ���޳�
     */
    UINT GetWaitTime();

    /**
     *  ��ӿɵȴ�������������ɾ���ķ������ʵ����ʵ�֣���IO����ת��Ϊ����
     *  �ܹ��ȴ��Ķ���
     *
     *  <pIO>, IO����
     *  <iEvent>, ��ע���¼�����
     *  <iIndex>, IO������Reactor���������
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result AddWaitableObject(
        IMEIO* pIO,
        INT iEvent,
        INT iIndex ) = 0;

    /**
     *  �Ƴ��ɵȴ�������������ɾ���ķ������ʵ����ʵ�֣��Ƴ�IO�����Ӧ�Ŀɵ�
     *  ������
     *
     *  <pIO>, IO����
     *  <iEvent>, ��ע���¼�����
     *  <iIndex>, IO������Reactor���������
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result RemoveWaitableObject(
        IMEIO* pIO,
        INT iEvent,
        INT iIndex ) = 0;

    // ��ֱֹ��delete CMEReactorBase����
    virtual ~CMEReactorBase();

private:
    /**
     *  �жϷ������Ƿ��пռ�����ע��IO�����¼�
     *
     *  ����ֵ:
     *      TRUE��ʾ������ע��ռ�����; FALSE��ʾ���������пռ�����ע��
     */
    BOOL IsFull();

    /* ��ֹĬ�Ϲ��캯�� */
    CMEReactorBase();

    /* ��ֹ���� */
    CMEReactorBase( const CMEReactorBase& );
    const CMEReactorBase& operator = ( const CMEReactorBase& );

	// 2013.4.11 edited by Τ��
//private:
protected:
    /*
     *  IOItem�������洢ע�������IO���󣬹�ע��IO�¼������Լ�IO�¼�����ص�����
     *  Ȼ�����ܹ���ʶ������󴥷���ʲô�¼�
     */ 
    typedef struct tagIOItem
    {
        IMEIO* pIO;                // �ܹ�ע��IO����ʵ��ָ��
        IMEIOSink* pIOSink;        // �ܹ�ע��IO�����¼��ص�ʵ��ָ��
        INT iEvent;                 // �ܹ�ע���¼�����
        INT iActiveEvent;           // ���������ʵ�ʷ������¼�����
    } ME_IO_ITEM;

    /* Index-IOItem Map��������������IO Item */
    typedef std::map<INT, ME_IO_ITEM> HikIOItemMap;

protected:
    /* ��Ҫ�ɷ���������������Wakener */
    friend class CMEReactorFactory;

    /* ����IO�¼���IO�������� */
    INT                             m_iActiveIOItemNum;

    /* ���ע���IO����IO�¼������Լ�IO�¼��ص���Map�� */
    HikIOItemMap                    m_ioItemMap;

    /* �������Ļ�����ʵ��ָ�� */
    IMEWakener*                    m_pWakener;

    /* ���������̰߳�ȫ�� */
    CMELockThreadMutex             m_hThreadMutex;

private:
    /* �µ�IO����������������µ�IO����ע���������ô������������IO�������� */
    INT                             m_iNewIndex;

    /* ��������״̬ */
    INT                             m_iStatus;

    /* ������������ */
    INT                             m_iType;

    /* ���������ӵ��¼����� */
    CMEEventQueue*                 m_pEventQueue;

    /* ���������ӵ�ʱ�Ӷ��� */
    CMETimerQueue*                 m_pTimerQueue;
};

ME_NAME_SPACE_END

#endif // __REACTOR_BASE_H_

