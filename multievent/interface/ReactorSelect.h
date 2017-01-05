/**
 *  ����Select�ķ�����������
 *
 *
 *  History
 *  01/28/2016 create file
 */

#ifndef __REACTOR_SELECT_H_
#define __REACTOR_SELECT_H_


#include "ReactorBase.h"

/* ���������忽��fd_set�ķ�ʽ�������ⲿ��Ӧ���Ƶ�Ԥ�������û��������������ļ��� */
#ifndef ENABLE_FD_SET_COPY
#define ENABLE_FD_SET_COPY
#endif

ME_NAME_SPACE_BEGIN

/**
 *  ����Select�ķ�����
 *
 *  ʹ��select��Ϊ��·������ƣ�Ŀǰ������ʵ�ַ�ʽ��һ����׼������fd_set������IO
 *  ����仯ʱ�ı�master���fd_set��ÿ�ε���selectʱ�����master���fd_setֱ�ӿ�
 *  ����working���fd_set����һ��������ÿ�ε���selectʱ������Ҫ���ݵ�ǰ��IO Item
 *  ����һ��ʼ��select�����fd_set
 *
 */
class CMEReactorSelect : public CMEReactorBase
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

    ME_Result InitWorkingFDSet(
        fd_set& stWorkingFDSetIn,
        fd_set& stWorkingFDSetOut,
        fd_set& stWorkingFDSetErr );

    void FeedbackWorkingFDSet(
        fd_set& stWorkingFDSetIn,
        fd_set& stWorkingFDSetOut,
        fd_set& stWorkingFDSetErr,
        INT iActiveNum );

private:
    friend class CMEReactorFactory;

    CMEReactorSelect( IMEWakener* pWakener );
    virtual ~CMEReactorSelect();

#if defined( ENABLE_FD_SET_COPY )
private:
    /* Master��fd_set */
    fd_set                          m_MasterFDSetIn;
    fd_set                          m_MasterFDSetOut;
    fd_set                          m_MasterFDSetErr;

    #if defined( ME_LINUX )
    /* Linux�»���Master��NFDS */
    INT                             m_iMasterNFDS;
    #endif
#endif
};

ME_NAME_SPACE_END

#endif // __REACTOR_SELECT_H_

