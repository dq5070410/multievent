/**
 *  基于Windows事件的唤醒者声明
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
 *  基于Windows事件的唤醒者类
 *
 *  对应于使用WaitForMultipleObjects方式的分离器：CMEReactorWFMO，目前没找到使
 *  用函数来时WaitForMultipleObjects跳出的方式，暂时先用Windows内核事件来做通知，
 *  虽然可能会浪费一个等待的位置，但是目前来讲这样的实现最为清晰简单
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

