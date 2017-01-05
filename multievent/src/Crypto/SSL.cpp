/** @file       SSL.cpp
 *
 *  @note       History
 *  @par        04/24/2016  Created
 */


#include "SSL.h"
#include "SSLContext.h"

#include "OSSocket.h"

#include "openssl/ssl.h"
#include "openssl/err.h"

//#include <iostream>


ME_NAME_SPACE_CRYPTO_BEGIN

CMESSL::CMESSL(
    SSL* pSSL,
    DWORD dwType,
    DWORD dwVerifyState )
    : m_pSSL( pSSL )
    , m_dwType( dwType )
    , m_dwVerifyState( dwVerifyState )
    , m_pbioWriteMem( NULL )
    , m_pbioReadMem( NULL )
{

}

CMESSL::~CMESSL()
{
    SSL_free( m_pSSL );
}

ME_Result CMESSL::SetVerify(
    DWORD dwMode,
    INT iMaxDepth /* = 0 */ )
{
    ME_ASSERTE_RETURN(
        (m_dwVerifyState == CMESSLContext::SSL_VERIFY_STATE_ENABLED),
        ME_ERROR_NOT_AVAILABLE );

    SSL_set_verify(
        m_pSSL,
        dwMode,
        NULL );

    if ( 0 != iMaxDepth)
    {
        SSL_set_verify_depth(
            m_pSSL,
            iMaxDepth );
    }

    return ME_OK;
}

ME_Result CMESSL::HandsShake(
    ME_HANDLE hSocket,
    BOOL bServer )
{
    ///< 因为Transport在Open之前，Socket还没有被设置为非阻塞模式，所以可以直接
    ///< 拿来做SSL握手通信
    INT iResult = SSL_set_fd(
        m_pSSL,
        (int)hSocket );

    ME_ASSERTE_RETURN( (1 == iResult), ME_ERROR_FAILURE );

    if ( bServer )
    {
        iResult = SSL_accept( m_pSSL );
    }
    else
    {
        iResult = SSL_connect( m_pSSL );
    }

    ME_ASSERTE_RETURN( (1 == iResult), ME_ERROR_FAILURE );

    //std::cout << "SSL connection using " << SSL_get_cipher( m_pSSL ) << std::endl;

    m_pbioReadMem = BIO_new( BIO_s_mem() );
    m_pbioWriteMem = BIO_new( BIO_s_mem() );

    SSL_set_bio(
        m_pSSL,
        m_pbioReadMem,
        m_pbioWriteMem );

    return ME_OK;
}

ME_Result CMESSL::Encrypt(
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut )
{
    UINT iBufferLen = pmbBlockAutoPtrIn->GetLength();
    char* pBuffer = new char[iBufferLen];
    pmbBlockAutoPtrIn->Read(
        pBuffer,
        iBufferLen );

    INT iWriteLen = SSL_write(
        m_pSSL,
        pBuffer,
        iBufferLen );

    INT iCipherBufferLen = BIO_pending( m_pbioWriteMem );
    char* pCipherBuffer = new char[iCipherBufferLen];
    INT iReadLen = BIO_read(
        m_pbioWriteMem,
        pCipherBuffer,
        iCipherBufferLen );

    pmbBlockAutoPtrOut = new CMEMessageBlock(
        iCipherBufferLen,
        pCipherBuffer,
        iCipherBufferLen,
        CMEMessageBlock::MESSAGEBLOCK_ATTR_DELETE_USERDATA );

    delete[] pBuffer;

    ///< 先不管是不是所有进来的数据都已经加密了
    return ME_OK;
}

ME_Result CMESSL::Decrypt(
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
    CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut )
{
    char* pCipherBuffer = NULL;
    INT iWriteLen = 0;

    //TODO
    UINT iCipherBufferLen = pmbBlockAutoPtrIn->GetLength();
    if ( 0 < iCipherBufferLen )
    {
        pCipherBuffer = new char[iCipherBufferLen];
        pmbBlockAutoPtrIn->Read(
            pCipherBuffer,
            iCipherBufferLen );

        iWriteLen = BIO_write(
            m_pbioReadMem,
            pCipherBuffer,
            iCipherBufferLen );
    }

    ME_Result hResult = ME_ERROR_FAILURE;

    INT iReadLen = 0;
    do 
    {
        char* pBuffer = new char[READ_BLOCK_SIZE];
        iReadLen = SSL_read(
            m_pSSL,
            pBuffer,
            READ_BLOCK_SIZE );

        if ( 0 >= iReadLen )
        {
            //ERR_print_errors_fp( stderr );

            delete[] pBuffer;

            hResult = ME_ERROR_NETWORK_PARTIAL_DATA;

            break;
        }

        if ( pmbBlockAutoPtrOut == NULL )
        {
            pmbBlockAutoPtrOut = new CMEMessageBlock(
                READ_BLOCK_SIZE,
                pBuffer,
                iReadLen,
                CMEMessageBlock::MESSAGEBLOCK_ATTR_DELETE_USERDATA );
        }
        else
        {
            pmbBlockAutoPtrOut->Append(
                CMEMessageBlock(
                    READ_BLOCK_SIZE,
                    pBuffer,
                    iReadLen,
                    CMEMessageBlock::MESSAGEBLOCK_ATTR_DELETE_USERDATA) );
        }

        hResult = ME_OK;

    } while ( iReadLen == READ_BLOCK_SIZE );

    delete[] pCipherBuffer;

    return hResult;
}

ME_NAME_SPACE_CRYPTO_END

