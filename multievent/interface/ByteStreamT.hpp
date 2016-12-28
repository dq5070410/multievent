
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

    /* ֻ��һ���ֽڣ��������л� */
    m_pMB->Write( &Value, sizeof(Value) );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator >> ( CHAR& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

	Value = 0;

    ME_ASSERTE_RETURN( m_pMB, *this );

    /* ֻ��һ���ֽڣ����跴���л� */
    m_pMB->Read( &Value, sizeof(Value) );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator << ( BYTE Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    ME_ASSERTE_RETURN( m_pMB, *this );

    /* ֻ��һ���ֽڣ��������л� */
    m_pMB->Write( &Value, sizeof(Value) );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator >> ( BYTE& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

	Value = 0;

    ME_ASSERTE_RETURN( m_pMB, *this );

    /* ֻ��һ���ֽڣ����跴���л� */
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

    /* �Ƿ����㹻�ռ������ַ������ַ������� */
    if ( m_pMB->GetFreeLength() < sizeof(UINT) + strlen(Value) )
    {
		ME_ERROR_TRACE_THIS( "MB Not Have Enough Space To Store." << m_pMB->GetFreeLength() );

        return *this;
    }

    /* �ַ�������д��MB */
    UINT iStrLen = strlen( Value );
    Write(iStrLen);

    /* �ַ�������д��MB */
    m_pMB->Write( Value, iStrLen );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator << ( const CMEString& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

    ME_ASSERTE_RETURN( m_pMB, *this );


    /* �Ƿ����㹻�ռ������ַ������ַ������� */
    if ( m_pMB->GetFreeLength() < sizeof(UINT) + Value.length() )
    {
        return *this;
    }

    /* �ַ�������д��MB */
    UINT iStrLen = Value.length();
    Write( iStrLen );

    /* �ַ�������д��MB */
    m_pMB->Write( Value.data(), iStrLen );

    return *this;
}

template<class MBType>
CByteStreamT<MBType>& CByteStreamT<MBType>::operator >> ( CMEString& Value )
{
    ME_INFO_TRACE_THIS( "Value" << Value );

	Value = "";

    ME_ASSERTE_RETURN( m_pMB, *this );

    /* �����ַ������� */
    UINT iStrLen = 0;
    ME_Result ret = Read( iStrLen );
    if ( ME_FAILED(ret) )
    {
        return *this;
    }

    /* �����ַ������� */
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
        /* ��ֵ��λ���ڸߵ�ַ */
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
        /* �͵�ַ�洢�������ָ�λ */
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
        /* С�� */
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

    /* ��MB �������� */
    ME_Result ret = m_pMB->Read( Buffer, sizeof(T) );
    if ( ME_FAILED(ret) )
    {
        return ret;
    }

    if ( TRUE == m_bBigEndian )
    {
        /* ������ˣ����跴���л� */
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
        /* ����С�ˣ���Ҫ�����л� */
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
    * ���������С�˴洢�����ڴ���e���ڴ��ǣ�02 01����ʱe.c = 2
    * ��������Ǵ�˴洢�����ڴ���e���ڴ��ǣ�01 02����ʱe.c = 1
    */
    return ( 1 == e.c ? TRUE : FALSE );
}
