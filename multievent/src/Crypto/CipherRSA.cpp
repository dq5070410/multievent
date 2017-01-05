/** @file       CipherRSA.cpp
 *
 *  @note       History
 *  @par        04/26/2016  Created
 */


#include "CipherRSA.h"

#include "openssl/rsa.h"
#include "openssl/err.h"


ME_NAME_SPACE_CRYPTO_BEGIN

CMECipherRSA::CMECipherRSA()
    : m_prsaPublicKey( NULL )
    , m_prsaPrivateKey( NULL )
{

}

CMECipherRSA::~CMECipherRSA()
{

}

ME_Result CMECipherRSA::GeneratorKeyPair(
    INT iBits,
    UINT iExponent,
    unsigned char* pPrivateKey,
    INT& iPrivateKeyLen,
    unsigned char* pPublicKey,
    INT& iPublicKeyLen )
{
    ME_ASSERTE_RETURN( (MIN_RSA_KEY_BITS <= iBits), ME_ERROR_INVALID_ARG );
    ME_ASSERTE_RETURN( (MAX_RSA_KEY_BITS >= iBits), ME_ERROR_INVALID_ARG );

    ME_ASSERTE_RETURN( (
        PUBLIC_EXPONENT_3 == iExponent ||
        PUBLIC_EXPONENT_17 == iExponent ||
        PUBLIC_EXPONENT_65537 == iExponent),
        ME_ERROR_INVALID_ARG );

    ME_ASSERTE_RETURN( (
        RSA_KEY_BUFFER_LEN <= iPrivateKeyLen &&
        RSA_KEY_BUFFER_LEN <= iPublicKeyLen),
        ME_ERROR_INVALID_ARG );

    BIGNUM* pBNE = BN_new();
    ME_ASSERTE_RETURN( (NULL != pBNE), ME_ERROR_OUT_OF_MEMORY );

    RSA* pRSA = NULL;
    ME_Result hResult = ME_ERROR_FAILURE;

    do 
    {
        pRSA = RSA_new();
        if ( NULL == pRSA )
        {
            break;
        }

        BN_set_word(
            pBNE,
            iExponent );

        INT iResult = RSA_generate_key_ex(
            pRSA,
            iBits,
            pBNE,
            NULL );

        if ( 1 != iResult )
        {
            break;
        }

        unsigned char* pOffset = pPrivateKey;
        iPrivateKeyLen = i2d_RSAPrivateKey(
            pRSA,
            &pOffset );

        pOffset = pPublicKey;
        iPublicKeyLen = i2d_RSAPublicKey(
            pRSA,
            &pOffset );

        hResult = ME_OK;

    } while ( FALSE );

    if ( NULL != pRSA )
    {
        RSA_free( pRSA );

        return hResult;
    }

    BN_free( pBNE );

    return hResult;
}

ME_Result CMECipherRSA::Initialize(
    const unsigned char* pPrivateKey,
    INT iPrivateKeyLen,
    const unsigned char* pPublicKey,
    INT iPubliceKeyLen )
{
    ME_ASSERTE_RETURN( (
        (NULL != pPrivateKey && 0 < iPrivateKeyLen) ||
        (NULL != pPublicKey && 0 < iPubliceKeyLen)),
        ME_ERROR_INVALID_ARG );

    do 
    {
        const unsigned char* pOffset = NULL;

        if ( NULL != pPrivateKey )
        {
            pOffset = pPrivateKey;
            m_prsaPrivateKey = d2i_RSAPrivateKey(
                NULL,
                &pOffset,
                static_cast<long>(iPrivateKeyLen) );

            if ( NULL == m_prsaPrivateKey )
            {
                break;
            }
        }

        if ( NULL != pPublicKey )
        {
            pOffset = pPublicKey;
            m_prsaPublicKey = d2i_RSAPublicKey(
                NULL,
                &pOffset,
                static_cast<long>(iPubliceKeyLen) );

            if ( NULL == m_prsaPublicKey )
            {
                break;
            }
        }

        return ME_OK;

    } while ( FALSE );

    ///< RSA_free自己会判断RSA指针是否为空，所以这里不需要额外的判断
    RSA_free( m_prsaPrivateKey );
    m_prsaPrivateKey = NULL;
    RSA_free( m_prsaPublicKey );
    m_prsaPublicKey = NULL;

    return ME_ERROR_INVALID_ARG;
}

ME_Result CMECipherRSA::PublicEncrypt(
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
    INT iMode /* = MODE_TYPE_RSA_PKCS1_PADDING */ )
{
    ME_ASSERTE_RETURN( (NULL != m_prsaPublicKey), ME_ERROR_NOT_AVAILABLE );
    ME_ASSERTE_RETURN( (pmbBlockAutoPtrIn != NULL), ME_ERROR_INVALID_ARG );
    ME_ASSERTE_RETURN( (pmbBlockAutoPtrIn->GetLength() > 0), ME_ERROR_INVALID_ARG );
    ME_ASSERTE_RETURN( (
        MODE_TYPE_RSA_PKCS1_PADDING == iMode ||
        MODE_TYPE_RSA_PKCS1_OAEP_PADDING == iMode ||
        MODE_TYPE_RSA_SSLV23_PADDING == iMode ||
        MODE_TYPE_RSA_NO_PADDING == iMode),
        ME_ERROR_INVALID_ARG );

    INT iPaddingReservedLen = PADDING_RESERVED_LEN_ZERO;
    if ( MODE_TYPE_RSA_PKCS1_PADDING == iMode ||
        MODE_TYPE_RSA_SSLV23_PADDING == iMode )
    {
        iPaddingReservedLen = PADDING_RESERVED_LEN_PKCS_V_1_5;
    }
    else if ( MODE_TYPE_RSA_PKCS1_OAEP_PADDING == iMode )
    {
        iPaddingReservedLen = PADDING_RESERVED_LEN_OAEP;
    }

    INT iBufferLen = pmbBlockAutoPtrIn->GetLength();
    INT iMaxCipherBufferLen = RSA_size( m_prsaPublicKey );

    if ( MODE_TYPE_RSA_NO_PADDING == iMode )
    {
        ME_ASSERTE_RETURN( (
            iBufferLen == iMaxCipherBufferLen),
            ME_ERROR_INVALID_ARG);
    }
    else
    {
        ME_ASSERTE_RETURN( (
            iBufferLen <= (iMaxCipherBufferLen - iPaddingReservedLen)),
            ME_ERROR_INVALID_ARG );
    }

    unsigned char* pBuffer = new unsigned char[iBufferLen];
    pmbBlockAutoPtrIn->Read(
        pBuffer,
        iBufferLen );

    unsigned char* pCipherBuffer = new unsigned char[iMaxCipherBufferLen];
    INT iResult = RSA_public_encrypt(
        iBufferLen,
        pBuffer,
        pCipherBuffer,
        m_prsaPublicKey,
        iMode );

    delete[] pBuffer;

    if ( RSA_ERROR == iResult )
    {
        //ERR_print_errors_fp( stderr );

        delete[] pCipherBuffer;

        return ME_ERROR_FAILURE;
    }

    ///< 如果加密成功，那么iResult就是加密后的密文长度
    pmbBlockAutoPtrOut = new CMEMessageBlock(
        iMaxCipherBufferLen,
        pCipherBuffer,
        iResult,
        CMEMessageBlock::MESSAGEBLOCK_ATTR_DELETE_USERDATA );

    return ME_OK;
}

ME_Result CMECipherRSA::PrivateDecrypt(
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
    INT iMode /* = MODE_TYPE_RSA_PKCS1_PADDING */ )
{
    ME_ASSERTE_RETURN( (NULL != m_prsaPrivateKey), ME_ERROR_NOT_AVAILABLE );
    ME_ASSERTE_RETURN( (pmbBlockAutoPtrIn != NULL), ME_ERROR_INVALID_ARG );
    ME_ASSERTE_RETURN( (pmbBlockAutoPtrIn->GetLength() > 0), ME_ERROR_INVALID_ARG );
    ME_ASSERTE_RETURN( (
        MODE_TYPE_RSA_PKCS1_PADDING == iMode ||
        MODE_TYPE_RSA_PKCS1_OAEP_PADDING == iMode ||
        MODE_TYPE_RSA_SSLV23_PADDING == iMode ||
        MODE_TYPE_RSA_NO_PADDING == iMode),
        ME_ERROR_INVALID_ARG );

    INT iCipherBufferLen = pmbBlockAutoPtrIn->GetLength();
    INT iMaxBufferLen = RSA_size( m_prsaPrivateKey );

    ME_ASSERTE_RETURN( (iCipherBufferLen == iMaxBufferLen), ME_ERROR_INVALID_ARG );

    unsigned char* pCipherBuffer = new unsigned char[iCipherBufferLen];
    pmbBlockAutoPtrIn->Read(
        pCipherBuffer,
        iCipherBufferLen );

    unsigned char* pBuffer = new unsigned char[iMaxBufferLen];
    INT iResult = RSA_private_decrypt(
        iCipherBufferLen,
        pCipherBuffer,
        pBuffer,
        m_prsaPrivateKey,
        iMode );

    delete[] pCipherBuffer;

    if ( RSA_ERROR == iResult )
    {
        //ERR_print_errors_fp( stderr );

        delete[] pBuffer;

        return ME_ERROR_FAILURE;
    }

    ///< 如果解密成功，那么iResult就是解密后的明文长度
    pmbBlockAutoPtrOut = new CMEMessageBlock(
        iMaxBufferLen,
        pBuffer,
        iResult,
        CMEMessageBlock::MESSAGEBLOCK_ATTR_DELETE_USERDATA );

    return ME_OK;
}

ME_Result CMECipherRSA::PrivateEncrypt(
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
    INT iMode /* = MODE_TYPE_RSA_PKCS1_PADDING */ )
{
    ME_ASSERTE_RETURN( (NULL != m_prsaPrivateKey), ME_ERROR_NOT_AVAILABLE );
    ME_ASSERTE_RETURN( (pmbBlockAutoPtrIn != NULL), ME_ERROR_INVALID_ARG );
    ME_ASSERTE_RETURN( (pmbBlockAutoPtrIn->GetLength() > 0), ME_ERROR_INVALID_ARG );
    ME_ASSERTE_RETURN( (
        MODE_TYPE_RSA_PKCS1_PADDING == iMode ||
        MODE_TYPE_RSA_NO_PADDING == iMode),
        ME_ERROR_INVALID_ARG );

    INT iPaddingReservedLen = PADDING_RESERVED_LEN_ZERO;
    if ( MODE_TYPE_RSA_PKCS1_PADDING == iMode )
    {
        iPaddingReservedLen = PADDING_RESERVED_LEN_PKCS_V_1_5;
    }

    INT iBufferLen = pmbBlockAutoPtrIn->GetLength();
    INT iMaxCipherBufferLen = RSA_size( m_prsaPrivateKey );
    ME_ASSERTE_RETURN( (
        iBufferLen <= (iMaxCipherBufferLen - iPaddingReservedLen)),
        ME_ERROR_INVALID_ARG );

    unsigned char* pBuffer = new unsigned char[iBufferLen];
    pmbBlockAutoPtrIn->Read(
        pBuffer,
        iBufferLen );

    unsigned char* pCipherBuffer = new unsigned char[iMaxCipherBufferLen];
    INT iResult = RSA_private_encrypt(
        iBufferLen,
        pBuffer,
        pCipherBuffer,
        m_prsaPrivateKey,
        iMode );

    if ( RSA_ERROR == iResult )
    {
        //ERR_print_errors_fp( stderr );

        delete[] pCipherBuffer;

        return ME_ERROR_FAILURE;
    }

    ///< 如果加密成功，那么iResult就是加密后的密文长度
    pmbBlockAutoPtrOut = new CMEMessageBlock(
        iMaxCipherBufferLen,
        pCipherBuffer,
        iResult,
        CMEMessageBlock::MESSAGEBLOCK_ATTR_DELETE_USERDATA );

    return ME_OK;
}

ME_Result CMECipherRSA::PublicDecrypt(
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
    INT iMode /* = MODE_TYPE_RSA_PKCS1_PADDING */ )
{
    ME_ASSERTE_RETURN( (NULL != m_prsaPublicKey), ME_ERROR_NOT_AVAILABLE );
    ME_ASSERTE_RETURN( (pmbBlockAutoPtrIn != NULL), ME_ERROR_INVALID_ARG );
    ME_ASSERTE_RETURN( (pmbBlockAutoPtrIn->GetLength() > 0), ME_ERROR_INVALID_ARG );
    ME_ASSERTE_RETURN( (
        MODE_TYPE_RSA_PKCS1_PADDING == iMode ||
        MODE_TYPE_RSA_NO_PADDING == iMode),
        ME_ERROR_INVALID_ARG );

    INT iCipherBufferLen = pmbBlockAutoPtrIn->GetLength();
    INT iMaxBufferLen = RSA_size( m_prsaPublicKey );
    ME_ASSERTE_RETURN( (iCipherBufferLen == iMaxBufferLen), ME_ERROR_INVALID_ARG );

    unsigned char* pCipherBuffer = new unsigned char[iCipherBufferLen];
    pmbBlockAutoPtrIn->Read(
        pCipherBuffer,
        iCipherBufferLen );

    unsigned char* pBuffer = new unsigned char[iMaxBufferLen];
    INT iResult = RSA_public_decrypt(
        iCipherBufferLen,
        pCipherBuffer,
        pBuffer,
        m_prsaPublicKey,
        iMode );

    delete[] pCipherBuffer;

    if ( RSA_ERROR == iResult )
    {
        //ERR_print_errors_fp( stderr );

        delete[] pBuffer;

        return ME_ERROR_FAILURE;
    }

    ///< 如果解密成功，那么iResult就是解密后的明文长度
    pmbBlockAutoPtrOut = new CMEMessageBlock(
        iMaxBufferLen,
        pBuffer,
        iResult,
        CMEMessageBlock::MESSAGEBLOCK_ATTR_DELETE_USERDATA );

    return ME_OK;
}

ME_Result CMECipherRSA::SetOption(
    DWORD dwOptionType,
    void* pOptionValue )
{
    return ME_ERROR_NOT_AVAILABLE;
}

ME_Result CMECipherRSA::GetOption(
    DWORD dwOptionType,
    void*& pOptionValue )
{
    return ME_ERROR_NOT_AVAILABLE;
}

ME_NAME_SPACE_CRYPTO_END

