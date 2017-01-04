/** @file       ConnectionOperator.cpp
 *
 *  @note       History
 *  @par        04/29/2016  Created
 */


#include "ConnectionOperator.h"


ME_NAME_SPACE_BEGIN

CMEConnectionOperator::CMEConnectionOperator()
{

}

CMEConnectionOperator::~CMEConnectionOperator()
{

}

ME_Result CMEConnectionOperator::PreOnConnect(
    ME_HANDLE hSocket,
    CMETransportAutoPtr& pTransportAutoPtr )
{
    ///< 默认不做任何处理就返回成功
    return ME_OK;
}

ME_Result CMEConnectionOperator::SetOption(
    DWORD dwOptionType,
    void* pOptoinValue )
{
    return ME_ERROR_NOT_AVAILABLE;
}

ME_Result CMEConnectionOperator::GetOption(
    DWORD dwOptionType,
    void*& pOptionType )
{
    return ME_ERROR_NOT_AVAILABLE;
}

DWORD CMEConnectionOperator::AddReference()
{
    return CMEReferenceControlT<CMELockMutex>::AddReference();
}

DWORD CMEConnectionOperator::RemoveReference()
{
    return CMEReferenceControlT<CMELockMutex>::RemoveReference();
}

ME_NAME_SPACE_END

