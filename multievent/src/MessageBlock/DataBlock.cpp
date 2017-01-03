/**
* 工厂类 公用数据块子模块
* 
*
* History
*	1/15/2016		Created
*/

#include "DataBlock.h"

ME_NAME_SPACE_BEGIN

CMEDataBlock::CMEDataBlock( UINT iBufferLength )
    : m_pBuffer( new BYTE[iBufferLength] )
    , m_iCapacity( iBufferLength )
    , m_iReadPos( 0 )
    , m_iWritePos( 0 )
    , m_bDelete( TRUE )
{
    memset( m_pBuffer, 0, iBufferLength );
}

CMEDataBlock::CMEDataBlock( 
    UINT iBufferLength, 
    LPVOID pBufferAddr, 
    UINT iDataLength,
    BOOL bDelete )
    : m_pBuffer( pBufferAddr )
    , m_iCapacity( iBufferLength )
    , m_iReadPos( 0 )
    , m_iWritePos( iBufferLength >= iDataLength ? iDataLength : iBufferLength )
    , m_bDelete( bDelete )
{

}

CMEDataBlock::~CMEDataBlock()
{
    if ( TRUE == m_bDelete )
    {
        LPBYTE pBuffer = (LPBYTE)m_pBuffer;
        ME_ARRAY_DELETE( pBuffer );
    }
}

ME_Result CMEDataBlock::Write(
    LPCVOID pBuffer,
    UINT iLength )
{
    /* 向缓冲区拷贝数据，移动写指针  */
#ifdef ME_WIN
    ::memcpy_s(
        (LPBYTE)m_pBuffer + m_iWritePos, 
        GetFreeLength(), 
        pBuffer, 
        iLength );
#else
    ::memcpy( 
        (LPBYTE)m_pBuffer + m_iWritePos, 
        pBuffer, 
        iLength );
#endif

    m_iWritePos += iLength;

    return ME_OK;
}

ME_Result CMEDataBlock::Read(
    LPVOID pBuffer, 
    UINT iLength )
{
    /* 拷贝数据、 移动读位置*/
#ifdef ME_WIN
    ::memcpy_s(
        pBuffer, 
        iLength, 
        (LPBYTE)m_pBuffer + m_iReadPos, 
        iLength );
#else
    ::memcpy(
        pBuffer, 
        (LPBYTE)m_pBuffer + m_iReadPos, 
        iLength );
#endif

    m_iReadPos += iLength;

    return ME_OK;
}

UINT CMEDataBlock::GetWritePos() const
{
    return m_iWritePos;
}

UINT CMEDataBlock::GetReadPos() const
{
    return m_iReadPos;
}

ME_Result CMEDataBlock::SetWritePos( UINT iWritePos )
{
    m_iWritePos = iWritePos;

    return ME_OK;
}

ME_Result CMEDataBlock::SetReadPos( UINT iReadPos )
{
    m_iReadPos = iReadPos;

    return ME_OK;
}

UINT CMEDataBlock::GetLength() const
{
    return m_iWritePos - m_iReadPos;
}

LPVOID CMEDataBlock::GetBuffer() const
{
    return m_pBuffer;
}

UINT CMEDataBlock::GetCapacity() const
{
    return m_iCapacity;
}

VOID CMEDataBlock::SetCapacity( UINT iCapacity )
{
    m_iCapacity = iCapacity;
}

UINT CMEDataBlock::GetFreeLength() const
{
    return m_iCapacity - m_iWritePos;
}

BOOL CMEDataBlock::IsDelete() const
{
    return m_bDelete;
}

ME_NAME_SPACE_END
