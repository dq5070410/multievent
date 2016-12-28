
template<class MBType>
CByteStreamT<MBType>::CByteStreamT( MBType* pMB )
    : m_bBigEndian( IsBigEndian() )
    , m_pMB( pMB )
{
    ME_INFO_TRACE_THIS( "BIG Endian=" << m_bBigEndian << "MB Address=" << m_pMB );
}

template<class MBType>
CByteStreamT<MBType>::~CByteStreamT()
{
    ME_INFO_TRACE_THIS( "" );
}

template<class MBType>
MBType* CByteStreamT<MBType>::GetMB()
{
    ME_INFO_TRACE_THIS( "" );

    return m_pMB;
}

template<class MBType>
VOID CByteStreamT<MBType>::SetMB( MBType* pMB )
{
    ME_INFO_TRACE_THIS( "pMB=" << pMB );

    m_pMB = pMB;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator << ( CHAR Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    ME_ASSERTE_RETURN( m_pMB, *this );

    /* 只有一个字节，不需序列化 */
    m_pMB->Write( &Value, sizeof(Value) );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator >> ( CHAR& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

	Value = 0;

    ME_ASSERTE_RETURN( m_pMB, *this );

    /* 只有一个字节，不需反序列化 */
    m_pMB->Read( &Value, sizeof(Value) );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator << ( BYTE Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    ME_ASSERTE_RETURN( m_pMB, *this );

    /* 只有一个字节，不需序列化 */
    m_pMB->Write( &Value, sizeof(Value) );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator >> ( BYTE& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

	Value = 0;

    ME_ASSERTE_RETURN( m_pMB, *this );

    /* 只有一个字节，不需反序列化 */
    m_pMB->Read( &Value, sizeof(Value) );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator << ( SHORT Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    Write( Value );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator >> ( SHORT& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    Read( Value );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator << ( WORD Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    Write( Value );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator >> ( WORD& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    Read( Value );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator << ( INT Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    Write( Value );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator >> ( INT& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    Read( Value );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator << ( UINT Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    Write( Value );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator >> ( UINT& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    Read( Value );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator << ( LONG Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    Write( Value );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator >> ( LONG& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    Read( Value );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator << ( ULONG Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    Write( Value );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator >> ( ULONG& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    Read( Value );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator << ( LONGLONG Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    Write( Value );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator >> ( LONGLONG& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    Read( Value );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator << ( ULONGLONG Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    Write( Value );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator >> ( ULONGLONG& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    Read( Value );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator << ( FLOAT Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    ME_ASSERTE_RETURN( m_pMB, *this );

    m_pMB->Write( &Value, sizeof(Value) );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator >> ( FLOAT& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

	Value = 0;

    ME_ASSERTE_RETURN( m_pMB, *this );

    m_pMB->Read( &Value, sizeof(Value) );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator << ( DOUBLE Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    ME_ASSERTE_RETURN( m_pMB, *this );

    m_pMB->Write( &Value, sizeof(Value) );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator >> ( DOUBLE& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

	Value = 0;

    ME_ASSERTE_RETURN( m_pMB, *this );

    m_pMB->Read( &Value, sizeof(Value) );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator << ( LPCSTR Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    ME_ASSERTE_RETURN( m_pMB, *this );

    /* 是否有足够空间容纳字符串和字符串长度 */
    if ( m_pMB->GetFreeLength() < sizeof(UINT) + strlen(Value) )
    {
		ME_ERROR_TRACE_THIS( "MB Not Have Enough Space To Store." << m_pMB->GetFreeLength() );

        return *this;
    }

    /* 字符串长度写入MB */
    UINT iStrLen = strlen( Value );
    Write(iStrLen);

    /* 字符串内容写入MB */
    m_pMB->Write( Value, iStrLen );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator << ( const CMEString& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    ME_ASSERTE_RETURN( m_pMB, *this );


    /* 是否有足够空间容纳字符串和字符串长度 */
    if ( m_pMB->GetFreeLength() < sizeof(UINT) + Value.length() )
    {
        return *this;
    }

    /* 字符串长度写入MB */
    UINT iStrLen = Value.length();
    Write( iStrLen );

    /* 字符串内容写入MB */
    m_pMB->Write( Value.data(), iStrLen );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator >> ( CMEString& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

	Value = "";

    ME_ASSERTE_RETURN( m_pMB, *this );

    /* 读出字符串长度 */
    UINT iStrLen = 0;
    ME_Result ret = Read( iStrLen );
    if ( ME_FAILED(ret) )
    {
        return *this;
    }

    /* 读出字符串内容 */
    Value.resize( iStrLen );
    m_pMB->Read( (LPVOID)Value.data(), iStrLen );

    return *this;
}

template<class MBType>
template<class T>
VOID CByteStreamT<MBType>::SerilizeToBigEndian( 
    LPBYTE pBuffer, 
    UINT iLength, 
    T Value )
{
    UINT iValueSize = sizeof( Value );
    ME_ASSERTE_RETURN_VOID( (iLength >= iValueSize) );

    while( iValueSize-- > 0 )
    {
        /* 数值低位放在高地址 */
        pBuffer[iValueSize] = Value & 0x0FF;
        Value >>= 8;
    }
}

template<class MBType>
template<class T>
VOID CByteStreamT<MBType>::DeSerilizeFromBigEndian( 
    LPCBYTE pBuffer, 
    UINT iLength, 
    T& Value )
{
    UINT iValueSize = sizeof( Value );
    ME_ASSERTE_RETURN_VOID( (iLength >= iValueSize) );

    UINT iByteIndex = 0;
    while ( iByteIndex < iValueSize )
    {
        /* 低地址存储的是数字高位 */
        Value |= pBuffer[iByteIndex++];
        if ( iByteIndex < iValueSize )
        {
            Value <<= 8;
        }
    }
}

template<class MBType>
template<class T>
ME_Result CByteStreamT<MBType>::Write( T Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    ME_ASSERTE_RETURN( m_pMB, ME_ERROR_NULL_POINTER );

    ME_Result ret = ME_OK;
    if ( TRUE == m_bBigEndian )
    {
        ret = m_pMB->Write( &Value, sizeof(T) );
    }
    else
    {
        /* 小端 */
        BYTE Buffer[sizeof(T)];
        memset( Buffer, 0, sizeof(T) );

        SerilizeToBigEndian( Buffer, sizeof(T), Value );

        ret = m_pMB->Write( Buffer, sizeof(T) );
    }

    return ret;
}

template<class MBType>
template<class T>
ME_Result CByteStreamT<MBType>::Read( T& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    Value = 0;

    ME_ASSERTE_RETURN( m_pMB, ME_ERROR_NULL_POINTER );

    BYTE Buffer[sizeof(T)];
    memset( Buffer, 0, sizeof(T) );

    /* 从MB 读出数据 */
    ME_Result ret = m_pMB->Read( Buffer, sizeof(T) );
    if ( ME_FAILED(ret) )
    {
        return ret;
    }

    if ( TRUE == m_bBigEndian )
    {
        /* 本机大端，不需反序列化 */
#ifdef ME_WIN
        ::memcpy_s( 
            &Value, 
            sizeof(T), 
            Buffer, 
            sizeof(T) );
#else
        ::memcpy(
            &Value, 
            Buffer, 
            sizeof(T) );
#endif
    }
    else
    {
        /* 本机小端，需要反序列化 */
        DeSerilizeFromBigEndian( Buffer, sizeof(T), Value );
    }

    return ME_OK;
}

template<class MBType>
BOOL CByteStreamT<MBType>::IsBigEndian()
{
    ME_INFO_TRACE_THIS( "" );

    union EU
    {
        SHORT s;
        CHAR c;
    };

    EU e;
    e.s = 0x0102;

    /* 
    * 如果本机是小端存储，在内存中e的内存是：02 01，此时e.c = 2
    * 如果本机是大端存储，在内存中e的内存是：01 02，此时e.c = 1
    */
    return ( 1 == e.c ? TRUE : FALSE );
}
