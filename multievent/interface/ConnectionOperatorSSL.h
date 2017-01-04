/** @file       ConnectionOperatorSSL.h
 *
 *  @note       History
 *  @par        04/23/2016  Created
 */


#ifndef __CONNECTION_OPERATOR_SSL_H_
#define __CONNECTION_OPERATOR_SSL_H_


#include "ConnectionOperator.h"
#include "SSL.h"


ME_NAME_SPACE_BEGIN

class CMEConnectionOperatorSSL
    : public CMEConnectionOperator
{
protected:
    CMEConnectionOperatorSSL();

public:
    virtual ~CMEConnectionOperatorSSL();

public:
    virtual ME_Result SetOption(
        DWORD dwOptionType,
        void* pOptoinValue );

    virtual ME_Result GetOption(
        DWORD dwOptionType,
        void*& pOptionType );

    DWORD GetType() const;

protected:
    CRYPTO::CMESSLAutoPtr          m_pSSLAutoPtr;

    DWORD                           m_dwSSLType;

    std::string                     m_sCertificateFile;

    std::string                     m_sPrivateKeyFile;

    std::string                     m_sPrivateKeyPassword;
};

ME_NAME_SPACE_END


#endif // __ME_CONNECTION_OPERATOR_SSL_H_

