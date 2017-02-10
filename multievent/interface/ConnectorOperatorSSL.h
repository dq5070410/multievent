/** @file       ConnectorOperatorSSL.h
 *
 *  @note       History
 *  @par        04/23/2016  Created
 */


#ifndef __ME_CONNECTOR_OPERATOR_SSL_H_
#define __ME_CONNECTOR_OPERATOR_SSL_H_


#include "ConnectionOperatorSSL.h"


ME_NAME_SPACE_BEGIN

class CMEConnectorOperatorSSL
    : public CMEConnectionOperatorSSL
{
public:
    CMEConnectorOperatorSSL();

    ~CMEConnectorOperatorSSL();

public:
    ME_Result PreOnConnect(
        ME_HANDLE hSocket,
        CTransportAutoPtr& pTransportAutoPtr );
};

ME_NAME_SPACE_END


#endif // __ME_CONNECTOR_OPERATOR_SSL_H_

