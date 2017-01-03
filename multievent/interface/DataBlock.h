/**
* ������ �������ݿ���ģ��
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
    /* ��Ԫ������Ԫ */
    friend class TestMB;

public:
    /**
    * �û��������ȹ���DB
    *
    * <iBufferLength>��DB��������������
    *
	* ����ֵ:
    *
	*/
    CMEDataBlock( UINT iBufferLength );

    /**
    * ���û��ڴ滺��������DB
    *
    * <iLength>���û�����������
    * <pAddr>���û���������ַ
    * <iDataLength>���û���������д����
    * <bDelete>���Ƿ�ɾ���û�����
    *
	* ����ֵ:
    *
	*/
    CMEDataBlock( 
        UINT iBufferLength, 
        LPVOID pBufferAddr,
        UINT iDataLength,
        BOOL bDelete = FALSE );

    /**
    * ����
    * �ж� ���Ƿ�ɾ���� ���ԣ����ΪTRUE���ͷŻ�����
    *
	* ����ֵ:
    *
	*/
    ~CMEDataBlock();

public:
    /**
    * д����
    *
    * <pBuffer>�����ݵ�ַ
    * <iLength>�����ݳ���
    *
	* ����ֵ:
    *   �����룬
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
    *   ������
	*/
    ME_Result Read(
        LPVOID pBuffer, 
        UINT iLength );

    /**
    * ��ȡдλ��
    *
	* ����ֵ:
    *   ��ǰдλ��
	*/
    UINT GetWritePos() const;

    /**
    * ��ȡ��λ��
    *
	* ����ֵ:
    *   ��ǰ��λ��
	*/
    UINT GetReadPos() const;

    /**
    * ����дλ��
    *
    * <iWritePos>��Ҫ���õ�дλ��
    *
	* ����ֵ:
    *   ������
	*/
    ME_Result SetWritePos( UINT iWritePos );

    /**
    * ���ö�λ��
    *
    * <iReadPos>��Ҫ���õĶ�λ��
    *
	* ����ֵ:
    *   ������
	*/
    ME_Result SetReadPos( UINT iReadPos );

    /**
    * ��ȡ��Ч���ݣ���д��δ��������
    *
	* ����ֵ:
    *   ��Ч���ݳ���
	*/
    UINT GetLength() const;

    /**
    * ��ȡ��������ַ
    *
	* ����ֵ:
    *   ��������ַ
	*/
    LPVOID GetBuffer() const;

    /**
    * ��ȡ����
    *
	* ����ֵ:
    *   ����
	*/
    UINT GetCapacity() const;

    /**
    * ������������Appendʱ���ܻ��õ���
    *
    * <iCapacity>���µ�����ֵ
    * <pMB>��˭����������
    *
	* ����ֵ:
    *
	*/
    VOID SetCapacity( UINT iCapacity );

    /**
    * ��ȡ���������пռ䳤��
    *
	* ����ֵ:
    *   ���������пռ䳤��
	*/
    UINT GetFreeLength() const;

    BOOL IsDelete() const;

private:
    LPVOID                          m_pBuffer;     // ��������ַ
    UINT                            m_iCapacity;   // ����������

    UINT                            m_iReadPos;    // ��ǰ��λ��
    UINT                            m_iWritePos;   // ��ǰдλ�� 

    BOOL                            m_bDelete;     // �Ƿ�ɾ��

};

typedef CMEDataBlock* PCMEDataBlock;

ME_NAME_SPACE_END

#endif // __ME_DATABLOCK_H_
