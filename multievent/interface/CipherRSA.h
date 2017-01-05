/** @file       CipherRSA.h

 *
 *  @note       History
 *  @par        04/23/2016  Created
 */


#ifndef __CIPHER_RSA_H_
#define __CIPHER_RSA_H_


#include "Base.h"
#include "MessageBlock.h"


// openssl struct declaration
typedef struct rsa_st RSA;


ME_NAME_SPACE_CRYPTO_BEGIN

class ME_OS_EXPORT CMECipherRSA
{
public:
    enum MODE_TYPE
    {
        MODE_TYPE_RSA_PKCS1_PADDING = 1, //RSA_PKCS1_PADDING

        MODE_TYPE_RSA_PKCS1_OAEP_PADDING = 4, //RSA_PKCS1_OAEP_PADDING

        ///< ������䷽ʽĿǰ�ڹ�Կ���ܡ���˽Կ���ܹ����лᷢ�������ʧ�ܵ�����
        ///< ��CSDN�ϲ鵽˵��OpenSSL��Դ����󣬵���û�������ط���ͬ���ĶԸ�����
        ///< ��˵�����������������û�еõ�OpenSSL�ٷ���ʽ˵����������Ӧ�ĸĶ�֮
        ///< ǰ���벻Ҫʹ��������䷽ʽ
        MODE_TYPE_RSA_SSLV23_PADDING = 2, //RSA_SSLV23_PADDING

        MODE_TYPE_RSA_NO_PADDING = 3, //RSA_NO_PADDING
    };

    enum PUBLIC_EXPONENT
    {
        PUBLIC_EXPONENT_3 = 3,

        PUBLIC_EXPONENT_17 = 17,

        PUBLIC_EXPONENT_65537 = 65537,
    };

    enum PADDING_RESERVED_LEN
    {
        PADDING_RESERVED_LEN_ZERO = 0,

        PADDING_RESERVED_LEN_PKCS_V_1_5 = 11,

        PADDING_RESERVED_LEN_OAEP = 41,
    };

    static const INT MIN_RSA_KEY_BITS = 96;

    static const INT MAX_RSA_KEY_BITS = 2048;

    static const int RSA_KEY_BUFFER_LEN = 2048;

    static const int RSA_ERROR = -1;

public:
    CMECipherRSA();

    ~CMECipherRSA();

public:
    static ME_Result GeneratorKeyPair(
        INT iBits,
        UINT iExponent,
        unsigned char* pPrivateKey,
        INT& iPrivateKeyLen,
        unsigned char* pPublicKey,
        INT& iPublicKeyLen );

public:
    ME_Result Initialize(
        const unsigned char* pPrivateKey,
        INT iPrivateKeyLen,
        const unsigned char* pPublicKey,
        INT iPubliceKeyLen );

    ME_Result PublicEncrypt(
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
        INT iMode = MODE_TYPE_RSA_PKCS1_PADDING );

    ME_Result PrivateDecrypt(
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
        INT iMode = MODE_TYPE_RSA_PKCS1_PADDING );

    ME_Result PrivateEncrypt(
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
        INT iMode = MODE_TYPE_RSA_PKCS1_PADDING );

    ME_Result PublicDecrypt(
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrIn,
        CMEMessageBlockAutoPtr& pmbBlockAutoPtrOut,
        INT iMode = MODE_TYPE_RSA_PKCS1_PADDING );

    ME_Result SetOption(
        DWORD dwOptionType,
        void* pOptionValue );

    ME_Result GetOption(
        DWORD dwOptionType,
        void*& pOptionValue );

private:
    RSA*                            m_prsaPublicKey;

    RSA*                            m_prsaPrivateKey;
};

ME_NAME_SPACE_CRYPTO_END


#endif // __CIPHER_RSA_H_

