/**
* DataBlock������ ����ģ��
*
*
* History
*	1/29/2013		Created
*/

#ifndef __BYTESTREAM_H_
#define __BYTESTREAM_H_

#include "Base.h"

NAME_SPACE_BEGIN

template<class MBType>
class CByteStreamT
{
public:
    friend class BSTest;

public:
    /**
	* ָ�� ���ô洢�� ����
    *
    * <pMB>�����ݽ���Ӵ˹��ô洢�������д��
    *
    * ����ֵ��
    *        
	*/
    CByteStreamT( MBType* pMB );

    /**
	* ����
	*/
    virtual ~CByteStreamT();

private:
    /* ��ֹ���� */
    CByteStreamT( const CByteStreamT& );
    CByteStreamT& operator= ( const CByteStreamT& );

public:
    /**
	* ��ȡMB�飬��MB���к��������Ѿ�д�������
    *
    * ����ֵ��
    *    MBָ��
	*/
    MBType* GetMB();

    /**
	* ����MB�飬������ɺ����ݽ��ᱻд�뵽�����õ�MB����
    *
    * <MB>��Ҫ���õ�MB��
    *
    * ����ֵ��
    *        
	*/
    VOID SetMB( MBType* MB );

public:
    /* �Ը����������ݵ�д�롢���� */
    CByteStreamT& operator << ( CHAR Value );
    CByteStreamT& operator >> ( CHAR& Value );

    CByteStreamT& operator << ( BYTE Value );
    CByteStreamT& operator >> ( BYTE& Value );

    CByteStreamT& operator << ( SHORT Value );
    CByteStreamT& operator >> ( SHORT& Value );

    CByteStreamT& operator << ( WORD Value );
    CByteStreamT& operator >> ( WORD& Value );

    CByteStreamT& operator << ( INT Value );
    CByteStreamT& operator >> ( INT& Value );

    CByteStreamT& operator << ( UINT Value );
    CByteStreamT& operator >> ( UINT& Value );

    CByteStreamT& operator << ( LONG Value );
    CByteStreamT& operator >> ( LONG& Value );

    CByteStreamT& operator << ( ULONG Value );
    CByteStreamT& operator >> ( ULONG& Value );

    CByteStreamT& operator << ( LONGLONG Value );
    CByteStreamT& operator >> ( LONGLONG& Value );

    CByteStreamT& operator << ( ULONGLONG Value );
    CByteStreamT& operator >> ( ULONGLONG& Value );

    CByteStreamT& operator << ( FLOAT Value );
    CByteStreamT& operator >> ( FLOAT& Value );

    CByteStreamT& operator << ( DOUBLE Value );
    CByteStreamT& operator >> ( DOUBLE& Value );

    CByteStreamT& operator << ( LPCSTR Value );

    CByteStreamT& operator << ( const CHikString& Value );
    CByteStreamT& operator >> ( CHikString& Value );

private:
    /**
	* �����ֽڱ������л�Ϊ��˴洢
    *
    * <pBuffer>����������ַ�����ݽ��ᱻ���л��������������
    * <iLength>������������
    * <Value>����Ҫ�����л��ı���
    *
    * ����ֵ��
	*/
    template<typename T>
    VOID SerilizeToBigEndian( 
        LPBYTE pBuffer, 
        UINT iLength, 
        T Value );

    /**
	* �Դ�˷�ʽ���Ͷ��ֽڱ���
    *
    * <pBuffer>����������ַ�����ݽ�����������������
    * <iLength>������������
    * <Value>�������л����ı���
    *
    * ����ֵ��
	*/
    template<typename T>
    VOID DeSerilizeFromBigEndian( 
         LPCBYTE pBuffer, 
         UINT iLength, 
         T& Value );

    /**
	* ������д�뵽�������ݿ�
    * ��������Ǵ�ˣ���ֱ�ӽ�����д�뵽�������ݿ���
    * ���������С�ˣ��������л�Ϊ��˺󣨵���SerilizeToBigEndian����д�뵽�������ݿ���
    *
    * <Value>��Ҫд�������
    *
    * ����ֵ��
    *    CByteStreamT����
	*/
    template<class T>
    ME_Result Write( T Value );

    /**
	* �����ݴӹ������ݿ����
    * ��������Ǵ�ˣ���ֱ�Ӷ���
    * ���������С�ˣ���ӹ������ݿ�����󣬰���С�˷�ʽ���ͣ�����DeSerilizeFromBigEndian��
    *
    * <Value>��Ҫд�������
    *
    * ����ֵ��
    *    CByteStreamT����
	*/
    template<class T>
    ME_Result Read( T& Value );

    /**
	* �����ֽ����Ƿ��Ǵ��
    *
    * ����ֵ��
    *   �Ƿ��Ǵ�ˣ������ֽ���
	*/
    BOOL IsBigEndian();

private:
    /* �����ֽ����Ƿ��Ǵ�ˣ������ֽ��� */
    BOOL                            m_bBigEndian;

    /* �洢�����ڴ�� */
    MBType*                         m_pMB;

};

#include "ByteStreamT.hpp"

NAME_SPACE_END

#endif // __HIK_BYTESTREAM_H_
