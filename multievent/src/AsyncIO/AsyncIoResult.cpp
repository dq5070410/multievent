#include "AsyncIoResult.h"

#include <iostream>

ME_NAME_SPACE_BEGIN

CMEAsyncIoResult::CMEAsyncIoResult( 
	int iIoType, 
	IMEAsyncIoSink* pSink, 
	ME_HANDLE hHandle /* = NULL */ )
	: m_pAioSink( pSink )
{
	/* 初始化overlapped */
	this->Internal = 0;
	this->InternalHigh = 0;

#ifdef ME_WIN
	this->Offset = 0;
	this->OffsetHigh = 0;

#endif	// ME_WIN

	this->hEvent = NULL;

	/* 初始化ME_ASYCH_IO_INFO */
	this->iIoType = iIoType;
	this->hHandle = hHandle;
}

CMEAsyncIoResult::~CMEAsyncIoResult()
{

}

void CMEAsyncIoResult::OnComplete( 
	BOOL bResult,
	int iActiveEvent,
	ME_HANDLE hInterface )
{

	this->OnCompleteImpl(
		bResult,
		iActiveEvent,
		hInterface );

	this->OnDestroy();
}

void CMEAsyncIoResult::OnDestroy()
{
	delete this;
}

ME_NAME_SPACE_END






