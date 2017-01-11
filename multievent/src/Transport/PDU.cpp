/**
* PDU定义，目前只有两种pdu：Package，KeepAlive
*
* History
*	04/26/2016		Created
*/

#include "PDU.h"

#include "MessageBlock.h"
#include "ByteStreamT.h"
#include "PDUDefines.h"

ME_NAME_SPACE_BEGIN

CMEPDUPackage::CMEPDUPackage()
	: m_nPackageLength( 0 )
	, m_pUserMB( NULL )
{

}

CMEPDUPackage::CMEPDUPackage(
	UINT nLength,
	CMEMessageBlock* mbBlock )
	: m_nPackageLength( nLength )
	, m_pUserMB( mbBlock )
{

}

ME_Result CMEPDUPackage::Encode( CMEMessageBlock& mbBlock )
{
	/* 写入长度字段 */
	CMEByteStreamT<CMEMessageBlock> bsStream( &mbBlock );
	bsStream << m_nPackageLength;

    ///< 必须使用头部来Append用户MB，否则将会导致用户的MB一直累加头部的4个字节
// 	m_pUserMB->Append( mbBlock, FALSE );

    mbBlock.Append( *m_pUserMB );

	return ME_OK;
}

ME_Result CMEPDUPackage::Decode( CMEMessageBlock& mbBlock )
{
	if ( mbBlock.GetLength() < GetLength() )
	{
		m_nPackageLength = 0;

		return ME_ERROR_NETWORK_PARTIAL_DATA;
	}

	CMEByteStreamT<CMEMessageBlock> bs( &mbBlock );
	bs >> m_nPackageLength;

	if ( mbBlock.GetLength() < m_nPackageLength )
	{
		mbBlock.MoveReadPos( 
			CMEMessageBlock::DIRECTION_TYPE_BACKWORD, 
			GetLength() );
		
		return ME_ERROR_NETWORK_PARTIAL_DATA;
	}

	/* 有一个新的用户MB */
	mbBlock.SetWritePos( mbBlock.GetReadPos() + m_nPackageLength );

	m_pUserMB = &mbBlock;

	return ME_OK;
}

UINT CMEPDUPackage::GetLength() const 
{
	return sizeof(m_nPackageLength);
}

CMEMessageBlock* CMEPDUPackage::GetData() const
{
	return m_pUserMB;
}

CMEPDUBase::CMEPDUBase()
	: m_byType( PDUTYPE_BASE )
{

}

CMEPDUBase::CMEPDUBase( BYTE byType )
	: m_byType( byType )
{

}

BYTE CMEPDUBase::GetType()
{
	return m_byType;
}

UINT CMEPDUBase::GetLength()
{
	return sizeof( m_byType );
}

ME_Result CMEPDUBase::Encode( CMEMessageBlock& mbBlock )
{
	CMEByteStreamT<CMEMessageBlock> bsStream( &mbBlock );

	bsStream << m_byType;

	return ME_OK;
}

ME_Result CMEPDUBase::Decode( CMEMessageBlock& mbBlock )
{
	CMEByteStreamT<CMEMessageBlock> bsStream( &mbBlock );

	bsStream >> m_byType;

	return ME_OK;
}

CMEPDUKeepAliveReq::CMEPDUKeepAliveReq()
	: CMEPDUBase( PDUTYPE_KEEPALIVE_REQ )
{

}

CMEPDUKeepAliveRsp::CMEPDUKeepAliveRsp()
	: CMEPDUBase( PDUTYPE_KEEPALIVE_RSP )
{

}

ME_NAME_SPACE_END
