/** @file       CipherAES.cpp
 *
 *  @note       History
 *  @par        04/25/2016  Created
 */


#include "CipherAES.h"

#include "openssl/evp.h"


ME_NAME_SPACE_CRYPTO_BEGIN

CMECipherAES::CMECipherAES()
{
    m_pCipherCTX = EVP_CIPHER_CTX_new();
}

CMECipherAES::~CMECipherAES()
{
    EVP_CIPHER_CTX_free( m_pCipherCTX );
}

ME_Result CMECipherAES::Initialize(
    const char* pszKey,
    const char* pszIV )
{
    ME_ASSERTE_RETURN( (NULL != pszKey), ME_ERROR_INVALID_ARG );

    m_sKey = pszKey;
    ME_ASSERTE_RETURN( !(m_sKey.empty()), ME_ERROR_INVALID_ARG );

    if ( NULL != pszIV )
    {
        m_sIV = pszIV;
    }

    return ME_OK;
}

ME_Result CMECipherAES::Encrypt(
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
    DWORD dwMode /* = MODE_TYPE_AES_128_ECB */ )
{
    ME_ASSERTE_RETURN( (pmbBlockAutoPtrIn != NULL), ME_ERROR_INVALID_ARG );
    ME_ASSERTE_RETURN( (pmbBlockAutoPtrIn->GetLength() > 0), ME_ERROR_INVALID_ARG );

    ///< 如果不设置IV，那么将不能使用XTS模式
    ME_ASSERTE_RETURN(
        !((MODE_TYPE_AES_128_XTS == dwMode) &&
        m_sIV.empty()), ME_ERROR_NOT_AVAILABLE );

    const EVP_CIPHER* pCipher = NULL;
    ME_Result hResult = GetCipher(
        dwMode,
        pCipher );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    UINT iBufferLen = pmbBlockAutoPtrIn->GetLength();
    unsigned char* pBuffer = new unsigned char[iBufferLen];
    pmbBlockAutoPtrIn->Read(
        pBuffer,
        iBufferLen );

    UINT iCipherBufferCapacity = iBufferLen + EVP_MAX_BLOCK_LENGTH;
    INT iCipherBufferLen = 0;
    INT iOffset = 0;
    unsigned char* pCipherBuffer = new unsigned char[iCipherBufferCapacity];

    EVP_CIPHER_CTX_init( m_pCipherCTX );

    do 
    {
        INT iResult = EVP_EncryptInit_ex(
            m_pCipherCTX,
            pCipher,
            NULL,
            reinterpret_cast<const unsigned char*>(m_sKey.data()),
            m_sIV.empty() ? NULL : reinterpret_cast<const unsigned char*>(m_sIV.data()) );

        if ( 1 != iResult )
        {
            break;
        }

        iResult = EVP_EncryptUpdate(
            m_pCipherCTX,
            pCipherBuffer,
            &iOffset,
            pBuffer,
            iBufferLen );

        if ( 1 != iResult )
        {
            break;
        }

        iCipherBufferLen += iOffset;

        iResult = EVP_EncryptFinal_ex(
            m_pCipherCTX,
            pCipherBuffer + iCipherBufferLen,
            &iOffset );

        if ( 1 != iResult )
        {
            break;
        }

        iCipherBufferLen += iOffset;

        pmbBlockAutoPtrOut = new CMEMessageBlock(
            iCipherBufferCapacity,
            pCipherBuffer,
            iCipherBufferLen,
            CMEMessageBlock::MESSAGEBLOCK_ATTR_DELETE_USERDATA );

        EVP_CIPHER_CTX_cleanup( m_pCipherCTX );

        delete[] pBuffer;

        return ME_OK;

    } while ( FALSE );

    EVP_CIPHER_CTX_cleanup( m_pCipherCTX );

    delete[] pBuffer;
    delete[] pCipherBuffer;

    return ME_ERROR_FAILURE;
}

ME_Result CMECipherAES::Decrypt(
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
    DWORD dwMode /* = MODE_TYPE_AES_128_ECB */ )
{
    ME_ASSERTE_RETURN( (pmbBlockAutoPtrIn != NULL), ME_ERROR_INVALID_ARG );
    ME_ASSERTE_RETURN( (pmbBlockAutoPtrIn->GetLength() > 0), ME_ERROR_INVALID_ARG );

    const EVP_CIPHER* pCipher = NULL;
    ME_Result hResult = GetCipher(
        dwMode,
        pCipher );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    UINT iCipherBufferLen = pmbBlockAutoPtrIn->GetLength();
    unsigned char* pCipherBuffer = new unsigned char[iCipherBufferLen];
    pmbBlockAutoPtrIn->Read(
        pCipherBuffer,
        iCipherBufferLen );

    UINT iBufferCapacity = iCipherBufferLen + EVP_MAX_BLOCK_LENGTH;
    INT iBufferLen = 0; 
    INT iOffset = 0;
    unsigned char* pBuffer = new unsigned char[iBufferCapacity];

    EVP_CIPHER_CTX_init( m_pCipherCTX );

    do 
    {
        INT iResult = EVP_DecryptInit_ex(
            m_pCipherCTX,
            pCipher,
            NULL,
            reinterpret_cast<const unsigned char*>(m_sKey.data()),
            m_sIV.empty() ? NULL : reinterpret_cast<const unsigned char*>(m_sIV.data()) );

        if ( 1 != iResult )
        {
            break;
        }

        iResult = EVP_DecryptUpdate(
            m_pCipherCTX,
            pBuffer,
            &iOffset,
            pCipherBuffer,
            iCipherBufferLen );

        if ( 1 != iResult )
        {
            break;
        }

        iBufferLen += iOffset;

        iResult = EVP_DecryptFinal_ex(
            m_pCipherCTX,
            pBuffer + iBufferLen,
            &iOffset );

        if ( 1 != iResult )
        {
            break;
        }

        iBufferLen +=iOffset;

        pmbBlockAutoPtrOut = new CMEMessageBlock(
            iBufferCapacity,
            pBuffer,
            iBufferLen,
            CMEMessageBlock::MESSAGEBLOCK_ATTR_DELETE_USERDATA );

        EVP_CIPHER_CTX_cleanup( m_pCipherCTX );

        delete[] pCipherBuffer;

        return ME_OK;

    } while ( FALSE );

    EVP_CIPHER_CTX_cleanup( m_pCipherCTX );

    delete[] pCipherBuffer;
    delete[] pBuffer;

    return ME_ERROR_FAILURE;
}

ME_Result CMECipherAES::SetOption(
    DWORD dwOptionType,
    void* pOptionValue )
{
    switch ( dwOptionType )
    {
    case OPTION_TYPE_KEY:
        {
            std::string* psKey = static_cast<std::string*>( pOptionValue );
            ME_ASSERTE_RETURN( (NULL != psKey), ME_ERROR_INVALID_ARG );
            ME_ASSERTE_RETURN( (!psKey->empty()), ME_ERROR_INVALID_ARG );

            m_sKey = *psKey;

            return ME_OK;
        }

    case OPTION_TYPE_IV:
        {
            std::string* psIV = static_cast<std::string*>( pOptionValue );
            ME_ASSERTE_RETURN( (NULL != psIV), ME_ERROR_INVALID_ARG );

            m_sIV = *psIV;

            return ME_OK;
        }

    default:
        {
            return ME_ERROR_NOT_AVAILABLE;
        }
    }
}

ME_Result CMECipherAES::GetOption(
    DWORD dwOptionType,
    void*& pOptionValue )
{
    return ME_ERROR_NOT_AVAILABLE;
}

ME_Result CMECipherAES::GetCipher(
    DWORD dwMode,
    const EVP_CIPHER*& pCipher )
{
    switch ( dwMode )
    {
    case MODE_TYPE_AES_128_ECB:
        {
            pCipher = EVP_aes_128_ecb();

    	    break;
        }

    case MODE_TYPE_AES_128_CBC:
        {
            pCipher = EVP_aes_128_cbc();

            break;
        }

    case MODE_TYPE_AES_128_CFB_1:
        {
            pCipher = EVP_aes_128_cfb1();

            break;
        }

    case MODE_TYPE_AES_128_CFB_8:
        {
            pCipher = EVP_aes_128_cfb8();

            break;
        }

    case MODE_TYPE_AES_128_CFB_128:
        {
            pCipher = EVP_aes_128_cfb128();

            break;
        }

    case MODE_TYPE_AES_128_OFB:
        {
            pCipher = EVP_aes_128_ofb();

            break;
        }

    case MODE_TYPE_AES_128_CTR:
        {
            pCipher = EVP_aes_128_ctr();

            break;
        }

    ///< CCM和GCM模式要求的加解密参数比其他的模式更多且加解密流程也不一样，暂时
    ///< 不开放这两种模式
//     case MODE_TYPE_AES_128_CCM:
//         {
//             pCipher = EVP_aes_128_ccm();
// 
//             break;
//         }
// 
//     case MODE_TYPE_AES_128_GCM:
//         {
//             pCipher = EVP_aes_128_gcm();
// 
//             break;
//         }

    case MODE_TYPE_AES_128_XTS:
        {
            pCipher = EVP_aes_128_xts();

            break;
        }

    default:
        {
            pCipher = NULL;

            return ME_ERROR_INVALID_ARG;
        }
    }

    return ME_OK;
}

ME_NAME_SPACE_CRYPTO_END

