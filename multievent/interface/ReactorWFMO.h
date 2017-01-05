/**
 *  基于WaitForMultipleObjects的分离器类声明
 *
 *
 *  History
 *  01/28/2016 create file
 */

#ifndef __REACTORWFMO_H_
#define __REACTORWFMO_H_

#include "ReactorBase.h"
#include "ConnectionDefines.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

/**
 *  基于WaitForMultipleObjects的分离器
 *
 *  目前使用WaitForMultipleObjects方式面对的都只是纯粹的事件句柄（不是socket），
 *  所以，基本上这种类型的分离器目前只用作用户线程来捕获自定义事件和时钟，在下个
 *  版本中会改为能够捕获socket io事件
 *
 */
class CMEReactorWFMO : public CMEReactorBase
{
public:
    virtual ME_Result HandleEvent();

protected:
    virtual ME_Result AddWaitableObject(
        IMEIO* pIO,
        INT iEvent,
        INT iIndex );

    virtual ME_Result RemoveWaitableObject(
        IMEIO* pIO,
        INT iEvent,
        INT iIndex );

private:
    friend class CMEReactorFactory;

    CMEReactorWFMO( IMEWakener* pWakener );
    virtual ~CMEReactorWFMO();

private:
    //HANDLE                          m_EventArray[ME_WFMO_MAX_WAIT_NUM];
	ME_HANDLE                      m_EventArray[ME_WFMO_MAX_WAIT_NUM];
    INT                             m_Slot[ME_WFMO_MAX_WAIT_NUM];
};

ME_NAME_SPACE_END

#endif	// ME_WIN

#endif // __REACTORWFMO_H_

