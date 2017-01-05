/**
 *  基于Select的分离器类声明
 *
 *
 *  History
 *  01/28/2016 create file
 */

#ifndef __REACTOR_SELECT_H_
#define __REACTOR_SELECT_H_


#include "ReactorBase.h"

/* 先启用整体拷贝fd_set的方式，最终这部分应该移到预编译配置或其他编译配置文件中 */
#ifndef ENABLE_FD_SET_COPY
#define ENABLE_FD_SET_COPY
#endif

ME_NAME_SPACE_BEGIN

/**
 *  基于Select的分离器
 *
 *  使用select作为多路分离机制，目前有两种实现方式，一种是准备两组fd_set，发生IO
 *  对象变化时改变master组的fd_set，每次调用select时，则从master组的fd_set直接拷
 *  贝到working组的fd_set；另一种则是在每次调用select时，都需要根据当前的IO Item
 *  表，逐一初始化select所需的fd_set
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
    /* Master组fd_set */
    fd_set                          m_MasterFDSetIn;
    fd_set                          m_MasterFDSetOut;
    fd_set                          m_MasterFDSetErr;

    #if defined( ME_LINUX )
    /* Linux下还有Master组NFDS */
    INT                             m_iMasterNFDS;
    #endif
#endif
};

ME_NAME_SPACE_END

#endif // __REACTOR_SELECT_H_

