/** @file       ConnectionOperatorInterface.h
 *
 *  @note       History
 *  @par        04/23/2016  Created
 */


#ifndef __CONNECTION_OPERATOR_INTERFACE_H_
#define __CONNECTION_OPERATOR_INTERFACE_H_


#include "Base.h"
#include "ConnectionInterface.h"
#include "ReferenceControlT.h"
#include "AutoPtrT.h"


ME_NAME_SPACE_BEGIN

class IMEConnectionOperator
    : public IMEReferenceControl
{
public:
    virtual ME_Result PreOnConnect(
        ME_HANDLE hSocket,
        CMETransportAutoPtr& pTransportAutoPtr ) = 0;

    virtual ME_Result SetOption(
        DWORD dwOptionType,
        void* pOptionValue ) = 0;

    virtual ME_Result GetOption(
        DWORD dwOptionType,
        void*& pOptionType ) = 0;

    virtual DWORD GetType() const = 0;

public:
    virtual ~IMEConnectionOperator() {}
};

typedef CMEAutoPtrT<IMEConnectionOperator> IMEConnectionOperatorAutoPtr;

ME_NAME_SPACE_END


#endif // __CONNECTION_OPERATOR_INTERFACE_H_

