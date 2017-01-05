/** @file       CipherDES.cpp
 *
 *  @note       History
 *  @par        04/25/2016  Created
 */


#include "CipherDES.h"

#include "openssl/evp.h"


ME_NAME_SPACE_CRYPTO_BEGIN

CMECipherDES::CMECipherDES()
{
    m_pCipherCTX = EVP_CIPHER_CTX_new();
}

CMECipherDES::~CMECipherDES()
{
    EVP_CIPHER_CTX_free( m_pCipherCTX );
}

ME_Result CMECipherDES::Initialize(
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

ME_Result CMECipherDES::Encrypt(
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
    DWORD dwMode )
{
    ME_ASSERTE_RETURN( (pmbBlockAutoPtrIn != NULL), ME_ERROR_INVALID_ARG );
    ME_ASSERTE_RETURN( (pmbBlockAutoPtrIn->GetLength() > 0), ME_ERROR_INVALID_ARG );

    ///< 除ECB，EDE，EDE3以外的模式理论上都需要初始化向量来完成加密，但是OpenSSL
    ///< 的实现并不强制要求传入IV，所以这里暂时不强制要求
//     ME_ASSERTE_RETURN(
//         !((MODE_TYPE_DES_ECB != dwMode &&
//         MODE_TYPE_DES_EDE != dwMode &&
//         MODE_TYPE_DES_EDE3 != dwMode) &&
//         (m_sIV.empty())),
//         ME_ERROR_NOT_AVAILABLE );

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

ME_Result CMECipherDES::Decrypt(
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
    DWORD dwMode )
{
    ME_ASSERTE_RETURN( (pmbBlockAutoPtrIn != NULL), ME_ERROR_INVALID_ARG );
    ME_ASSERTE_RETURN( (pmbBlockAutoPtrIn->GetLength() > 0), ME_ERROR_INVALID_ARG );

    ///< 除ECB，EDE，EDE3以外的模式理论上都需要初始化向量来完成加密，但是OpenSSL
    ///< 的实现并不强制要求传入IV，所以这里暂时不强制要求
//     ME_ASSERTE_RETURN(
//         !((MODE_TYPE_DES_ECB != dwMode &&
//         MODE_TYPE_DES_EDE != dwMode &&
//         MODE_TYPE_DES_EDE3 != dwMode) &&
//         (m_sIV.empty())),
//         ME_ERROR_NOT_AVAILABLE );

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

ME_Result CMECipherDES::SetOption(
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

ME_Result CMECipherDES::GetOption(
    DWORD dwOptionType,
    void*& pOptionValue )
{
    return ME_ERROR_NOT_AVAILABLE;
}

ME_Result CMECipherDES::GetCipher(
    DWORD dwMode,
    const EVP_CIPHER*& pCipher )
{
    switch ( dwMode )
    {
    case MODE_TYPE_DES_ECB:
        {
            pCipher = EVP_des_ecb();

            break;
        }

    case MODE_TYPE_DES_CBC:
        {
            pCipher = EVP_des_ecb();

            break;
        }

    case MODE_TYPE_DES_CFB:
        {
            pCipher = EVP_des_cfb();

            break;
        }

    case MODE_TYPE_DES_OFB:
        {
            pCipher = EVP_des_ofb();

            break;
        }

    case MODE_TYPE_DES_EDE:
        {
            pCipher = EVP_des_ede();

            break;
        }

    case MODE_TYPE_DES_EDE_CBC:
        {
            pCipher = EVP_des_ede_cbc();

            break;
        }

    case MODE_TYPE_DES_EDE_CFB:
        {
            pCipher = EVP_des_ede_cfb();

            break;
        }

    case MODE_TYPE_DES_EDE_OFB:
        {
            pCipher = EVP_des_ede_ofb();

            break;
        }

    case MODE_TYPE_DES_EDE3:
        {
            pCipher = EVP_des_ede3();

            break;
        }

    case MODE_TYPE_DES_EDE3_CBC:
        {
            pCipher = EVP_des_ede3_cbc();

            break;
        }

    case MODE_TYPE_DES_EDE3_CFB:
        {
            pCipher = EVP_des_ede3_cfb();

            break;
        }

    case MODE_TYPE_DES_EDE3_OFB:
        {
            pCipher = EVP_des_ede3_ofb();

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

