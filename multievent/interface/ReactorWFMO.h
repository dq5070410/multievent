/**
 *  ����WaitForMultipleObjects�ķ�����������
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
 *  ����WaitForMultipleObjects�ķ�����
 *
 *  Ŀǰʹ��WaitForMultipleObjects��ʽ��ԵĶ�ֻ�Ǵ�����¼����������socket����
 *  ���ԣ��������������͵ķ�����Ŀǰֻ�����û��߳��������Զ����¼���ʱ�ӣ����¸�
 *  �汾�л��Ϊ�ܹ�����socket io�¼�
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

