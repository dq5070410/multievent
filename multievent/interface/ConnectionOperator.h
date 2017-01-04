/** @file       ConnectionOperator.h
 *
 *  @note       History
 *  @par        04/23/2016  Created
 */


#ifndef __CONNECTION_OPERATOR_H_
#define __CONNECTION_OPERATOR_H_


#include "ConnectionOperatorInterface.h"


ME_NAME_SPACE_BEGIN

class CMEConnectionOperator
    : public IMEConnectionOperator
    , public CMEReferenceControlT<CMELockMutex>
{
protected:
    CMEConnectionOperator();

public:
    virtual ~CMEConnectionOperator();

public:
    virtual ME_Result PreOnConnect(
        ME_HANDLE hSocket,
        CMETransportAutoPtr& pTransportAutoPtr );

    virtual ME_Result SetOption(
        DWORD dwOptionType,
        void* pOptoinValue );

    virtual ME_Result GetOption(
        DWORD dwOptionType,
        void*& pOptionType );

public:
    DWORD AddReference();

    DWORD RemoveReference();
};

ME_NAME_SPACE_END


#endif // __CONNECTION_OPERATOR_H_

