#include "CustomizeEventRun.h"
#include "Thread.h"

ME_NAME_SPACE_BEGIN

CEventRun::CEventRun()
{

}

CEventRun::~CEventRun()
{

}

ME_Result CEventRun::Run( 
	IMEThread* pThread, 
	BOOL bSync /* = FALSE */ )
{
	ME_ASSERTE_RETURN( pThread, ME_ERROR_NULL_POINTER );

	ME_Result hResult = ME_ERROR_FAILURE;
	if ( FALSE == bSync )
	{
		hResult = pThread->PostEvent( this );
	}
	else
	{
		hResult = pThread->SendEvent( this );
	}

	return hResult;
}





ME_NAME_SPACE_END










