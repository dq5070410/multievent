/**
 *  ����Windows�¼��Ļ���������
 *
 *  History
 *  01/28/2016 create file
 */

#ifndef __WAKENER_EVENT_H_
#define __WAKENER_EVENT_H_

#include "Wakener.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

/**
 *  ����Windows�¼��Ļ�������
 *
 *  ��Ӧ��ʹ��WaitForMultipleObjects��ʽ�ķ�������CMEReactorWFMO��Ŀǰû�ҵ�ʹ
 *  �ú�����ʱWaitForMultipleObjects�����ķ�ʽ����ʱ����Windows�ں��¼�����֪ͨ��
 *  ��Ȼ���ܻ��˷�һ���ȴ���λ�ã�����Ŀǰ����������ʵ����Ϊ������
 *
 */
class CMEWakenerEvent : public IMEWakener
{
public:
    virtual void SetHandle( ME_HANDLE hHandle );

    virtual ME_HANDLE GetHandle() const;

    virtual void OnInput(
        ME_HANDLE hHandle,
        int iEvent );

    virtual void OnOutput(
        ME_HANDLE hHandle,
        int iEvent );

    virtual void OnClose(
        ME_HANDLE hHandle,
        int iEvent );

    virtual ME_Result Notify();

    virtual ME_Result Initialize();

    virtual ME_Result Finalize();

private:
    friend class CMEReactorFactory;
    friend class CMEWakenerEventTester;

    CMEWakenerEvent();
    virtual ~CMEWakenerEvent();

private:
    ME_EVENT                       m_hEvent;
};

ME_NAME_SPACE_END

#endif	// ME_WIN

#endif // __WAKENER_EVENT_H_

