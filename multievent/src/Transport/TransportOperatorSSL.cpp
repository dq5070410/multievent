/** @file       TransportOperatorSSL.cpp
 *
 *  @note       History
 *  @par        04/29/2016  Created
 */


#include "TransportOperatorSSL.h"
#include "ConnectionInterface.h"


using ME::CRYPTO::CMESSL;


ME_NAME_SPACE_BEGIN

CMETransportOperatorSSL::CMETransportOperatorSSL()
    : m_bNeedNewPackage( TRUE )
{

}

CMETransportOperatorSSL::~CMETransportOperatorSSL()
{

}

ME_Result CMETransportOperatorSSL::PreSendData( CMEMessageBlock*& pmbBlock )
{
    CMEMessageBlockAutoPtr pmbBlockAutoPtrIn = pmbBlock;
    CMEMessageBlockAutoPtr pmbBlockAutoPtrOut = NULL;

    ME_Result hResult = m_pSSLAutoPtr->Encrypt(
        pmbBlockAutoPtrIn,
        pmbBlockAutoPtrOut );

    ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

    pmbBlock = pmbBlockAutoPtrOut.Get();

    pmbBlock->AddReference();

    return hResult;
}

ME_Result CMETransportOperatorSSL::PostSendData( CMEMessageBlock*& pmbBlock )
{
    pmbBlock->RemoveReference();

    return ME_OK;
}

ME_Result CMETransportOperatorSSL::PreOnCanSendAgain()
{
    ///< 预留，目前Transport还不会调用这个接口

    return ME_OK;
}

ME_Result CMETransportOperatorSSL::PreOnReceive( CMEMessageBlock*& pmbBlock )
{
    if ( m_bNeedNewPackage )
    {
        m_pmbMessgeBlockIn = pmbBlock;
    }

    ME_Result hResult = m_pSSLAutoPtr->Decrypt(
        m_pmbMessgeBlockIn,
        m_pmbMessgeBlockOut );

    if ( ME_SUCCEEDED(hResult) )
    {
        pmbBlock = m_pmbMessgeBlockOut.Get();

        m_bNeedNewPackage = FALSE;

        hResult = RETURN_VALUE_AGAIN;
    }
    else
    {
        m_pmbMessgeBlockIn = NULL;

        m_bNeedNewPackage = TRUE;

        hResult = RETURN_VALUE_FINISH;
    }

    return hResult;
}

ME_Result CMETransportOperatorSSL::PostOnReceive( CMEMessageBlock*& pmbBlock )
{
    m_pmbMessgeBlockOut = NULL;

    return ME_OK;
}

ME_Result CMETransportOperatorSSL::SetOption(
    DWORD dwOptionType,
    VOID* pOptionValue )
{
    ME_Result hResult = ME_ERROR_NOT_AVAILABLE;

    switch ( dwOptionType )
    {
    case ITransport::OPTION_TYPE_SET_SSL:
        {
            CMESSL* pSSL = static_cast<CMESSL*>( pOptionValue );
            if ( NULL != pSSL )
            {
                m_pSSLAutoPtr = pSSL;

                hResult = ME_OK;
            }

    	    break;
        }

    default:
        break;
    }

    return hResult;
}

DWORD CMETransportOperatorSSL::GetType()
{
    return IConnectionManager::CONNECTION_TYPE_SSL;
}

ME_NAME_SPACE_END

