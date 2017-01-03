/**
* MessageBlock工厂类 公用数据块
* 
*
* History
*	1/15/2016		Created
*/

#ifndef __MESSAGEBLOCK_H_
#define __MESSAGEBLOCK_H_

#include "Base.h"
#include "AutoPtrT.h"
#include "ReferenceControlT.h"

ME_NAME_SPACE_BEGIN

class CMEDataBlock;

class ME_OS_EXPORT CMEMessageBlock : public CMEReferenceControlT<CMELockMutex>
{
public:
    /* 单元测试友元类 */
    friend class TestMB;

public:
    /* 读写位置移动方向 */
    enum DIRECTION_TYPE
    {
        DIRECTION_TYPE_FORWORD = 0,     // 向前

        DIRECTION_TYPE_BACKWORD = 1,    // 向后
    };

    /* 内存块属性 */
    enum MESSAGEBLOCK_ATTR
    {
        MESSAGEBLOCK_ATTR_NONE = 0,

        /* 只读 */
        MESSAGEBLOCK_ATTR_READONLY = (1 << 0),  

        /* 析构时释放用户数据（仅在第二种构造方式时使用） */
        MESSAGEBLOCK_ATTR_DELETE_USERDATA = (1 << 1),

        /* 上面两种属性的综合 */
        MESSAGEBLOCK_ATTR_READONLY_DELETE_USERDATA = MESSAGEBLOCK_ATTR_READONLY \
                                                    | MESSAGEBLOCK_ATTR_DELETE_USERDATA,
    };

public:
    /**
	* 指定缓冲区长度构造
	*   将包含一个iBufferLength长的缓冲区，如果申请内存失败，抛出bad_alloc异常
    *
    * <iBufferLength>，缓冲区长度
    * <iAttr>，默认是“不只读”
    *
    * 返回值：
    *        
	*/
    CMEMessageBlock(
        UINT iBufferLength,
        MESSAGEBLOCK_ATTR iAttr = MESSAGEBLOCK_ATTR_NONE );

    /**
	* 用用户的一段缓冲区构造
	*
	* <iBufferLength>, 用户缓冲区长度 
	* <pBufferAddr>, 用户缓冲区地址 
    * <iDataLength>, 缓冲区已写长度
    * <iAttr>，默认“不删除”、“不只读”
	*
	* 返回值:
	*		
	*/
    CMEMessageBlock(
        UINT iBufferLength,
        LPVOID pBufferAddr,
        UINT iDataLength, 
        MESSAGEBLOCK_ATTR iAttr = MESSAGEBLOCK_ATTR_NONE );

    /**
    * 拷贝构造（浅拷贝）
    *      
    * <hMB>，拷贝模板
	*
	* 返回值:
	*		
	*/
    CMEMessageBlock( const CMEMessageBlock& hMB );

    /**
    * 析构
	*/
    virtual ~CMEMessageBlock();

public:
    /**
    * "="操作符（浅拷贝）
    *      
    * <hMB>，拷贝模板
	*
	* 返回值:
	*		MB本身
	*/
    CMEMessageBlock& operator = ( const CMEMessageBlock& hMB );

    /**
    * 写数据
    *      
    * <pBuffer>，缓冲区地址
    * <iLength>，缓冲区长度
	*
	* 返回值:
	*		错误码，可能是“成功”，“空指针”， “内存不足”
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
	*		错误码，可能是“成功”，“空指针”， “超出范围”
	*/
    ME_Result Read(
        LPVOID pBUffer, 
        UINT iLength );

    /**
    * 获取当前写位置
    *      
	* 返回值:
	*		当前写位置
	*/
    UINT GetWritePos() const;

    /**
    * 设置当前写位置
    *
    * <iWritePos>，当前写位置
    *
	* 返回值:
	*		错误码，可能是“成功”，“超出范围”
	*/
    ME_Result SetWritePos( UINT iWritePos );

    /**
    * 移动（向前或向后）当前写位置
    *
    * <iDirection>，移动方向
    * <iStep>，移动步长
    *
	* 返回值:
	*		错误码，可能是“成功”，“超出范围”
	*/
    ME_Result MoveWritePos(
        DIRECTION_TYPE iDirection, 
        UINT iStep );

    /**
    * 获取当前读位置
    *
	* 返回值:
	*		当前读位置
	*/
    UINT GetReadPos() const;

    /**
    * 设置当前读位置
    *
    * <iWritePos>，当前读位置
    *
	* 返回值:
	*		错误码，可能是“成功”，“超出范围”
	*/
    ME_Result SetReadPos( UINT iReadPos );

    /**
    * 移动（向前或向后）当前读位置
    *
    * <iDirection>，移动方向
    * <iStep>，移动步长
    *
	* 返回值:
	*		错误码，可能是“成功”，“超出范围”
	*/
    ME_Result MoveReadPos( 
        DIRECTION_TYPE iDirection, 
        UINT iStep );

    /**
    * 获取容量（缓冲区总长度）
    *
	* 返回值:
	*		容量
	*/
    UINT GetCapacity() const;

    /**
    * 获取有效数据长度
    *   有效数据是已经写入但还没有读取的数据
    *
	* 返回值:
	*	有效数据长度
	*/
    UINT GetLength() const;

    /**
    * 获取MB空闲空间大小
    *
	* 返回值:
	*	空闲空间大小
	*/
    UINT GetFreeLength() const;

    /**
    * 累加
    *    在当前内存块后累加内存块,如果当前MB还有空间没有写，则会被释放
    *    累加后的读位置=0，写位置是hMB的写位置
    *
    * <hMB>，待累加的MB
    * <bTail>，如果为TRUE，向尾部累加
    *          如果为FALSE，向头部添加
    *
	* 返回值:
    *    MB本身
	*/
    CMEMessageBlock& Append( const CMEMessageBlock& hMB, BOOL bTail = TRUE );

    /**
    * Clone出一个和当前MB相同的MB
    *    新科隆出的MB能够保证所含有的缓冲区能被MB删除
    *    申请内存，失败抛出bad_alloc异常
    *
	* 返回值:
    *    新Clone出的MB，用户需负责该MB的回收
	*/
    CMEMessageBlock* Clone();

    /**
    * 重置
    *    设置写位置为0、读位置为0
    *    
	* 返回值:
    *
	*/
    VOID Reset();

	/**
	* 清空MB
	* 含有的内存将释放
	*/
	VOID Clear();

    /**
    * 用MB内部所有DB块信息填充iovec数组
    *
    * <pIOVec>，被填充数组地址
    * <iIOVecNum>，数组长度。
    *    
	* 返回值:
    *   被填充的iovec数组元素个数
	*/
    UINT FillIOVec( iovec* pIOVec, UINT iIOVecNum );

private:
	/**
	* 对外接口Append将调用此方法
	*/
    VOID Append( 
        CMEMessageBlock& mbOutPut,
        const CMEMessageBlock& mbHead, 
        const CMEMessageBlock& mbTail );

	UINT Resize( UINT iMaxLengthDBSize );

private:
    /* 存储DB指针容器类型 */
    typedef std::vector< CMEAutoPtrT<CMEDataBlock> > DBContainerType;

private:
    DBContainerType                 m_vectorDBs;    // 存储所有DB指针
    UINT                            m_iWriteDB;     // 当前写DB在容器中的位置
    UINT                            m_iReadDB;      // 当前读DB 在容器中的位置

    UINT                            m_iCapacity;    // 缓冲区总长度
    UINT                            m_iWritePos;    // 缓冲区当前写位置
    UINT                            m_iReadPos;     // 缓冲区当前读位置

    DWORD                           m_iAttr;        // 属性

}; 

typedef CMEAutoPtrT<CMEMessageBlock> CMEMessageBlockAutoPtr;

ME_NAME_SPACE_END

#endif // __MESSAGEBLOCK_H_
