/**
* MessageBlock������ �������ݿ�
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
    /* ��Ԫ������Ԫ�� */
    friend class TestMB;

public:
    /* ��дλ���ƶ����� */
    enum DIRECTION_TYPE
    {
        DIRECTION_TYPE_FORWORD = 0,     // ��ǰ

        DIRECTION_TYPE_BACKWORD = 1,    // ���
    };

    /* �ڴ������ */
    enum MESSAGEBLOCK_ATTR
    {
        MESSAGEBLOCK_ATTR_NONE = 0,

        /* ֻ�� */
        MESSAGEBLOCK_ATTR_READONLY = (1 << 0),  

        /* ����ʱ�ͷ��û����ݣ����ڵڶ��ֹ��췽ʽʱʹ�ã� */
        MESSAGEBLOCK_ATTR_DELETE_USERDATA = (1 << 1),

        /* �����������Ե��ۺ� */
        MESSAGEBLOCK_ATTR_READONLY_DELETE_USERDATA = MESSAGEBLOCK_ATTR_READONLY \
                                                    | MESSAGEBLOCK_ATTR_DELETE_USERDATA,
    };

public:
    /**
	* ָ�����������ȹ���
	*   ������һ��iBufferLength���Ļ���������������ڴ�ʧ�ܣ��׳�bad_alloc�쳣
    *
    * <iBufferLength>������������
    * <iAttr>��Ĭ���ǡ���ֻ����
    *
    * ����ֵ��
    *        
	*/
    CMEMessageBlock(
        UINT iBufferLength,
        MESSAGEBLOCK_ATTR iAttr = MESSAGEBLOCK_ATTR_NONE );

    /**
	* ���û���һ�λ���������
	*
	* <iBufferLength>, �û����������� 
	* <pBufferAddr>, �û���������ַ 
    * <iDataLength>, ��������д����
    * <iAttr>��Ĭ�ϡ���ɾ����������ֻ����
	*
	* ����ֵ:
	*		
	*/
    CMEMessageBlock(
        UINT iBufferLength,
        LPVOID pBufferAddr,
        UINT iDataLength, 
        MESSAGEBLOCK_ATTR iAttr = MESSAGEBLOCK_ATTR_NONE );

    /**
    * �������죨ǳ������
    *      
    * <hMB>������ģ��
	*
	* ����ֵ:
	*		
	*/
    CMEMessageBlock( const CMEMessageBlock& hMB );

    /**
    * ����
	*/
    virtual ~CMEMessageBlock();

public:
    /**
    * "="��������ǳ������
    *      
    * <hMB>������ģ��
	*
	* ����ֵ:
	*		MB����
	*/
    CMEMessageBlock& operator = ( const CMEMessageBlock& hMB );

    /**
    * д����
    *      
    * <pBuffer>����������ַ
    * <iLength>������������
	*
	* ����ֵ:
	*		�����룬�����ǡ��ɹ���������ָ�롱�� ���ڴ治�㡱
	*/
    ME_Result Write(
        LPCVOID pBuffer,
        UINT iLength );

    /**
    * ������
    *      
    * <pBuffer>����������ַ
    * <iLength>������������
	*
	* ����ֵ:
	*		�����룬�����ǡ��ɹ���������ָ�롱�� ��������Χ��
	*/
    ME_Result Read(
        LPVOID pBUffer, 
        UINT iLength );

    /**
    * ��ȡ��ǰдλ��
    *      
	* ����ֵ:
	*		��ǰдλ��
	*/
    UINT GetWritePos() const;

    /**
    * ���õ�ǰдλ��
    *
    * <iWritePos>����ǰдλ��
    *
	* ����ֵ:
	*		�����룬�����ǡ��ɹ�������������Χ��
	*/
    ME_Result SetWritePos( UINT iWritePos );

    /**
    * �ƶ�����ǰ����󣩵�ǰдλ��
    *
    * <iDirection>���ƶ�����
    * <iStep>���ƶ�����
    *
	* ����ֵ:
	*		�����룬�����ǡ��ɹ�������������Χ��
	*/
    ME_Result MoveWritePos(
        DIRECTION_TYPE iDirection, 
        UINT iStep );

    /**
    * ��ȡ��ǰ��λ��
    *
	* ����ֵ:
	*		��ǰ��λ��
	*/
    UINT GetReadPos() const;

    /**
    * ���õ�ǰ��λ��
    *
    * <iWritePos>����ǰ��λ��
    *
	* ����ֵ:
	*		�����룬�����ǡ��ɹ�������������Χ��
	*/
    ME_Result SetReadPos( UINT iReadPos );

    /**
    * �ƶ�����ǰ����󣩵�ǰ��λ��
    *
    * <iDirection>���ƶ�����
    * <iStep>���ƶ�����
    *
	* ����ֵ:
	*		�����룬�����ǡ��ɹ�������������Χ��
	*/
    ME_Result MoveReadPos( 
        DIRECTION_TYPE iDirection, 
        UINT iStep );

    /**
    * ��ȡ�������������ܳ��ȣ�
    *
	* ����ֵ:
	*		����
	*/
    UINT GetCapacity() const;

    /**
    * ��ȡ��Ч���ݳ���
    *   ��Ч�������Ѿ�д�뵫��û�ж�ȡ������
    *
	* ����ֵ:
	*	��Ч���ݳ���
	*/
    UINT GetLength() const;

    /**
    * ��ȡMB���пռ��С
    *
	* ����ֵ:
	*	���пռ��С
	*/
    UINT GetFreeLength() const;

    /**
    * �ۼ�
    *    �ڵ�ǰ�ڴ����ۼ��ڴ��,�����ǰMB���пռ�û��д����ᱻ�ͷ�
    *    �ۼӺ�Ķ�λ��=0��дλ����hMB��дλ��
    *
    * <hMB>�����ۼӵ�MB
    * <bTail>�����ΪTRUE����β���ۼ�
    *          ���ΪFALSE����ͷ�����
    *
	* ����ֵ:
    *    MB����
	*/
    CMEMessageBlock& Append( const CMEMessageBlock& hMB, BOOL bTail = TRUE );

    /**
    * Clone��һ���͵�ǰMB��ͬ��MB
    *    �¿�¡����MB�ܹ���֤�����еĻ������ܱ�MBɾ��
    *    �����ڴ棬ʧ���׳�bad_alloc�쳣
    *
	* ����ֵ:
    *    ��Clone����MB���û��踺���MB�Ļ���
	*/
    CMEMessageBlock* Clone();

    /**
    * ����
    *    ����дλ��Ϊ0����λ��Ϊ0
    *    
	* ����ֵ:
    *
	*/
    VOID Reset();

	/**
	* ���MB
	* ���е��ڴ潫�ͷ�
	*/
	VOID Clear();

    /**
    * ��MB�ڲ�����DB����Ϣ���iovec����
    *
    * <pIOVec>������������ַ
    * <iIOVecNum>�����鳤�ȡ�
    *    
	* ����ֵ:
    *   ������iovec����Ԫ�ظ���
	*/
    UINT FillIOVec( iovec* pIOVec, UINT iIOVecNum );

private:
	/**
	* ����ӿ�Append�����ô˷���
	*/
    VOID Append( 
        CMEMessageBlock& mbOutPut,
        const CMEMessageBlock& mbHead, 
        const CMEMessageBlock& mbTail );

	UINT Resize( UINT iMaxLengthDBSize );

private:
    /* �洢DBָ���������� */
    typedef std::vector< CMEAutoPtrT<CMEDataBlock> > DBContainerType;

private:
    DBContainerType                 m_vectorDBs;    // �洢����DBָ��
    UINT                            m_iWriteDB;     // ��ǰдDB�������е�λ��
    UINT                            m_iReadDB;      // ��ǰ��DB �������е�λ��

    UINT                            m_iCapacity;    // �������ܳ���
    UINT                            m_iWritePos;    // ��������ǰдλ��
    UINT                            m_iReadPos;     // ��������ǰ��λ��

    DWORD                           m_iAttr;        // ����

}; 

typedef CMEAutoPtrT<CMEMessageBlock> CMEMessageBlockAutoPtr;

ME_NAME_SPACE_END

#endif // __MESSAGEBLOCK_H_
