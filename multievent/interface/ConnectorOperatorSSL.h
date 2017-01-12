/** @file       HikConnectorOperatorSSL.h
 *  @copyright  HangZhou Hikvision System Technology Co., Ltd.
 *              All Right Reserved.
 *  @brief
 *
 *  @author     ∑Ω”Ó  (fangyu@hikvision.com)
 *
 *  @note       History
 *  @par        04/23/2014  Created
 */


#ifndef __ME_CONNECTOR_OPERATOR_SSL_H_
#define __ME_CONNECTOR_OPERATOR_SSL_H_


#include "HikConnectionOperatorSSL.h"


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
        CMETransportAutoPtr& pTransportAutoPtr );
};

ME_NAME_SPACE_END


#endif // __ME_CONNECTOR_OPERATOR_SSL_H_

