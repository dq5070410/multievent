/** @file       TransportOperatorSSL.h
 *
 *  @note       History
 *  @par        04/23/2016  Created
 */


#ifndef __ME_TRANSPORT_OPERATOR_SSL_H_
#define __ME_TRANSPORT_OPERATOR_SSL_H_


#include "TransportOperatorBase.h"
#include "SSL.h"


ME_NAME_SPACE_BEGIN

class CMETransportOperatorSSL
    : public CMETransportOperatorBase
{
public:
    CMETransportOperatorSSL();

    ~CMETransportOperatorSSL();

public:
    ME_Result PreSendData( CMEMessageBlock*& pmbBlock );

    ME_Result PostSendData( CMEMessageBlock*& pmbBlock );

    ME_Result PreOnCanSendAgain();

    ME_Result PreOnReceive( CMEMessageBlock*& pmbBlock );

    ME_Result PostOnReceive( CMEMessageBlock*& pmbBlock );

    ME_Result SetOption( DWORD dwOptionType, VOID* pOptionValue );

    DWORD GetType();

private:
    CRYPTO::CMESSLAutoPtr          m_pSSLAutoPtr;

    CMEMessageBlockAutoPtr         m_pmbMessgeBlockIn;

    CMEMessageBlockAutoPtr         m_pmbMessgeBlockOut;

    BOOL                            m_bNeedNewPackage;
};

ME_NAME_SPACE_END


#endif // __TRANSPORT_OPERATOR_SSL_H_

