/** @file       SSLContext.cpp
 *
 *  @note       History
 *  @par        04/24/2016  Created
 */


#include "SSLContext.h"
extern "C" {
#include "openssl/ssl.h"
#include "openssl/err.h"
}

ME_NAME_SPACE_CRYPTO_BEGIN

///< TODO，默认的密钥文件，后面再做一份更正式点的
const char* CMESSLContext::m_pDefaultKeyText =
"\
-----BEGIN RSA PRIVATE KEY-----\n\
Proc-Type: 4,ENCRYPTED\n\
DEK-Info: DES-EDE3-CBC,AE536068A4DB6FE0\n\
\n\
jasnf//RaCr5TYBv2svSkEXM/SLFTyiywdsu/aPlA4CIZfYmqfP7G+xk9viqx9Xt\n\
UyNrusOsX2+hw8hoPAGDXu+wVpeVfLbMrExFxdHfMCKM0hEXhI3FDuOyCOgWYAXw\n\
M1/6/q/Q7RVDaxCoD7eP5gzeMmsbb3cG5ngd8aZcsV1IC9ACcIw0k7qRhBDYQMeq\n\
+JB8XZHz5QA9dvW7bymF4o5Jg+ETOypg/nRA/ari07kqPjpZ4xDXvw91yX7TK2Sx\n\
dbIxHOVu7LTKVc6az47RbBpQCER7tGw7G4AvPYhIaH63UG1qVBDtdyLAcvGPJurx\n\
u088kdkpxV3Nrvm3/7Do1y4vHEmeJA2hSndZrpTPlI+iQ1hrsobw1+Wtfn0GYC62\n\
pnNmHdYe1dpzVdqCq7r7Qt22iosVJ0Jd8xiT/Xl4aFmNZ8R20f4I/+cgDfzo8tag\n\
jhfJQK7LXeLggNEQwC5cDb37ApJWu03Lr11xlHS2MXkICVxWQpEQnkhS2mgddXQ1\n\
vNEBapW74cTqezFiZCSY7+UNzUKeNKZ5B1RUfYqhQVYYDUnR2+gJr8h6Vn6JvxMo\n\
szo3GTSM7Rghl2OdsTcpR/RNiWJDYca/uebmYff4RWAOrs2UnVMsIEZCCWbW/ipd\n\
Rs/1P+qVSttyKHd+oOqcw67tZ+7l2h064cyfiK62joUKeOyBmdO9wFZI6eDSncjo\n\
yf4C0GjNgGPASTP3I5z8HKFhPLMfdzaXeX8q1hwQ8btppb+Z1jQ4jXYojd6nU2kY\n\
gYa92RvPDx35OpPzGlsMBV1Bq/ZgdsnBftQwlOxFitEIfpn2AwDgsQ==\n\
-----END RSA PRIVATE KEY-----\n\
";

///< TODO，默认的证书文件，后面再做一份更正式点的
const char* CMESSLContext::m_pDefaultCertText =
"\
Certificate:\n\
    Data:\n\
        Version: 3 (0x2)\n\
        Serial Number: 4 (0x4)\n\
    Signature Algorithm: md5WithRSAEncryption\n\
        Issuer: C=CN, ST=ZheJiang, L=HangZhou, O=Hikvision, OU=HIK, CN=FangYu/emailAddress=fangyu@hikvision.com\n\
        Validity\n\
            Not Before: Mar 20 09:09:00 2014 GMT\n\
            Not After : Mar 20 09:09:00 2015 GMT\n\
        Subject: CN=server, C=CN, ST=ZheJiang, O=Hikvision, OU=HIK\n\
        Subject Public Key Info:\n\
            Public Key Algorithm: rsaEncryption\n\
                Public-Key: (1024 bit)\n\
                    Modulus:\n\
                        00:bf:ec:d6:b7:5e:dc:f3:6a:d8:67:9d:b1:ff:d1:\n\
                        1c:f5:0a:b0:88:32:c6:29:04:a9:46:b0:1d:a7:f7:\n\
                        6c:dc:ce:44:bf:9a:b1:1f:02:2f:d5:44:8b:13:d5:\n\
                        c7:7b:9e:a7:14:5f:f0:3d:7d:95:f2:35:67:4f:ad:\n\
                        13:46:c6:13:25:0e:11:03:05:bf:79:bd:41:be:ca:\n\
                        54:2d:dd:b8:13:3c:a7:6d:be:93:2c:a9:b7:92:49:\n\
                        91:41:81:88:1d:79:48:d7:c8:df:e4:68:f8:89:24:\n\
                        66:bd:29:aa:05:a1:dd:50:3e:af:28:c9:b9:af:fc:\n\
                        83:d2:6b:7a:01:95:a1:8a:e7\n\
                    Exponent: 65537 (0x10001)\n\
        X509v3 extensions:\n\
            X509v3 Basic Constraints: \n\
                CA:FALSE\n\
            Netscape Comment: \n\
                OpenSSL Generated Certificate\n\
            X509v3 Subject Key Identifier: \n\
                2C:5B:59:1E:D1:1D:C9:9A:CB:26:7F:C7:00:DC:46:78:1F:10:92:0D\n\
            X509v3 Authority Key Identifier: \n\
                keyid:A3:A6:69:D2:8F:72:CB:F4:8C:B7:11:2F:4A:24:A8:E9:00:9F:50:4C\n\
\n\
    Signature Algorithm: md5WithRSAEncryption\n\
        80:b5:f7:e8:eb:9e:c6:f2:d8:02:8c:a2:ec:4b:89:51:0c:5a:\n\
        68:61:bb:db:7c:5c:b9:1a:4a:d8:50:dd:ab:e7:52:77:20:95:\n\
        e0:a4:bf:31:67:13:a8:be:ac:89:07:83:7c:a5:64:e2:87:5d:\n\
        83:0d:91:81:e6:17:78:d3:c3:7b:73:ed:dc:a1:77:91:bb:46:\n\
        7e:40:3e:ff:1c:8d:1c:e1:fc:c3:36:58:2f:0b:32:bd:52:56:\n\
        b9:ff:90:bf:cb:9e:dc:fb:f7:93:6a:12:f9:65:3d:d5:77:6d:\n\
        44:9d:b9:fa:f3:2b:0d:35:f9:fb:9e:2f:0d:21:65:cd:69:bc:\n\
        2e:6b:60:59:fb:e6:a6:55:64:59:a0:da:4f:b7:fb:f2:7a:75:\n\
        a6:f9:26:f9:d0:17:2c:c8:6a:cb:73:9b:c9:06:98:81:d5:0f:\n\
        47:5b:63:a1:02:3f:b1:2a:53:5f:d8:26:80:7f:76:ca:dc:59:\n\
        5e:e3:ca:82:08:b6:33:9e:45:95:9b:a8:d5:07:d6:26:5d:67:\n\
        b3:80:85:66:df:7b:e3:5d:e8:4c:60:36:f1:f3:ac:ac:93:fa:\n\
        e2:47:e0:63:e3:9f:aa:84:0f:4f:e7:b9:fb:5a:f1:e1:08:25:\n\
        d3:25:3d:e9:de:8f:32:50:5f:50:43:87:2f:54:6d:fc:fb:44:\n\
        e7:bc:8d:1d\n\
-----BEGIN CERTIFICATE-----\n\
MIIDUTCCAjmgAwIBAgIBBDANBgkqhkiG9w0BAQQFADCBizELMAkGA1UEBhMCQ04x\n\
ETAPBgNVBAgMCFpoZUppYW5nMREwDwYDVQQHDAhIYW5nWmhvdTESMBAGA1UECgwJ\n\
SGlrdmlzaW9uMQwwCgYDVQQLDANISUsxDzANBgNVBAMMBkZhbmdZdTEjMCEGCSqG\n\
SIb3DQEJARYUZmFuZ3l1QGhpa3Zpc2lvbi5jb20wHhcNMTQwMzIwMDkwOTAwWhcN\n\
MTUwMzIwMDkwOTAwWjBTMQ8wDQYDVQQDDAZzZXJ2ZXIxCzAJBgNVBAYTAkNOMREw\n\
DwYDVQQIDAhaaGVKaWFuZzESMBAGA1UECgwJSGlrdmlzaW9uMQwwCgYDVQQLDANI\n\
SUswgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBAL/s1rde3PNq2Gedsf/RHPUK\n\
sIgyxikEqUawHaf3bNzORL+asR8CL9VEixPVx3uepxRf8D19lfI1Z0+tE0bGEyUO\n\
EQMFv3m9Qb7KVC3duBM8p22+kyypt5JJkUGBiB15SNfI3+Ro+IkkZr0pqgWh3VA+\n\
ryjJua/8g9JregGVoYrnAgMBAAGjezB5MAkGA1UdEwQCMAAwLAYJYIZIAYb4QgEN\n\
BB8WHU9wZW5TU0wgR2VuZXJhdGVkIENlcnRpZmljYXRlMB0GA1UdDgQWBBQsW1ke\n\
0R3Jmssmf8cA3EZ4HxCSDTAfBgNVHSMEGDAWgBSjpmnSj3LL9Iy3ES9KJKjpAJ9Q\n\
TDANBgkqhkiG9w0BAQQFAAOCAQEAgLX36OuexvLYAoyi7EuJUQxaaGG723xcuRpK\n\
2FDdq+dSdyCV4KS/MWcTqL6siQeDfKVk4oddgw2RgeYXeNPDe3Pt3KF3kbtGfkA+\n\
/xyNHOH8wzZYLwsyvVJWuf+Qv8ue3Pv3k2oS+WU91XdtRJ25+vMrDTX5+54vDSFl\n\
zWm8LmtgWfvmplVkWaDaT7f78np1pvkm+dAXLMhqy3ObyQaYgdUPR1tjoQI/sSpT\n\
X9gmgH92ytxZXuPKggi2M55FlZuo1QfWJl1ns4CFZt97413oTGA28fOsrJP64kfg\n\
Y+OfqoQPT+e5+1rx4Qgl0yU96d6PMlBfUEOHL1Rt/PtE57yNHQ==\n\
-----END CERTIFICATE-----\n\
\n\
";


CMESSLContext::CMESSLContext()
    : m_psslCTX( NULL )
    , m_dwVerifyState( SSL_VERIFY_STATE_DISABLED )
{

}

CMESSLContext::~CMESSLContext()
{

}

INT CMESSLContext::PemPasswordCallback(
    char* pPassword,
    INT iSize,
    INT irwFlag,
    void* pUserData )
{
    strncpy(
        pPassword,
        static_cast<const char*>(pUserData),
        iSize );

    pPassword[iSize - 1] = '\0';

    return strlen( pPassword );
}

ME_Result CMESSLContext::Initialize(
    const char* pCertFile,
    const char* pKeyFile,
    const char* pKeyPassword )
{
    ///< 只有两种选择：要么不指定证书、私钥及私钥密码；要么都指定
    ME_ASSERTE_RETURN(
        ((NULL == pCertFile && NULL == pKeyFile && NULL == pKeyPassword) ||
        (NULL != pCertFile && NULL != pKeyFile && NULL != pKeyPassword)),
        ME_ERROR_INVALID_ARG );

    ME_ASSERTE_RETURN( (NULL == m_psslCTX), ME_ERROR_ALREADY_INITIALIZED );

    ///< 加载全局错误信息和算法
    SSL_load_error_strings();
    SSLeay_add_ssl_algorithms();

    ///< 创建SSL上下文
    m_psslCTX = SSL_CTX_new( SSLv23_method() );
    ME_ASSERTE_RETURN( (NULL != m_psslCTX), ME_ERROR_FAILURE );

    X509* pCert = NULL;
    EVP_PKEY* pKey = NULL;
    ME_Result hResult = ME_ERROR_FAILURE;

    ///< 加载证书和私钥
    if ( NULL == pCertFile )
    {
        LoadDefaultCertificate( pCert );
        LoadDefaultPrivateKey( pKey );
    }
    else
    {
        LoadCertificate(
            pCert,
            pCertFile );

        LoadPrivateKey(
            pKey,
            pKeyFile,
            pKeyPassword );
    }

    ///< 在成功读取到证书和私钥的情况下才使用这两者来初始化SSL上下文
    if ( NULL != pCert && NULL != pKey )
    {
         hResult = InitializeSSLContext(
            pCert,
            pKey );
    }

    if ( ME_FAILED(hResult) )
    {
        if ( NULL != pCert )
        {
            X509_free( pCert );
            pCert = NULL;
        }

        if ( NULL != pKey )
        {
            EVP_PKEY_free( pKey );
            pKey = NULL;
        }

        SSL_CTX_free( m_psslCTX );
        m_psslCTX = NULL;
    }

    return hResult;
}

void CMESSLContext::Finalize()
{
    ME_ASSERTE_RETURN_VOID( (NULL != m_psslCTX) );

    SSL_CTX_free( m_psslCTX );
    m_psslCTX = NULL;

    EVP_cleanup();
    ERR_free_strings();
}

ME_Result CMESSLContext::LoadVerifyLocations(
    const char* pCAFile,
    const char* pCADirectory )
{
    ME_ASSERTE_RETURN( (NULL != m_psslCTX), ME_ERROR_NOT_INITIALIZED );

    INT iResult = SSL_CTX_load_verify_locations(
        m_psslCTX,
        pCAFile,
        pCADirectory );

    if ( 1 != iResult )
    {
        //TODO error log

        return ME_ERROR_FAILURE;
    }

    m_dwVerifyState = SSL_VERIFY_STATE_ENABLED;

    return ME_OK;
}

ME_Result CMESSLContext::CreateSSL(
    CMESSLAutoPtr& pSSLAutoPtr,
    DWORD dwType /* = SSL_TYPE_SSL_V23 */,
    const char* pCertFile /* = NULL */,
    const char* pKeyFile /* = NULL */,
    const char* pKeyPassword /* = NULL */ )
{
    ME_ASSERTE_RETURN(
        ((NULL == pCertFile && NULL == pKeyFile && NULL == pKeyPassword) ||
        (NULL != pCertFile && NULL != pKeyFile && NULL != pKeyPassword)),
        ME_ERROR_INVALID_ARG );

    ME_ASSERTE_RETURN( (NULL != m_psslCTX), ME_ERROR_NOT_INITIALIZED );

    SSL* pSSL = SSL_new( m_psslCTX );
    ME_ASSERTE_RETURN( (NULL != pSSL), ME_ERROR_FAILURE );

    ME_Result hResult = ME_ERROR_FAILURE;

    if ( NULL != pCertFile )
    {
        X509* pCert = NULL;
        LoadCertificate(
            pCert,
            pCertFile );

        EVP_PKEY* pKey = NULL;
        LoadPrivateKey(
            pKey,
            pKeyFile,
            pKeyPassword );

        if ( NULL != pCert && NULL != pKey )
        {
            hResult = InitializeSSL(
                pSSL,
                dwType,
                pCert,
                pKey );
        }

        if ( ME_FAILED(hResult) )
        {
            if ( NULL != pCert )
            {
                X509_free( pCert );
                pCert = NULL;
            }

            if ( NULL != pKey )
            {
                EVP_PKEY_free( pKey );
                pKey = NULL;
            }

            SSL_free( pSSL );
            pSSL = NULL;
        }
    }
    else
    {
        hResult = InitializeSSL(
            pSSL,
            dwType,
            NULL,
            NULL );

        if ( ME_FAILED(hResult) )
        {
            SSL_free( pSSL );
            pSSL = NULL;
        }
    }

    if ( NULL == pSSL )
    {
        return hResult;
    }

    pSSLAutoPtr = new CMESSL(
        pSSL,
        dwType,
        m_dwVerifyState);

    return ME_OK;
}


ME_Result CMESSLContext::LoadDefaultCertificate( X509*& pCert )
{
    BIO* pbioCertificateFile = BIO_new( BIO_s_mem() );
    ME_ASSERTE_RETURN( (NULL != pbioCertificateFile), ME_ERROR_FAILURE );

    ME_Result hResult = ME_ERROR_FAILURE;
    pCert = NULL;

    do 
    {
        INT iResult = BIO_write(
            pbioCertificateFile,
            m_pDefaultCertText,
            strlen(m_pDefaultCertText) );

        if ( iResult != strlen(m_pDefaultCertText) )
        {
            //TODO log

            break;
        }

        pCert = PEM_read_bio_X509(
            pbioCertificateFile,
            NULL,
            NULL,
            NULL );

        if ( NULL == pCert )
        {
            //TODO log
        }

        hResult = ME_OK;

    } while ( FALSE );

    BIO_free( pbioCertificateFile );

    return hResult;
}

ME_Result CMESSLContext::LoadCertificate(
    X509*& pCert,
    const char* pCertFile )
{
    BIO* pbioCertificateFile = BIO_new( BIO_s_file() );
    ME_ASSERTE_RETURN( (NULL != pbioCertificateFile), ME_ERROR_FAILURE );

    ME_Result hResult = ME_ERROR_FAILURE;
    pCert = NULL;

    do 
    {
        if ( 1 != BIO_read_filename(pbioCertificateFile, pCertFile) )
        {
            //TODO log

            break;
        }

        pCert = PEM_read_bio_X509(
            pbioCertificateFile,
            NULL,
            NULL,
            NULL );

        if ( NULL == pCert )
        {
            //TODO log
        }

        hResult = ME_OK;

    } while ( FALSE );

    BIO_free( pbioCertificateFile );

    return hResult;
}

ME_Result CMESSLContext::LoadDefaultPrivateKey( EVP_PKEY*& pKey )
{
    BIO* pbioPrivateKeyFile = BIO_new( BIO_s_mem() );
    ME_ASSERTE_RETURN( (NULL != pbioPrivateKeyFile), ME_ERROR_FAILURE );

    ME_Result hResult = ME_ERROR_FAILURE;
    pKey = NULL;

    do 
    {
        INT iResult = BIO_write(
            pbioPrivateKeyFile,
            m_pDefaultKeyText,
            strlen(m_pDefaultKeyText) );

        if ( iResult != strlen(m_pDefaultKeyText) )
        {
            //ERR_print_errors_fp( stderr );

            break;
        }

        const char szDefaultPrivateKeyPassword[] = "123456";
        pKey = PEM_read_bio_PrivateKey(
            pbioPrivateKeyFile,
            NULL,
            PemPasswordCallback,
            (void*)szDefaultPrivateKeyPassword );

        if ( NULL == pKey )
        {
            //TODO log
        }

        hResult = ME_OK;

    } while ( FALSE );

    BIO_free( pbioPrivateKeyFile );

    return hResult;
}

ME_Result CMESSLContext::LoadPrivateKey(
    EVP_PKEY*& pKey,
    const char* pKeyFile,
    const char* pKeyPassword )
{
    BIO* pbioPrivateKeyFile = BIO_new( BIO_s_file() );
    ME_ASSERTE_RETURN( (NULL != pbioPrivateKeyFile), ME_ERROR_FAILURE );

    ME_Result hResult = ME_ERROR_FAILURE;
    pKey = NULL;

    do 
    {
        if ( 1 != BIO_read_filename(pbioPrivateKeyFile, pKeyFile) )
        {
            //TODO log

            break;
        }

        pKey = PEM_read_bio_PrivateKey(
            pbioPrivateKeyFile,
            NULL,
            PemPasswordCallback,
            (void*)pKeyPassword );

        if ( NULL == pKey )
        {
            //TODO log
        }

        hResult = ME_OK;

    } while ( FALSE );

    BIO_free( pbioPrivateKeyFile );

    return hResult;
}

ME_Result CMESSLContext::InitializeSSLContext(
    X509* pCert,
    EVP_PKEY* pKey )
{
    INT iResult = SSL_CTX_use_certificate(
        m_psslCTX,
        pCert );

    if ( 1 != iResult )
    {
        //TODO error log


        return ME_ERROR_FAILURE;
    }

    iResult = SSL_CTX_use_PrivateKey(
        m_psslCTX,
        pKey );

    if ( 1 != iResult )
    {
        //TODO error log


        return ME_ERROR_FAILURE;
    }

    iResult = SSL_CTX_check_private_key(m_psslCTX);
    if ( 1 != iResult )
    {
        //TODO error log


        return ME_ERROR_FAILURE;
    }

    return ME_OK;
}

ME_Result CMESSLContext::InitializeSSL(
    SSL* pSSL,
    DWORD dwType,
    X509* pCert,
    EVP_PKEY* pKey )
{
    const SSL_METHOD* pMethod = NULL;
    ME_Result hResult = GetSSLType(
        pMethod,
        dwType);

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    INT iResult = SSL_set_ssl_method(
        pSSL,
        pMethod );

    if ( 1 != iResult )
    {
        //TODO error log


        return ME_ERROR_FAILURE;
    }

    if ( NULL == pCert )
    {
        return ME_OK;
    }

    iResult = SSL_use_certificate(
        pSSL,
        pCert );

    if ( 1 != iResult )
    {
        //TODO error log


        return ME_ERROR_FAILURE;
    }

    iResult = SSL_use_PrivateKey(
        pSSL,
        pKey );

    if ( 1 != iResult )
    {
        //TODO error log


        return ME_ERROR_FAILURE;
    }

    iResult = SSL_check_private_key( pSSL );

    if ( 1 != iResult )
    {
        //TODO error log


        return ME_ERROR_FAILURE;
    }

    return ME_OK;
}

ME_Result CMESSLContext::GetSSLType(
    const SSL_METHOD*& pMethod,
    DWORD dwType )
{
    ME_Result hResult = ME_OK;

    switch ( dwType )
    {
    case SSL_TYPE_SSL_V23:
        {
            pMethod = SSLv23_method();

            break;
        }

    case SSL_TYPE_SSL_V2:
        {
            pMethod = SSLv2_method();

            break;
        }

    case SSL_TYPE_SSL_V3:
        {
            pMethod = SSLv3_method();

            break;
        }

    case SSL_TYPE_TLS_V1:
        {
            pMethod = TLSv1_method();

            break;
        }

    case SSL_TYPE_TLS_V1_1:
        {
            pMethod = TLSv1_1_method();

            break;
        }

    case SSL_TYPE_TLS_V1_2:
        {
            pMethod = TLSv1_2_method();

            break;
        }

    default:
        {
            pMethod = NULL;

            hResult = ME_ERROR_INVALID_ARG;
        }
    }

    return hResult;
}

ME_NAME_SPACE_CRYPTO_END

