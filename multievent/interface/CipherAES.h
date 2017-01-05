/** @file       CipherAES.h
 *
 *  @note       History
 *  @par        04/23/2014  Created
 */


#ifndef __CIPHER_AES_H_
#define __CIPHER_AES_H_


#include "Base.h"
#include "MessageBlock.h"


// openssl struct declaration
typedef struct evp_cipher_ctx_st EVP_CIPHER_CTX;
typedef struct evp_cipher_st EVP_CIPHER;


ME_NAME_SPACE_CRYPTO_BEGIN

class ME_OS_EXPORT CMECipherAES
{
public:
    enum MODE_TYPE
    {
        MODE_TYPE_AES_128_ECB = 200,

        MODE_TYPE_AES_128_CBC,

        MODE_TYPE_AES_128_CFB_1,

        MODE_TYPE_AES_128_CFB_8,

        MODE_TYPE_AES_128_CFB_128,

        MODE_TYPE_AES_128_OFB,

        MODE_TYPE_AES_128_CTR,

        MODE_TYPE_AES_128_CCM,

        MODE_TYPE_AES_128_GCM,

        MODE_TYPE_AES_128_XTS,
    };

    enum OPTION_TYPE
    {
        OPTION_TYPE_NONE = 300,

        OPTION_TYPE_KEY,

        OPTION_TYPE_IV,
    };

public:
    CMECipherAES();

    ~CMECipherAES();

public:
    ME_Result Initialize(
        const char* pszKey,
        const char* pszIV );

    ME_Result Encrypt(
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
        DWORD dwMode = MODE_TYPE_AES_128_ECB );

    ME_Result Decrypt(
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
        DWORD dwMode = MODE_TYPE_AES_128_ECB );

    ME_Result SetOption(
        DWORD dwOptionType,
        void* pOptionValue );

    ME_Result GetOption(
        DWORD dwOptionType,
        void*& pOptionValue );

private:
    ME_Result GetCipher(
        DWORD dwMode,
        const EVP_CIPHER*& pCipher );

private:
    EVP_CIPHER_CTX*                 m_pCipherCTX;

    std::string                     m_sKey;

    std::string                     m_sIV;
};

ME_NAME_SPACE_CRYPTO_END


#endif // __CIPHER_AES_H_

