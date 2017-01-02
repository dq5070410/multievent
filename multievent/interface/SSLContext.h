/** @file       SSLContext.h
 *
 *  @note       History
 *  @par        04/23/2016  Created
 */


#ifndef __SSL_CONTEXT_H_
#define __SSL_CONTEXT_H_


#include "Base.h"
#include "SingletonT.h"
#include "SSL.h"


// openssl struct declaration
typedef struct ssl_ctx_st SSL_CTX;
typedef struct x509_st X509;
typedef struct evp_pkey_st EVP_PKEY;
typedef struct ssl_method_st SSL_METHOD;


ME_NAME_SPACE_CRYPTO_BEGIN

class CMESSLContext
{
public:
    enum SSL_TYPE
    {
        SSL_TYPE_NONE = 100,

        SSL_TYPE_SSL_V2,

        SSL_TYPE_SSL_V3,

        SSL_TYPE_TLS_V1,

        SSL_TYPE_TLS_V1_1,

        SSL_TYPE_TLS_V1_2,

        SSL_TYPE_SSL_V23,
    };

    enum SSL_VERIFY_STATE
    {
        SSL_VERIFY_STATE_DISABLED = 0,

        SSL_VERIFY_STATE_ENABLED = 1,
    };

protected:
    friend class CMESingletonT<CMESSLContext>;

    CMESSLContext();

    ~CMESSLContext();

private:
    static INT PemPasswordCallback(
        char* pPassword,
        INT iSize,
        INT irwFlag,
        void* pUserData );

private:
    static const char* m_pDefaultKeyText;

    static const char* m_pDefaultCertText;

public:
    ME_Result Initialize(
        const char* pCertFile,
        const char* pKeyFile,
        const char* pKeyPassword );

    void Finalize();

    ME_Result LoadVerifyLocations(
        const char* pCAFile,
        const char* pCADirectory );

public:
    ME_Result CreateSSL(
        CMESSLAutoPtr& pSSLAutoPtr,
        DWORD dwType = SSL_TYPE_SSL_V23,
        const char* pCertFile = NULL,
        const char* pKeyFile = NULL,
        const char* pKeyPassword = NULL );

private:
    ME_Result LoadDefaultCertificate( X509*& pCert );

    ME_Result LoadCertificate(
        X509*& pCert,
        const char* pCertFile );

    ME_Result LoadDefaultPrivateKey( EVP_PKEY*& pKey );

    ME_Result LoadPrivateKey(
        EVP_PKEY*& pKey,
        const char* pKeyFile,
        const char* pKeyPassword );

    ME_Result InitializeSSLContext(
        X509* pCert,
        EVP_PKEY* pKey );

    ME_Result InitializeSSL(
        SSL* pSSL,
        DWORD dwType,
        X509* pCert,
        EVP_PKEY* pKey );

    ME_Result GetSSLType(
        const SSL_METHOD*& pMethod,
        DWORD dwType );

private:
    SSL_CTX*                        m_psslCTX;

    DWORD                           m_dwVerifyState;
};

typedef CMESingletonT<CMESSLContext> CMESSLContextSingleton;

ME_NAME_SPACE_CRYPTO_END


#endif // __SSL_CONTEXT_H_

