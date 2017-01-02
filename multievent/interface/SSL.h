/** @file       SSL.h
 *
 *  @note       History
 *  @par        04/23/2016  Created
 */


#ifndef __SSL_H_
#define __SSL_H_


#include "Base.h"
#include "MessageBlock.h"


// openssl struct declaration
typedef struct ssl_st SSL;
typedef struct bio_st BIO;


ME_NAME_SPACE_CRYPTO_BEGIN

class CMESSL
    : public CMEReferenceControlT<CMELockMutex>
{
public:
    enum SSL_VERIFY_MODE
    {
        SSL_VERIFY_MODE_NONE = 0x00, // SSL_VERIFY_NONE

        SSL_VERIFY_MODE_VERIFY_PEER = 0x01, //SSL_VERIFY_PEER

        SSL_VERIFY_MODE_VERIFY_FAIL_IF_NO_PEER_CERT = 0x02, //SSL_VERIFY_FAIL_IF_NO_PEER_CERT

        SSL_VERIFY_MODE_VERIFY_CLIENT_ONCE = 0x04, //SSL_VERIFY_CLIENT_ONCE
    };

    static const UINT READ_BLOCK_SIZE = 1024;

public:
    CMESSL(
        SSL* pSSL,
        DWORD dwType,
        DWORD dwVerifyState );

    ~CMESSL();

public:
    ME_Result SetVerify(
        DWORD dwMode,
        INT iMaxDepth = 0 );

    ME_Result HandsShake(
        ME_HANDLE hSocket,
        BOOL bServer );

    ME_Result Encrypt(
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut );

    ME_Result Decrypt(
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut );

private:
    CMESSL();

    CMESSL( const CMESSL& );

    const CMESSL& operator = ( const CMESSL& );

private:
    SSL*                            m_pSSL;

    DWORD                           m_dwType;

    DWORD                           m_dwVerifyState;

    BIO*                            m_pbioReadMem;

    BIO*                            m_pbioWriteMem;

    INT                             m_iPendingLen;
};

typedef CMEAutoPtrT<CMESSL> CMESSLAutoPtr;

ME_NAME_SPACE_CRYPTO_END


#endif // __SSL_H_

