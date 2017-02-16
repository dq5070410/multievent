/** @file       AcceptorOperatorSSL.h
 *
 *  @note       History
 *  @par        04/23/2014  Created
 */


#ifndef __ACCEPTOR_OPERATOR_SSL_H_
#define __ACCEPTOR_OPERATOR_SSL_H_


#include "ConnectionOperatorSSL.h"


ME_NAME_SPACE_BEGIN

class CMEAcceptorOperatorSSL
    : public CMEConnectionOperatorSSL
{
public:
    CMEAcceptorOperatorSSL();

    ~CMEAcceptorOperatorSSL();

public:
    ME_Result PreOnConnect(
        ME_HANDLE hSocket,
        CTransportAutoPtr& pTransportAutoPtr );
};

ME_NAME_SPACE_END


#endif // __ACCEPTOR_OPERATOR_SSL_H_

