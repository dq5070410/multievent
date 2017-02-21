/** @file       ConnectionOperatorSSL.cpp
 *
 *  @note       History
 *  @par        04/29/2016  Created
 */


#include "ConnectionOperatorSSL.h"
#include "SSLContext.h"
#include "ConnectionDefines.h"


ME_NAME_SPACE_BEGIN

CMEConnectionOperatorSSL::CMEConnectionOperatorSSL()
    : m_dwSSLType( CRYPTO::CMESSLContext::SSL_TYPE_SSL_V23 )
{

}

CMEConnectionOperatorSSL::~CMEConnectionOperatorSSL()
{

}

ME_Result CMEConnectionOperatorSSL::SetOption(
    DWORD dwOptionType,
    void* pOptionValue )
{
    ME_Result hResult = ME_ERROR_NOT_AVAILABLE;

    switch ( dwOptionType )
    {
    case CONNECTION_OPTION_TYPE_SSL_TYPE:
        {
            DWORD* pdwSSLType = static_cast<DWORD*>( pOptionValue );
            if ( NULL != pdwSSLType )
            {
                m_dwSSLType = *pdwSSLType;

                hResult = ME_OK;
            }

            break;
        }

    case CONNECTION_OPTION_TYPE_CERTIFICATE_FILE:
        {
            char* psCertificateFile = static_cast<char*>( pOptionValue );
            if ( NULL != psCertificateFile )
            {
                m_sCertificateFile = psCertificateFile;

                hResult = ME_OK;
            }

    	    break;
        }

    case CONNECTION_OPTION_TYPE_PRIVATEKEY_FILE:
        {
            char* psPrivateKeyFile = static_cast<char*>( pOptionValue );
            if ( NULL != psPrivateKeyFile )
            {
                m_sPrivateKeyFile = psPrivateKeyFile;

                hResult = ME_OK;
            }

            break;
        }

    case CONNECTION_OPTION_TYPE_PRIVATEKEY_PASSWORD:
        {
            char* psPrivateKeyPassword = static_cast<char*>( pOptionValue );
            if ( NULL != psPrivateKeyPassword )
            {
                m_sPrivateKeyPassword = psPrivateKeyPassword;

                hResult = ME_OK;
            }

            break;
        }

    default:
        break;
    }

    return hResult;
}

ME_Result CMEConnectionOperatorSSL::GetOption(
    DWORD dwOptionType,
    void*& pOptionType )
{
    return ME_ERROR_NOT_AVAILABLE;
}

DWORD CMEConnectionOperatorSSL::GetType() const
{
    return IConnectionManager::CONNECTION_TYPE_SSL;
}

ME_NAME_SPACE_END

