/**
* MessageBlock������ �������ݿ�
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

    /* �������͵��������ֶ���ʼ������ֹ��"="ʱ����Ϊֻ�� */
    m_iAttr = MESSAGEBLOCK_ATTR_NONE;

    /* ���� operator = */
    *this = hMB;
}

CMEMessageBlock::~CMEMessageBlock()
{
    ME_INFO_TRACE_THIS( "" );

    /**
    * ���洢DBָ���������գ������д�ŵ���DB������ָ�룬��������ָ���
    * ����������DB�����ü�����һ��������ü���Ϊ0��DB���Զ��ͷ�
    */
    m_vectorDBs.clear();
}

CMEMessageBlock& CMEMessageBlock::operator = ( const CMEMessageBlock& hMB )
{
    ME_INFO_TRACE_THIS( "" );

    if ( this != &hMB )
    {
        /* ֻ�� */
        ME_ASSERTE_RETURN( ME_BIT_DISABLED(m_iAttr, MESSAGEBLOCK_ATTR_READONLY), *this );

        /* ǳ����������������DBָ�룩*/
        m_vectorDBs = hMB.m_vectorDBs;

        /* �������� */
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

    /* �Ƿ�ֻ�����û����ݵ�ַ�Ƿ�Ϊ��ָ�� */
    ME_ASSERTE_RETURN( ME_BIT_DISABLED(m_iAttr, MESSAGEBLOCK_ATTR_READONLY), ME_ERROR_NOT_AVAILABLE );
    ME_ASSERTE_RETURN( pBuffer, ME_ERROR_NULL_POINTER );

    /* �Ƿ����㹻�ռ�д�� */
    if ( GetFreeLength() < iLength )
    {
        return ME_ERROR_OUT_OF_MEMORY;
    }

    UINT iBytesToWrite = iLength;   // Ҫд������Ŀ
    UINT iDBFreeSpace = 0;          // ��ǰдDB��ʣ��ռ��С
    UINT iDBWriteNum = 0;           // Ӧ����ǰдDBд������ݸ���

    while( iBytesToWrite > 0 && m_vectorDBs.size() > m_iWriteDB )     // ����Ҫд������
    {
        /* ��ǰдDB���пռ� */
        iDBFreeSpace = m_vectorDBs[m_iWriteDB]->GetFreeLength();

        /* Ӧ����ǰдDBд��ĸ��� */
        iDBWriteNum = iDBFreeSpace > iBytesToWrite ? iBytesToWrite : iDBFreeSpace;

        /* д������ */
        m_vectorDBs[m_iWriteDB]->Write( 
            (LPCBYTE)pBuffer + iLength - iBytesToWrite, 
            iDBWriteNum );

        /* ��Ҫд���� */
        iBytesToWrite -= iDBWriteNum;

        if ( iBytesToWrite > 0 ) 
        {
            /* ûд�꣬��������һ��DBд */
            ++m_iWriteDB;
        }
    }

	if ( 
		m_iWriteDB < m_vectorDBs.size() &&
		m_vectorDBs[m_iWriteDB]->GetWritePos() == m_vectorDBs[m_iWriteDB]->GetCapacity() )
	{
		++m_iWriteDB;
	}

    /* �ۼӻ�����дλ�� */
    m_iWritePos += iLength;

    return ME_OK;
}

ME_Result CMEMessageBlock::Read( 
    LPVOID pBuffer, 
    UINT iLength )
{
    ME_INFO_TRACE_THIS( "Buffer=" << pBuffer << " Length=" << iLength );

    /* �û����ݵ�ַ�Ƿ�Ϊ��ָ�� */
    ME_ASSERTE_RETURN( pBuffer, ME_ERROR_NULL_POINTER );

    /* �Ƿ����㹻������ݿɹ����� */
    if ( GetLength() < iLength )
    {
        return ME_ERROR_REACH_MAX;
    }

    UINT iBytesToRead = iLength;        // Ҫ�����ݸ���
    UINT iDBDataLength = 0;             // ��ǰ��DB���ɱ�����������
    UINT iDBReadNum = 0;                // Ӧ�ôӵ�ǰ��DB��ȡ�����ݸ���

    while( iBytesToRead > 0 && m_vectorDBs.size() > m_iReadDB )
    {
        /* ��ǰ��DB���ɱ����������� */
        iDBDataLength = m_vectorDBs[m_iReadDB]->GetLength();

        /* Ӧ�ôӵ�ǰ��DB��ȡ���ֽڸ��� */
        iDBReadNum = iDBDataLength > iBytesToRead ? iBytesToRead : iDBDataLength;

        /* �������� */
        m_vectorDBs[m_iReadDB]->Read( 
            (LPBYTE)pBuffer + iLength - iBytesToRead, 
            iDBReadNum );

        /* ��Ҫ������ */
        iBytesToRead -= iDBReadNum;

        if ( iBytesToRead > 0 ) 
        {
            /* û���꣬���Ŵ���һ��DB�� */
            ++m_iReadDB;
        }
    }

	if ( 
		m_iReadDB < m_vectorDBs.size() && 
		m_vectorDBs[m_iReadDB]->GetReadPos() == m_vectorDBs[m_iReadDB]->GetCapacity() )
	{
		++m_iReadDB;
	}

    /* �ۼӶ�λ�� */
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

    /* �Ƿ�ֻ��*/
    ME_ASSERTE_RETURN( ME_BIT_DISABLED(m_iAttr, MESSAGEBLOCK_ATTR_READONLY), ME_ERROR_NOT_AVAILABLE );

    /* �Ƿ���ǵ�ǰдλ�� */
    //if ( m_iWritePos == iWritePos )
    //{
        //return ME_OK;
    //}

    /* �Ƿ񳬳��˷�Χ */
    if ( m_iCapacity < iWritePos )
    {
        return ME_ERROR_REACH_MAX;
    }

    /* DB������ */
    UINT iDBsCapacity = 0;

    UINT iDBIndex = 0;
    for ( ; iDBIndex != m_vectorDBs.size(); ++iDBIndex )
    {
        /* ��ͷ�ۼ�DB������ */
        iDBsCapacity += m_vectorDBs[iDBIndex]->GetCapacity();

        if ( iDBsCapacity >= iWritePos )
        {
            /* DB�� �������͡� �״γ�����Ҫ���õ�дλ��
               ˵����DB�����Ҫ���õ�д�� */

            /* ���ÿ���ƫ���� */
            m_vectorDBs[iDBIndex]->SetWritePos(
                m_vectorDBs[iDBIndex]->GetCapacity() - (iDBsCapacity - iWritePos) );

            /* ���õ�ǰд�� */
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

    /* ���û�����дλ�� */
    m_iWritePos = iWritePos;

    return ME_OK;
}

ME_Result CMEMessageBlock::MoveWritePos( 
    DIRECTION_TYPE iDirection, 
    UINT iStep )
{
    ME_INFO_TRACE_THIS( " Direction=" << iDirection << " Step=" << iStep );

    /* �Ƿ�ֻ�� */
    ME_ASSERTE_RETURN( ME_BIT_DISABLED(m_iAttr, MESSAGEBLOCK_ATTR_READONLY), ME_ERROR_NOT_AVAILABLE );

    if ( DIRECTION_TYPE_FORWORD == iDirection )
    {
        /* ��ǰ */
        return SetWritePos( GetWritePos() + iStep );
    }
    else
    {
        /* ��� */
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

    /* �Ƿ���ǵ�ǰ��λ�á��Ƿ񳬳���Χ*/
    //if ( m_iReadPos == iReadPos )
    //{
        //return ME_OK;
    //}

    if ( m_iCapacity < iReadPos )
    {
        return ME_ERROR_REACH_MAX;
    }

    UINT iDBsCapacity = 0;      // DB������

    UINT iDBIndex = 0;
    for( ; iDBIndex < m_vectorDBs.size(); ++iDBIndex )
    {
        /* ��ͷ�ۼ�DB���� */
        iDBsCapacity += m_vectorDBs[iDBIndex]->GetCapacity();

        if ( iDBsCapacity >= iReadPos )
        {
            /**
            * DB�� �������͡� �״γ�����Ҫ���õĶ�λ��
            * ˵����DB�����Ҫ���õĶ���
            */

            /* ���ÿ���ƫ���� */
            m_vectorDBs[iDBIndex]->SetReadPos( 
                m_vectorDBs[iDBIndex]->GetCapacity() - (iDBsCapacity - iReadPos) );

            /* ���õ�ǰ���� */
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


    /* ���û�������ǰ��λ�� */
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
        /* ��ǰ */
        return SetReadPos( GetReadPos() + iStep );
    }
    else
    {
        /* ��� */
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

    /* ��д����δ������������ */
    return m_iWritePos - m_iReadPos;
}

UINT CMEMessageBlock::GetFreeLength() const
{
    ME_INFO_TRACE_THIS( "" );

    ME_ASSERTE_RETURN( (m_iCapacity >= m_iWritePos), 0 );

    /* ʣ����пռ� */
    return m_iCapacity - m_iWritePos;
}

VOID CMEMessageBlock::Append( 
    CMEMessageBlock& mbOutPut,
    const CMEMessageBlock& mbHead, 
    const CMEMessageBlock& mbTail )
{
    /**
    * �����е��쳣�����
    * 1�����ô˷���ʱ����MB�ĵ�ǰдDB����DB�飬����Щ��̿�ɾ��
    * 2�����ô˷���ʱ����MB�ĵ�ǰдDB��û�б�д�꣨��ǰдDB��дλ�ú������ֶβ���ȣ���
    *    ��û��д�Ŀռ���Ե�������DB����ΪDBдλ�ã�
    */

    mbOutPut = mbHead;

    /* ����������һ���쳣��� */
    UINT iDBIndex = mbOutPut.m_iWriteDB + 1;  // ��ǰд���һ��
    for ( ; iDBIndex < mbOutPut.m_vectorDBs.size(); ++iDBIndex )
    {
        /* ��ȥ���� */
        mbOutPut.m_iCapacity -= mbOutPut.m_vectorDBs[iDBIndex]->GetCapacity();

        /* ɾ�����DB */
        mbOutPut.m_vectorDBs.erase( mbOutPut.m_vectorDBs.begin() + iDBIndex );
    }

    /* ���������ڶ����쳣��� */
    UINT iWriteDBCapacity = mbOutPut.m_vectorDBs.size() >  mbOutPut.m_iWriteDB ? mbOutPut.m_vectorDBs[mbHead.m_iWriteDB]->GetCapacity() : 0;
    UINT iWriteDBWritePos = mbOutPut.m_vectorDBs.size() >  mbOutPut.m_iWriteDB ? mbOutPut.m_vectorDBs[mbHead.m_iWriteDB]->GetWritePos() : 0;
    if ( iWriteDBCapacity != iWriteDBWritePos )
    {
        ME_ASSERTE_RETURN_VOID( (iWriteDBCapacity > iWriteDBWritePos) );

        /* ���Ե�ǰDBû��д��Ŀռ� */
        mbOutPut.m_iCapacity -= ( iWriteDBCapacity - iWriteDBWritePos );
        mbOutPut.m_vectorDBs[mbOutPut.m_iWriteDB]->SetCapacity( iWriteDBWritePos );

        /* �����ǰ�鱻������ϣ���ɾ���ÿ� */
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

    /* ����hMB��DB��ָ�뵽��ǰMBĩβ */
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

    /* �Ƿ��Ǳ��� */
    ME_ASSERTE_RETURN( (this != &hMB), *this );

    /* �Ƿ�ֻ�� */
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
    
    /* ��� */
    CMEMessageBlock* pmb = new CMEMessageBlock( m_iCapacity );

    //ME_ASSERTE_RETURN( (pmb && pmb->m_vectorDBs.size() > 0), NULL );

    /* ����hMB���������ݵ�pDB�� */
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

    /* ������MB������ */
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

    /* �û��������Ƿ�Ϊ��ָ�� */
    ME_ASSERTE_RETURN( pIOVec, 0 );

	ME_ASSERTE_RETURN( m_iWritePos >= m_iReadPos, 0 );

    /* DB�����Ƿ�С�����鳤�� */
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

    /* ����DB������������Ϣ����Ӧ����Ԫ�� */
    UINT iDBIndex = m_iReadDB;
    for ( ; iDBIndex < m_iReadDB + iCopyNum; ++iDBIndex )
    {
		if ( iDBIndex >= m_vectorDBs.size() )
		{
			continue;
		}

        /* ��������ַ */
        pIOVec->iov_base = (LPSTR)m_vectorDBs[iDBIndex]->GetBuffer() + m_vectorDBs[iDBIndex]->GetReadPos();

        /* ���������� */
        pIOVec->iov_len = m_vectorDBs[iDBIndex]->GetWritePos() - m_vectorDBs[iDBIndex]->GetReadPos();

        ++pIOVec;
    }

	return iCopyNum;
}

UINT CMEMessageBlock::Resize( UINT iMaxLengthDBSize )
{
	/* ���ݵ�ǰ��дλ�ã� �������Ҫ��DB���� */
	UINT iCurrentLengthDBSize = m_iWriteDB - m_iReadDB;

	if ( m_vectorDBs.size() > m_iWriteDB &&
		0 != m_vectorDBs[m_iWriteDB]->GetWritePos() )
	{
		++iCurrentLengthDBSize; 
	}

	/* ���û���㹻�ռ�����DB�� ��������DB�ϲ�Ϊһ�� */
	if ( iCurrentLengthDBSize > iMaxLengthDBSize )
	{
		UINT iCopySize = 0;
		UINT iCopyDBStart = m_iReadDB + iMaxLengthDBSize -1;

		/* �������Ҫ�ϲ�DB���ܳ��� */
		UINT iCopyDBIndex = iCopyDBStart;
		for( ; iCopyDBIndex < m_iReadDB + iCurrentLengthDBSize; ++iCopyDBIndex )
		{
			iCopySize += m_vectorDBs[iCopyDBIndex]->GetCapacity();
		}

		/* new һ��DB */
		CMEAutoPtrT<CMEDataBlock> pDB;
		try
		{
			pDB = new CMEDataBlock(iCopySize);
		}
		catch( std::bad_alloc )
		{
			return 0;
		}

		/* �������ɵ�DB��д������ */
		iCopyDBIndex = iCopyDBStart;
		for( ; iCopyDBIndex < m_iReadDB + iCurrentLengthDBSize; ++iCopyDBIndex )
		{
			pDB->Write( 
				m_vectorDBs[iCopyDBIndex]->GetBuffer(), 
				m_vectorDBs[iCopyDBIndex]->GetWritePos() );
		}

		/* ɾ���Ѿ�����õ�DB */
		m_vectorDBs.erase( 
			m_vectorDBs.begin() + iCopyDBStart,
			m_vectorDBs.begin() + m_iReadDB + iCurrentLengthDBSize );

		/* ���������ɵ�DB */
		m_vectorDBs.insert( 
			m_vectorDBs.begin() + iCopyDBStart, 
			pDB );

        SetWritePos( m_iWritePos );

        SetReadPos( m_iReadPos );
	}

	return iCurrentLengthDBSize < iMaxLengthDBSize ? iCurrentLengthDBSize : iMaxLengthDBSize;
}

ME_NAME_SPACE_END
