/** @file       CipherDES.h
 *
 *  @note       History
 *  @par        04/23/2016  Created
 */


#ifndef __CIPHER_DES_H_
#define __CIPHER_DES_H_


#include "Base.h"
#include "MessageBlock.h"


// openssl struct declaration
typedef struct evp_cipher_ctx_st EVP_CIPHER_CTX;
typedef struct evp_cipher_st EVP_CIPHER;


ME_NAME_SPACE_CRYPTO_BEGIN

class ME_OS_EXPORT CMECipherDES
{
public:
    enum MODE_TYPE
    {
        MODE_TYPE_DES_ECB = 100,

        MODE_TYPE_DES_CBC,

        MODE_TYPE_DES_CFB,

        MODE_TYPE_DES_OFB,

        MODE_TYPE_DES_EDE,

        MODE_TYPE_DES_EDE_CBC,

        MODE_TYPE_DES_EDE_CFB,

        MODE_TYPE_DES_EDE_OFB,

        MODE_TYPE_DES_EDE3,

        MODE_TYPE_DES_EDE3_CBC,

        MODE_TYPE_DES_EDE3_CFB,

        MODE_TYPE_DES_EDE3_OFB,
    };

    enum OPTION_TYPE
    {
        OPTION_TYPE_NONE = 300,

        OPTION_TYPE_KEY,

        OPTION_TYPE_IV,
    };

public:
    CMECipherDES();

    ~CMECipherDES();

public:
    ME_Result Initialize(
        const char* pszKey,
        const char* pszIV );

    ME_Result Encrypt(
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
        DWORD dwMode = MODE_TYPE_DES_ECB );

    ME_Result Decrypt(
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
        DWORD dwMode = MODE_TYPE_DES_ECB );

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


#endif // __CIPHER_DES_H_

