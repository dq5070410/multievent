/** @file       DigestMD5.cpp
 *
 *  @note       History
 *  @par        04/28/2016  Created
 */


#include "DigestMD5.h"

#include "openssl/evp.h"
#include "openssl/hmac.h"


ME_NAME_SPACE_CRYPTO_BEGIN

CMEDigestMD5::CMEDigestMD5()
{
    m_pDigestCTX = new EVP_MD_CTX;
    m_pmacCTX = new HMAC_CTX;
}

CMEDigestMD5::~CMEDigestMD5()
{
    ME_DELETE( m_pDigestCTX );
    ME_DELETE( m_pmacCTX );
}

ME_Result CMEDigestMD5::Digest(
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
    DWORD dwMode /* = MODE_TYPE_MD5 */,
    const char* pKey /* = NULL */,
    INT iKeyLen /* = 0 */ )
{
    ME_ASSERTE_RETURN( (pmbBlockAutoPtrIn != NULL), ME_ERROR_INVALID_ARG );
    ME_ASSERTE_RETURN( (pmbBlockAutoPtrIn->GetLength() > 0), ME_ERROR_INVALID_ARG );

    const EVP_MD* pDigest = NULL;
    ME_Result hResult = GetDigest(
        dwMode,
        pDigest );

    ME_ASSERTE_RETURN( (NULL != pDigest), hResult );

    if ( NULL == pKey )
    {
        return Digest(
            pmbBlockAutoPtrIn,
            pmbBlockAutoPtrOut,
            pDigest );
    }
    else
    {
        return HMAC(
            pmbBlockAutoPtrIn,
            pmbBlockAutoPtrOut,
            pDigest,
            pKey,
            iKeyLen );
    }
}

ME_Result CMEDigestMD5::SetOption(
    DWORD dwOptionType,
    void* pOptionValue )
{
    return ME_ERROR_NOT_AVAILABLE;
}

ME_Result CMEDigestMD5::GetOption(
    DWORD dwOptionType,
    void*& pOptionValue )
{
    return ME_ERROR_NOT_AVAILABLE;
}

ME_Result CMEDigestMD5::Digest(
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
    const EVP_MD* pDigest )
{
    UINT iBufferLen = pmbBlockAutoPtrIn->GetLength();
    char* pBuffer = new char[iBufferLen];
    pmbBlockAutoPtrIn->Read(
        pBuffer,
        iBufferLen );

    unsigned char* pDigestBuffer = new unsigned char[EVP_MAX_MD_SIZE];
    UINT iDigestBufferLen = 0;

    EVP_MD_CTX_init( m_pDigestCTX );

    do
    {
        INT iResult = EVP_DigestInit_ex(
            m_pDigestCTX,
            pDigest,
            NULL );

        if ( 1 != iResult )
        {
            break;
        }

        iResult = EVP_DigestUpdate(
            m_pDigestCTX,
            pBuffer,
            iBufferLen );

        if ( 1 != iResult )
        {
            break;
        }

        iResult = EVP_DigestFinal_ex(
            m_pDigestCTX,
            pDigestBuffer,
            &iDigestBufferLen );

        if ( 1 != iResult )
        {
            break;
        }

        pmbBlockAutoPtrOut = new CMEMessageBlock(
            EVP_MAX_MD_SIZE,
            pDigestBuffer,
            iDigestBufferLen,
            CMEMessageBlock::MESSAGEBLOCK_ATTR_DELETE_USERDATA );

        EVP_MD_CTX_cleanup( m_pDigestCTX );

        delete[] pBuffer;

        return ME_OK;

    } while ( FALSE );

    EVP_MD_CTX_cleanup( m_pDigestCTX );

    delete[] pBuffer;
    delete[] pDigestBuffer;

    return ME_ERROR_FAILURE;
}

ME_Result CMEDigestMD5::HMAC(
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
    const EVP_MD* pDigest,
    const char* pKey,
    INT iKeyLen )
{
    ME_ASSERTE_RETURN( (NULL != pKey && 0 < iKeyLen), ME_ERROR_INVALID_ARG );

    UINT iBufferLen = pmbBlockAutoPtrIn->GetLength();
    unsigned char* pBuffer = new unsigned char[iBufferLen];
    pmbBlockAutoPtrIn->Read(
        pBuffer,
        iBufferLen );

    unsigned char* phmacBuffer = new unsigned char[EVP_MAX_MD_SIZE];
    UINT ihmacBufferLen = 0;

    HMAC_CTX_init( m_pmacCTX );

    do 
    {
        INT iResult = HMAC_Init_ex(
            m_pmacCTX,
            pKey,
            iKeyLen,
            pDigest,
            NULL );

        if ( 1 != iResult )
        {
            break;
        }

        iResult = HMAC_Update(
            m_pmacCTX,
            pBuffer,
            iBufferLen );

        if ( 1 != iResult )
        {
            break;
        }

        iResult = HMAC_Final(
            m_pmacCTX,
            phmacBuffer,
            &ihmacBufferLen );

        if ( 1 != iResult )
        {
            break;
        }

        HMAC_CTX_cleanup( m_pmacCTX );

        pmbBlockAutoPtrOut = new CMEMessageBlock(
            EVP_MAX_MD_SIZE,
            phmacBuffer,
            ihmacBufferLen,
            CMEMessageBlock::MESSAGEBLOCK_ATTR_DELETE_USERDATA );

        delete[] pBuffer;

        return ME_OK;

    } while ( FALSE );

    HMAC_CTX_cleanup( m_pmacCTX );

    delete[] pBuffer;
    delete[] phmacBuffer;

    return ME_ERROR_FAILURE;
}

ME_Result CMEDigestMD5::GetDigest(
    DWORD dwMode,
    const EVP_MD*& pDigest )
{
    switch ( dwMode )
    {
    case MODE_TYPE_MD5:
        {
            pDigest = EVP_md5();

    	    return ME_OK;
        }

    default:
        {
            pDigest = NULL;

            return ME_ERROR_NOT_FOUND;
        }
    }
}

ME_NAME_SPACE_CRYPTO_END

