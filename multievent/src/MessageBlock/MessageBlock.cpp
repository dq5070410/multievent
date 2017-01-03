/**
* MessageBlock工厂类 公用数据块
* 
*
* History
*	1/15/2016		Created
*/

#include "MessageBlock.h"
#include "DataBlock.h"

ME_NAME_SPACE_BEGIN

CMEMessageBlock::CMEMessageBlock(
    UINT iBufferLength, 
    MESSAGEBLOCK_ATTR iAttr /* = MESSAGEBLOCK_ATTR_NONE */ )
    : m_vectorDBs()
    , m_iWriteDB( 0 )
    , m_iReadDB( 0 )
    , m_iCapacity( iBufferLength )
    , m_iWritePos( 0 )
    , m_iReadPos( 0 )
    , m_iAttr( iAttr )
{
    ME_INFO_TRACE_THIS( " BufferLength=" << iBufferLength << " Attr=" << iAttr );

    if ( iBufferLength > 0 )
    {
        m_vectorDBs.push_back( new CMEDataBlock(iBufferLength) );
    }
}

CMEMessageBlock::CMEMessageBlock( 
    UINT iBufferLength, 
    LPVOID pBufferAddr, 
    UINT iDataLength, 
    MESSAGEBLOCK_ATTR iAttr /* = MESSAGEBLOCK_ATTR_NONE */ )
    : m_vectorDBs()
    , m_iWriteDB( 0 )
    , m_iReadDB( 0 )
    , m_iCapacity( iBufferLength )
    , m_iWritePos( iBufferLength >= iDataLength ? iDataLength : iBufferLength )
    , m_iReadPos( 0 )
    , m_iAttr( iAttr )
{
    ME_INFO_TRACE_THIS( " BufferLength=" << iBufferLength << " BufferAddr=" << pBufferAddr << " DataLength=" << iDataLength << " Attr" << iAttr );

    BOOL bDBDelete = ME_BIT_ENABLED( iAttr, MESSAGEBLOCK_ATTR_DELETE_USERDATA ) ? TRUE : FALSE;

    m_vectorDBs.push_back( new CMEDataBlock(iBufferLength, pBufferAddr, iDataLength, bDBDelete) );

	SetWritePos( m_iWritePos );
}

CMEMessageBlock::CMEMessageBlock( const CMEMessageBlock& hMB )
{
    ME_INFO_TRACE_THIS( "" );

    /* 对内置型的数据需手动初始化，防止在"="时误判为只读 */
    m_iAttr = MESSAGEBLOCK_ATTR_NONE;

    /* 调用 operator = */
    *this = hMB;
}

CMEMessageBlock::~CMEMessageBlock()
{
    ME_INFO_TRACE_THIS( "" );

    /**
    * 将存储DB指针的容器清空，容器中存放的是DB的智能指针，调用智能指针的
    * 析构函数，DB的引用计数减一，如果引用计数为0，DB会自动释放
    */
    m_vectorDBs.clear();
}

CMEMessageBlock& CMEMessageBlock::operator = ( const CMEMessageBlock& hMB )
{
    ME_INFO_TRACE_THIS( "" );

    if ( this != &hMB )
    {
        /* 只读 */
        ME_ASSERTE_RETURN( ME_BIT_DISABLED(m_iAttr, MESSAGEBLOCK_ATTR_READONLY), *this );

        /* 浅拷贝（仅拷贝所有DB指针）*/
        m_vectorDBs = hMB.m_vectorDBs;

        /* 复制属性 */
        m_iWriteDB = hMB.m_iWriteDB;
        m_iReadDB = hMB.m_iReadDB;

        m_iCapacity = hMB.m_iCapacity;
        m_iWritePos = hMB.m_iWritePos;
        m_iReadPos = hMB.m_iReadPos;

        m_iAttr = hMB.m_iAttr;
    }

    return *this;
}

ME_Result CMEMessageBlock::Write( 
    LPCVOID pBuffer, 
    UINT iLength )
{
    ME_INFO_TRACE_THIS( " Buffer=" << pBuffer << " Length=" << iLength );

    /* 是否只读、用户数据地址是否为空指针 */
    ME_ASSERTE_RETURN( ME_BIT_DISABLED(m_iAttr, MESSAGEBLOCK_ATTR_READONLY), ME_ERROR_NOT_AVAILABLE );
    ME_ASSERTE_RETURN( pBuffer, ME_ERROR_NULL_POINTER );

    /* 是否有足够空间写入 */
    if ( GetFreeLength() < iLength )
    {
        return ME_ERROR_OUT_OF_MEMORY;
    }

    UINT iBytesToWrite = iLength;   // 要写数据数目
    UINT iDBFreeSpace = 0;          // 当前写DB块剩余空间大小
    UINT iDBWriteNum = 0;           // 应该向当前写DB写入的数据个数

    while( iBytesToWrite > 0 && m_vectorDBs.size() > m_iWriteDB )     // 还有要写的数据
    {
        /* 当前写DB空闲空间 */
        iDBFreeSpace = m_vectorDBs[m_iWriteDB]->GetFreeLength();

        /* 应该向当前写DB写入的个数 */
        iDBWriteNum = iDBFreeSpace > iBytesToWrite ? iBytesToWrite : iDBFreeSpace;

        /* 写入数据 */
        m_vectorDBs[m_iWriteDB]->Write( 
            (LPCBYTE)pBuffer + iLength - iBytesToWrite, 
            iDBWriteNum );

        /* 还要写多少 */
        iBytesToWrite -= iDBWriteNum;

        if ( iBytesToWrite > 0 ) 
        {
            /* 没写完，接着向下一个DB写 */
            ++m_iWriteDB;
        }
    }

	if ( 
		m_iWriteDB < m_vectorDBs.size() &&
		m_vectorDBs[m_iWriteDB]->GetWritePos() == m_vectorDBs[m_iWriteDB]->GetCapacity() )
	{
		++m_iWriteDB;
	}

    /* 累加缓冲区写位置 */
    m_iWritePos += iLength;

    return ME_OK;
}

ME_Result CMEMessageBlock::Read( 
    LPVOID pBuffer, 
    UINT iLength )
{
    ME_INFO_TRACE_THIS( "Buffer=" << pBuffer << " Length=" << iLength );

    /* 用户数据地址是否为空指针 */
    ME_ASSERTE_RETURN( pBuffer, ME_ERROR_NULL_POINTER );

    /* 是否有足够多的数据可供读出 */
    if ( GetLength() < iLength )
    {
        return ME_ERROR_REACH_MAX;
    }

    UINT iBytesToRead = iLength;        // 要读数据个数
    UINT iDBDataLength = 0;             // 当前读DB最多可被读出的数据
    UINT iDBReadNum = 0;                // 应该从当前读DB读取的数据个数

    while( iBytesToRead > 0 && m_vectorDBs.size() > m_iReadDB )
    {
        /* 当前读DB最多可被读出的数据 */
        iDBDataLength = m_vectorDBs[m_iReadDB]->GetLength();

        /* 应该从当前读DB读取的字节个数 */
        iDBReadNum = iDBDataLength > iBytesToRead ? iBytesToRead : iDBDataLength;

        /* 读出数据 */
        m_vectorDBs[m_iReadDB]->Read( 
            (LPBYTE)pBuffer + iLength - iBytesToRead, 
            iDBReadNum );

        /* 还要读多少 */
        iBytesToRead -= iDBReadNum;

        if ( iBytesToRead > 0 ) 
        {
            /* 没读完，接着从下一个DB读 */
            ++m_iReadDB;
        }
    }

	if ( 
		m_iReadDB < m_vectorDBs.size() && 
		m_vectorDBs[m_iReadDB]->GetReadPos() == m_vectorDBs[m_iReadDB]->GetCapacity() )
	{
		++m_iReadDB;
	}

    /* 累加读位置 */
    m_iReadPos += iLength;

    return ME_OK;
}

UINT CMEMessageBlock::GetWritePos() const
{
    ME_INFO_TRACE_THIS( "" );

    return m_iWritePos;
}

ME_Result CMEMessageBlock::SetWritePos( UINT iWritePos )
{
    ME_INFO_TRACE_THIS( " WritePos=" << iWritePos );

    /* 是否只读*/
    ME_ASSERTE_RETURN( ME_BIT_DISABLED(m_iAttr, MESSAGEBLOCK_ATTR_READONLY), ME_ERROR_NOT_AVAILABLE );

    /* 是否就是当前写位置 */
    //if ( m_iWritePos == iWritePos )
    //{
        //return ME_OK;
    //}

    /* 是否超出了范围 */
    if ( m_iCapacity < iWritePos )
    {
        return ME_ERROR_REACH_MAX;
    }

    /* DB容量和 */
    UINT iDBsCapacity = 0;

    UINT iDBIndex = 0;
    for ( ; iDBIndex != m_vectorDBs.size(); ++iDBIndex )
    {
        /* 从头累加DB块容量 */
        iDBsCapacity += m_vectorDBs[iDBIndex]->GetCapacity();

        if ( iDBsCapacity >= iWritePos )
        {
            /* DB块 “容量和” 首次超过了要设置的写位置
               说明该DB块就是要设置的写块 */

            /* 设置块内偏移量 */
            m_vectorDBs[iDBIndex]->SetWritePos(
                m_vectorDBs[iDBIndex]->GetCapacity() - (iDBsCapacity - iWritePos) );

            /* 设置当前写块 */
            m_iWriteDB = iDBIndex;

			if ( m_vectorDBs[m_iWriteDB]->GetWritePos() == m_vectorDBs[m_iWriteDB]->GetCapacity() )
			{
				++m_iWriteDB;
			}

            break;
        }
        else
        {
            m_vectorDBs[iDBIndex]->SetWritePos( m_vectorDBs[iDBIndex]->GetCapacity() );
        }
    }

    iDBIndex += 1;
    for ( ; iDBIndex < m_vectorDBs.size(); ++iDBIndex )
    {
        m_vectorDBs[iDBIndex]->SetWritePos( 0 );
    }

    /* 设置缓冲区写位置 */
    m_iWritePos = iWritePos;

    return ME_OK;
}

ME_Result CMEMessageBlock::MoveWritePos( 
    DIRECTION_TYPE iDirection, 
    UINT iStep )
{
    ME_INFO_TRACE_THIS( " Direction=" << iDirection << " Step=" << iStep );

    /* 是否只读 */
    ME_ASSERTE_RETURN( ME_BIT_DISABLED(m_iAttr, MESSAGEBLOCK_ATTR_READONLY), ME_ERROR_NOT_AVAILABLE );

    if ( DIRECTION_TYPE_FORWORD == iDirection )
    {
        /* 向前 */
        return SetWritePos( GetWritePos() + iStep );
    }
    else
    {
        /* 向后 */
        return SetWritePos( GetWritePos() - iStep );
    }
}

UINT CMEMessageBlock::GetReadPos() const
{
    ME_INFO_TRACE_THIS( "" );

    return m_iReadPos;
}

ME_Result CMEMessageBlock::SetReadPos( UINT iReadPos )
{
    ME_INFO_TRACE_THIS( " ReadPos=" << iReadPos );

    /* 是否就是当前读位置、是否超出范围*/
    //if ( m_iReadPos == iReadPos )
    //{
        //return ME_OK;
    //}

    if ( m_iCapacity < iReadPos )
    {
        return ME_ERROR_REACH_MAX;
    }

    UINT iDBsCapacity = 0;      // DB容量和

    UINT iDBIndex = 0;
    for( ; iDBIndex < m_vectorDBs.size(); ++iDBIndex )
    {
        /* 从头累加DB容量 */
        iDBsCapacity += m_vectorDBs[iDBIndex]->GetCapacity();

        if ( iDBsCapacity >= iReadPos )
        {
            /**
            * DB块 “容量和” 首次超过了要设置的读位置
            * 说明该DB块就是要设置的读块
            */

            /* 设置块内偏移量 */
            m_vectorDBs[iDBIndex]->SetReadPos( 
                m_vectorDBs[iDBIndex]->GetCapacity() - (iDBsCapacity - iReadPos) );

            /* 设置当前读块 */
            m_iReadDB = iDBIndex;
			
			if ( m_vectorDBs[m_iReadDB]->GetReadPos() == m_vectorDBs[m_iReadDB]->GetCapacity() )
			{
				++m_iReadDB;
			}

            break;
        }
        else
        {
            m_vectorDBs[iDBIndex]->SetReadPos( m_vectorDBs[iDBIndex]->GetCapacity() );
        }
    }
    
    iDBIndex += 1;
    for ( ; iDBIndex < m_vectorDBs.size(); ++iDBIndex )
    {
        m_vectorDBs[iDBIndex]->SetReadPos( 0 );
    }


    /* 设置缓冲区当前读位置 */
    m_iReadPos = iReadPos;

    return ME_OK;
}

ME_Result CMEMessageBlock::MoveReadPos( 
    DIRECTION_TYPE iDirection, 
    UINT iStep )
{
    ME_INFO_TRACE_THIS( " Direction=" << iDirection << " Step=" << iStep );

    if ( DIRECTION_TYPE_FORWORD == iDirection )
    {
        /* 向前 */
        return SetReadPos( GetReadPos() + iStep );
    }
    else
    {
        /* 向后 */
        return SetReadPos( GetReadPos() - iStep );
    }
}

UINT CMEMessageBlock::GetCapacity() const
{
    ME_INFO_TRACE_THIS( "" );

    return m_iCapacity;
}

UINT CMEMessageBlock::GetLength() const
{
    ME_INFO_TRACE_THIS( "" );

    ME_ASSERTE_RETURN( (m_iWritePos >= m_iReadPos), 0 );

    /* 已写但还未被读出的数据 */
    return m_iWritePos - m_iReadPos;
}

UINT CMEMessageBlock::GetFreeLength() const
{
    ME_INFO_TRACE_THIS( "" );

    ME_ASSERTE_RETURN( (m_iCapacity >= m_iWritePos), 0 );

    /* 剩余空闲空间 */
    return m_iCapacity - m_iWritePos;
}

VOID CMEMessageBlock::Append( 
    CMEMessageBlock& mbOutPut,
    const CMEMessageBlock& mbHead, 
    const CMEMessageBlock& mbTail )
{
    /**
    * 可能有的异常情况：
    * 1，调用此方法时，该MB的当前写DB后还有DB块，将这些后继块删除
    * 2，调用此方法时，该MB的当前写DB还没有被写完（当前写DB的写位置和容量字段不相等），
    *    将没有写的空间忽略掉（设置DB容量为DB写位置）
    */

    mbOutPut = mbHead;

    /* 处理上述第一种异常情况 */
    UINT iDBIndex = mbOutPut.m_iWriteDB + 1;  // 当前写块后一块
    for ( ; iDBIndex < mbOutPut.m_vectorDBs.size(); ++iDBIndex )
    {
        /* 减去容量 */
        mbOutPut.m_iCapacity -= mbOutPut.m_vectorDBs[iDBIndex]->GetCapacity();

        /* 删除后继DB */
        mbOutPut.m_vectorDBs.erase( mbOutPut.m_vectorDBs.begin() + iDBIndex );
    }

    /* 处理上述第二种异常情况 */
    UINT iWriteDBCapacity = mbOutPut.m_vectorDBs.size() >  mbOutPut.m_iWriteDB ? mbOutPut.m_vectorDBs[mbHead.m_iWriteDB]->GetCapacity() : 0;
    UINT iWriteDBWritePos = mbOutPut.m_vectorDBs.size() >  mbOutPut.m_iWriteDB ? mbOutPut.m_vectorDBs[mbHead.m_iWriteDB]->GetWritePos() : 0;
    if ( iWriteDBCapacity != iWriteDBWritePos )
    {
        ME_ASSERTE_RETURN_VOID( (iWriteDBCapacity > iWriteDBWritePos) );

        /* 忽略当前DB没有写完的空间 */
        mbOutPut.m_iCapacity -= ( iWriteDBCapacity - iWriteDBWritePos );
        mbOutPut.m_vectorDBs[mbOutPut.m_iWriteDB]->SetCapacity( iWriteDBWritePos );

        /* 如果当前块被擦除完毕，则删除该块 */
        if ( 0 == mbOutPut.m_vectorDBs[mbOutPut.m_iWriteDB]->GetCapacity() )
        {
            mbOutPut.m_vectorDBs.erase( mbOutPut.m_vectorDBs.begin() + mbOutPut.m_iWriteDB );
            if ( mbOutPut.m_iWriteDB > 0 )
            {
                --mbOutPut.m_iWriteDB;
            }
        }
    }

    mbOutPut.m_iCapacity += mbTail.GetCapacity(); 

    //mbOutPut.m_iWriteDB += 
    //    ( mbOutPut.m_vectorDBs.size() == 0 || mbTail.m_vectorDBs.size() == 0 ) ? mbTail.m_iWriteDB : ( mbTail.m_iWriteDB + 1 );

    //mbOutPut.m_iWritePos += mbTail.GetWritePos();

    /* 复制hMB的DB块指针到当前MB末尾 */
    iDBIndex = 0;
    for ( ; iDBIndex < mbTail.m_vectorDBs.size(); ++iDBIndex )
    {
        mbOutPut.m_vectorDBs.push_back( mbTail.m_vectorDBs[iDBIndex] );
    }

    mbOutPut.SetReadPos( 0 );

    mbOutPut.SetWritePos( mbOutPut.m_iWritePos + mbTail.GetWritePos() );
}

CMEMessageBlock& CMEMessageBlock::Append( 
    const CMEMessageBlock& hMB, 
    BOOL bTail /* = TRUE */ )
{
    ME_INFO_TRACE_THIS( "" );

    /* 是否是本身 */
    ME_ASSERTE_RETURN( (this != &hMB), *this );

    /* 是否只读 */
    ME_ASSERTE_RETURN( ME_BIT_DISABLED(m_iAttr, MESSAGEBLOCK_ATTR_READONLY), *this );

    CMEMessageBlock mbOutPut(0);

    if ( TRUE == bTail )
    {
        Append( mbOutPut, *this, hMB );
    }
    else
    {
        Append( mbOutPut, hMB, *this );
    }

    *this = mbOutPut;

    return *this;
}

CMEMessageBlock* CMEMessageBlock::Clone()
{
    ME_INFO_TRACE_THIS( "" );
    
    /* 深拷贝 */
    CMEMessageBlock* pmb = new CMEMessageBlock( m_iCapacity );

    //ME_ASSERTE_RETURN( (pmb && pmb->m_vectorDBs.size() > 0), NULL );

    /* 拷贝hMB缓冲区内容到pDB中 */
    UINT iOffSet = 0;
    UINT iDBIndex = 0;
    for ( ; iDBIndex != m_vectorDBs.size() && pmb->m_vectorDBs.size() > 0; 
		++iDBIndex )
    {
#ifdef ME_WIN
        ::memcpy_s( 
            (LPBYTE)pmb->m_vectorDBs[0]->GetBuffer() + iOffSet,
            pmb->m_vectorDBs[0]->GetCapacity() - iOffSet,
            m_vectorDBs[iDBIndex]->GetBuffer(),
            m_vectorDBs[iDBIndex]->GetWritePos() );
#else
        ::memcpy( 
            (LPBYTE)pmb->m_vectorDBs[0]->GetBuffer() + iOffSet,
            m_vectorDBs[iDBIndex]->GetBuffer(),
            m_vectorDBs[iDBIndex]->GetWritePos() );
#endif
        iOffSet += m_vectorDBs[iDBIndex]->GetWritePos();
    }

    /* 设置新MB的属性 */
    pmb->SetWritePos( m_iWritePos );
    pmb->SetReadPos( m_iReadPos );

    pmb->m_iCapacity = m_iCapacity;
    pmb->m_iAttr = m_iAttr;

    return pmb;
}

VOID CMEMessageBlock::Reset()
{
    ME_INFO_TRACE_THIS( "" );

    SetWritePos( 0 );
    SetReadPos( 0 );
}

VOID CMEMessageBlock::Clear()
{
	m_vectorDBs.clear();

	m_iWriteDB = 0;
	m_iReadDB = 0;

	m_iCapacity = 0;
	m_iWritePos = 0;
	m_iReadPos = 0;

	//m_iAttr = MESSAGEBLOCK_ATTR_NONE;
}

UINT CMEMessageBlock::FillIOVec( iovec* pIOVec, UINT iIOVecNum )
{
    ME_INFO_TRACE_THIS( " IOVecAddr=" << pIOVec << " IOVecNum" );

    /* 用户缓冲区是否为空指针 */
    ME_ASSERTE_RETURN( pIOVec, 0 );

	ME_ASSERTE_RETURN( m_iWritePos >= m_iReadPos, 0 );

    /* DB数量是否小于数组长度 */
    //if ( m_vectorDBs.size() > iIOVecNum )
    //{
        //return 0;
    //}

	UINT iCopyNum = Resize( iIOVecNum );
	if ( 0 == iCopyNum )
	{
		ME_ERROR_TRACE( "Resize Fail" );
		return 0;
	}

    /* 遍历DB容器，拷贝信息到对应数组元素 */
    UINT iDBIndex = m_iReadDB;
    for ( ; iDBIndex < m_iReadDB + iCopyNum; ++iDBIndex )
    {
		if ( iDBIndex >= m_vectorDBs.size() )
		{
			continue;
		}

        /* 缓冲区地址 */
        pIOVec->iov_base = (LPSTR)m_vectorDBs[iDBIndex]->GetBuffer() + m_vectorDBs[iDBIndex]->GetReadPos();

        /* 缓冲区长度 */
        pIOVec->iov_len = m_vectorDBs[iDBIndex]->GetWritePos() - m_vectorDBs[iDBIndex]->GetReadPos();

        ++pIOVec;
    }

	return iCopyNum;
}

UINT CMEMessageBlock::Resize( UINT iMaxLengthDBSize )
{
	/* 根据当前读写位置， 计算出需要的DB数量 */
	UINT iCurrentLengthDBSize = m_iWriteDB - m_iReadDB;

	if ( m_vectorDBs.size() > m_iWriteDB &&
		0 != m_vectorDBs[m_iWriteDB]->GetWritePos() )
	{
		++iCurrentLengthDBSize; 
	}

	/* 如果没有足够空间容纳DB， 则将最后面的DB合并为一个 */
	if ( iCurrentLengthDBSize > iMaxLengthDBSize )
	{
		UINT iCopySize = 0;
		UINT iCopyDBStart = m_iReadDB + iMaxLengthDBSize -1;

		/* 计算出需要合并DB的总长度 */
		UINT iCopyDBIndex = iCopyDBStart;
		for( ; iCopyDBIndex < m_iReadDB + iCurrentLengthDBSize; ++iCopyDBIndex )
		{
			iCopySize += m_vectorDBs[iCopyDBIndex]->GetCapacity();
		}

		/* new 一个DB */
		CMEAutoPtrT<CMEDataBlock> pDB;
		try
		{
			pDB = new CMEDataBlock(iCopySize);
		}
		catch( std::bad_alloc )
		{
			return 0;
		}

		/* 向新生成的DB中写入数据 */
		iCopyDBIndex = iCopyDBStart;
		for( ; iCopyDBIndex < m_iReadDB + iCurrentLengthDBSize; ++iCopyDBIndex )
		{
			pDB->Write( 
				m_vectorDBs[iCopyDBIndex]->GetBuffer(), 
				m_vectorDBs[iCopyDBIndex]->GetWritePos() );
		}

		/* 删除已经整理好的DB */
		m_vectorDBs.erase( 
			m_vectorDBs.begin() + iCopyDBStart,
			m_vectorDBs.begin() + m_iReadDB + iCurrentLengthDBSize );

		/* 插入新生成的DB */
		m_vectorDBs.insert( 
			m_vectorDBs.begin() + iCopyDBStart, 
			pDB );

        SetWritePos( m_iWritePos );

        SetReadPos( m_iReadPos );
	}

	return iCurrentLengthDBSize < iMaxLengthDBSize ? iCurrentLengthDBSize : iMaxLengthDBSize;
}

ME_NAME_SPACE_END
