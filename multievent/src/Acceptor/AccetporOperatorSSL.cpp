/** @file       AccetporOperatorSSL.cpp
 *  @par        04/29/2016  Created
 */


#include "AcceptorOperatorSSL.h"
#include "SSLContext.h"
#include "LockThreadMutex.h"


using ME::CRYPTO::CMESSLContextSingleton;


ME_NAME_SPACE_BEGIN

CMEAcceptorOperatorSSL::CMEAcceptorOperatorSSL()
{

}

CMEAcceptorOperatorSSL::~CMEAcceptorOperatorSSL()
{

}

ME_Result CMEAcceptorOperatorSSL::PreOnConnect(
    ME_HANDLE hSocket,
    CMETransportAutoPtr& pTransportAutoPtr )
{
    ME_Result hResult = CMESSLContextSingleton::Instance()->CreateSSL(
        m_pSSLAutoPtr,
        m_dwSSLType,
        m_sCertificateFile.empty() ? NULL : m_sCertificateFile.c_str(),
        m_sPrivateKeyFile.empty() ? NULL : m_sPrivateKeyFile.c_str(),
        m_sPrivateKeyPassword.empty() ? NULL : m_sPrivateKeyPassword.c_str() );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    hResult = m_pSSLAutoPtr->HandsShake(
        hSocket,
        TRUE );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    pTransportAutoPtr->SetOption(
        IMETransport::OPTION_TYPE_SET_SSL,
        m_pSSLAutoPtr.Get() );

    return hResult;
}

ME_NAME_SPACE_END

