/** @file       DigestSHA.h
 *
 *  @note       History
 *  @par        04/23/2016  Created
 */


#ifndef __ME_DIGEST_SHA_H_
#define __ME_DIGEST_SHA_H_


#include "Base.h"
#include "MessageBlock.h"

// openssl struct declaration
typedef struct env_md_st EVP_MD;
typedef struct env_md_ctx_st EVP_MD_CTX;
typedef struct hmac_ctx_st HMAC_CTX;


ME_NAME_SPACE_CRYPTO_BEGIN

class ME_OS_EXPORT CMEDigestSHA
{
public:
    enum MODE_TYPE
    {
        MODE_TYPE_SHA1 = 100,

        MODE_TYPE_SHA224,

        MODE_TYPE_SHA256,

        MODE_TYPE_SHA384,

        MODE_TYPE_SHA512,
    };

public:
    CMEDigestSHA();

    ~CMEDigestSHA();

public:
    ME_Result Digest(
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
        DWORD dwMode = MODE_TYPE_SHA1,
        const char* pKey = NULL,
        INT iKeyLen = 0 );

    ME_Result SetOption(
        DWORD dwOptionType,
        void* pOptionValue );

    ME_Result GetOption(
        DWORD dwOptionType,
        void*& pOptionValue );

private:
    ME_Result Digest(
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
        const EVP_MD* pDigest );

    ME_Result HMAC(
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
        const EVP_MD* pDigest,
        const char* pKey,
        INT iKeyLen  );

    ME_Result GetDigest(
        DWORD dwMode,
        const EVP_MD*& pDigest );

private:
    EVP_MD_CTX*                     m_pDigestCTX;

    HMAC_CTX*                       m_pmacCTX;
};

ME_NAME_SPACE_CRYPTO_END


#endif // __DIGEST_SHA_H_

