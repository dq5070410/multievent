/**
* DataBlock工厂类 流化模块
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
	* 指定 公用存储块 构造
    *
    * <pMB>，数据将会从此公用存储块读出或写入
    *
    * 返回值：
    *        
	*/
    CByteStreamT( MBType* pMB );

    /**
	* 析构
	*/
    virtual ~CByteStreamT();

private:
    /* 禁止拷贝 */
    CByteStreamT( const CByteStreamT& );
    CByteStreamT& operator= ( const CByteStreamT& );

public:
    /**
	* 获取MB块，该MB块中含有所有已经写入的数据
    *
    * 返回值：
    *    MB指针
	*/
    MBType* GetMB();

    /**
	* 设置MB块，设置完成后，数据将会被写入到新设置的MB块中
    *
    * <MB>，要设置的MB块
    *
    * 返回值：
    *        
	*/
    VOID SetMB( MBType* MB );

public:
    /* 对各种类型数据的写入、读出 */
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
	* 将多字节变量序列化为大端存储
    *
    * <pBuffer>，缓冲区地址，数据将会被序列化到这个缓冲区中
    * <iLength>，缓冲区长度
    * <Value>，需要被序列化的变量
    *
    * 返回值：
	*/
    template<typename T>
    VOID SerilizeToBigEndian( 
        LPBYTE pBuffer, 
        UINT iLength, 
        T Value );

    /**
	* 以大端方式解释多字节变量
    *
    * <pBuffer>，缓冲区地址，数据将会从这个缓冲区读出
    * <iLength>，缓冲区长度
    * <Value>，反序列化出的变量
    *
    * 返回值：
	*/
    template<typename T>
    VOID DeSerilizeFromBigEndian( 
         LPCBYTE pBuffer, 
         UINT iLength, 
         T& Value );

    /**
	* 将数据写入到公用数据块
    * 如果本机是大端，则直接将数据写入到公用数据块中
    * 如果本机是小端，则先序列化为大端后（调用SerilizeToBigEndian）再写入到公用数据块中
    *
    * <Value>，要写入的数据
    *
    * 返回值：
    *    CByteStreamT本身
	*/
    template<class T>
    ME_Result Write( T Value );

    /**
	* 将数据从公用数据块读出
    * 如果本机是大端，则直接读出
    * 如果本机是小端，则从公用数据块读出后，按则按小端方式解释（调用DeSerilizeFromBigEndian）
    *
    * <Value>，要写入的数据
    *
    * 返回值：
    *    CByteStreamT本身
	*/
    template<class T>
    ME_Result Read( T& Value );

    /**
	* 本机字节序是否是大端
    *
    * 返回值：
    *   是否是大端（网络字节序）
	*/
    BOOL IsBigEndian();

private:
    /* 本机字节序是否是大端（网络字节序） */
    BOOL                            m_bBigEndian;

    /* 存储数据内存块 */
    MBType*                         m_pMB;

};

#include "ByteStreamT.hpp"

NAME_SPACE_END

#endif // __HIK_BYTESTREAM_H_
