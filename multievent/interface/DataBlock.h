/**
* 工厂类 公用数据块子模块
*
*
* History
*	1/28/2016		Created
*/

#ifndef __DATABLOCK_H_
#define __DATABLOCK_H_

#include "Base.h"
#include "LockMutex.h"
#include "ReferenceControlT.h"

ME_NAME_SPACE_BEGIN

class CMEDataBlock : public CMEReferenceControlT<CMELockMutex>
{
public:
    /* 单元测试友元 */
    friend class TestMB;

public:
    /**
    * 用缓冲区长度构造DB
    *
    * <iBufferLength>，DB所含缓冲区长度
    *
	* 返回值:
    *
	*/
    CMEDataBlock( UINT iBufferLength );

    /**
    * 用用户内存缓冲区构造DB
    *
    * <iLength>，用户缓冲区长度
    * <pAddr>，用户缓冲区地址
    * <iDataLength>，用户缓冲区已写长度
    * <bDelete>，是否删除用户数据
    *
	* 返回值:
    *
	*/
    CMEDataBlock( 
        UINT iBufferLength, 
        LPVOID pBufferAddr,
        UINT iDataLength,
        BOOL bDelete = FALSE );

    /**
    * 析构
    * 判读 “是否删除” 属性，如果为TRUE，释放缓冲区
    *
	* 返回值:
    *
	*/
    ~CMEDataBlock();

public:
    /**
    * 写数据
    *
    * <pBuffer>，数据地址
    * <iLength>，数据长度
    *
	* 返回值:
    *   错误码，
    */
    ME_Result Write(
        LPCVOID pBuffer,
        UINT iLength );

    /**
    * 读数据
    *
    * <pBuffer>，缓冲区地址
    * <iLength>，缓冲区长度
    *
	* 返回值:
    *   错误码
	*/
    ME_Result Read(
        LPVOID pBuffer, 
        UINT iLength );

    /**
    * 获取写位置
    *
	* 返回值:
    *   当前写位置
	*/
    UINT GetWritePos() const;

    /**
    * 获取读位置
    *
	* 返回值:
    *   当前读位置
	*/
    UINT GetReadPos() const;

    /**
    * 设置写位置
    *
    * <iWritePos>，要设置的写位置
    *
	* 返回值:
    *   错误码
	*/
    ME_Result SetWritePos( UINT iWritePos );

    /**
    * 设置读位置
    *
    * <iReadPos>，要设置的读位置
    *
	* 返回值:
    *   错误码
	*/
    ME_Result SetReadPos( UINT iReadPos );

    /**
    * 获取有效数据（已写但未读）长度
    *
	* 返回值:
    *   有效数据长度
	*/
    UINT GetLength() const;

    /**
    * 获取缓冲区地址
    *
	* 返回值:
    *   缓冲区地址
	*/
    LPVOID GetBuffer() const;

    /**
    * 获取容量
    *
	* 返回值:
    *   容量
	*/
    UINT GetCapacity() const;

    /**
    * 设置容量（在Append时可能会用到）
    *
    * <iCapacity>，新的容量值
    * <pMB>，谁在设置容量
    *
	* 返回值:
    *
	*/
    VOID SetCapacity( UINT iCapacity );

    /**
    * 获取缓冲区空闲空间长度
    *
	* 返回值:
    *   缓冲区空闲空间长度
	*/
    UINT GetFreeLength() const;

    BOOL IsDelete() const;

private:
    LPVOID                          m_pBuffer;     // 缓冲区地址
    UINT                            m_iCapacity;   // 缓冲区容量

    UINT                            m_iReadPos;    // 当前读位置
    UINT                            m_iWritePos;   // 当前写位置 

    BOOL                            m_bDelete;     // 是否删除

};

typedef CMEDataBlock* PCMEDataBlock;

ME_NAME_SPACE_END

#endif // __ME_DATABLOCK_H_
