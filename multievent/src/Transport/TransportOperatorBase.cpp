#include "TransportOperatorBase.h"

ME_NAME_SPACE_BEGIN

CMETransportOperatorBase::CMETransportOperatorBase()
{

}

CMETransportOperatorBase::~CMETransportOperatorBase()
{

}

ME_Result CMETransportOperatorBase::PreSendData( CMEMessageBlock*& pmbBlock )
{
	return RETURN_VALUE_OK;
}

ME_Result CMETransportOperatorBase::PostSendData( CMEMessageBlock*& pmbBlock )
{
	return RETURN_VALUE_OK;
}

ME_Result CMETransportOperatorBase::PreOnReceive( CMEMessageBlock*& pmbBlock )
{
	return RETURN_VALUE_OK;
}

ME_Result CMETransportOperatorBase::PostOnReceive( CMEMessageBlock*& pmbBlock )
{
	return RETURN_VALUE_OK;
}

ME_Result CMETransportOperatorBase::PreOnCanSendAgain()
{
	return RETURN_VALUE_OK;
}

ME_Result CMETransportOperatorBase::SetOption( DWORD dwOptionType, VOID* pOptionValue )
{
	return RETURN_VALUE_OK;
}

DWORD CMETransportOperatorBase::AddReference()
{
	return CMEReferenceControlT<MutexType>::AddReference();
}

DWORD CMETransportOperatorBase::RemoveReference()
{
	return CMEReferenceControlT<MutexType>::RemoveReference();
}

ME_NAME_SPACE_END








