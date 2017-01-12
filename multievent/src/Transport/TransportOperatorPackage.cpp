/**
* ����TCP������Ĵ�����
* 
*
* History
*	03/29/2016		Created
*/

#include "TransportOperatorPackage.h"
#include "ConnectionInterface.h"
#include "ByteStreamT.h"
#include "TraceFormator.h"
#include "PDU.h"

ME_NAME_SPACE_BEGIN

CMETransportOperatorPackage::CMETransportOperatorPackage()
	: m_bNewPackage( TRUE )
	, m_bStickPackage( FALSE )
	, m_pMB( new CMEMessageBlock(0) )
{
	ME_INFO_TRACE_THIS( SelfInfo() );
}

CMETransportOperatorPackage::~CMETransportOperatorPackage()
{
	ME_INFO_TRACE_THIS( SelfInfo() );
}

ME_Result CMETransportOperatorPackage::PreSendData( CMEMessageBlock*& pmbBlock )
{
	/*
	* �÷�����
	* 1�����û���ǰ��һ��4�ֽ�ͷ�������û�������
	* 2������û�������64k���ƶ�дλ�ã���֤GetLengthС�ڵ���64k
	*/

	ME_ASSERTE_RETURN( pmbBlock, ME_ERROR_NULL_POINTER );
	ME_INFO_TRACE_THIS( SelfInfo() << ", Parameter MB=" << pmbBlock << ", Parameter MB WritePos" << pmbBlock->GetWritePos() );

	ME_Result hResult = ME_OK;

	if ( TRUE == m_bNewPackage )
	{
		pmbBlock->AddReference();

		ME_ASSERTE_RETURN( 0 == pmbBlock->GetReadPos(), ME_ERROR_INVALID_ARG );

		pmbBlock->RemoveReference();

		/* ��ͷ */
		CMEPDUPackage pduPackage( 
			pmbBlock->GetLength(),
			pmbBlock );

        ///< ��Ϊ׷��ʱ���ܹ��޸��û�MB�����Ա�����ͷ��MB��Append�û���MB�����
        ///< ��Ҫͷ����MBҪ���ֵ��������
        ///< Ŀǰͨ�����ü����ķ�ʽ�����Append�����û�MB������4���ֽڵ����⣬Ҳ
        ///< �����ǰ����޸��ó�Ա�����洢���µĲ�֧�ֲ������õ����⣬Ŀǰ����
        ///< ��ʽ��ȱ���ǲ��ܹ���������Operator��Package֮���޸�pmbBlockָ����ָ
        ///< ���MB����Ŀǰ��������û��������Operator�����������������ǰ�ȫ��
        ///< fix by fangyu 2014/07/11
        CMEMessageBlock* pmbHead = new CMEMessageBlock( pduPackage.GetLength() );
		pduPackage.Encode( *pmbHead );

		pmbBlock = pmbHead;

        ///< ���ü���+1����PostSendData����-1������Ҫ��PostSendData�����mb�Ǵ�
        ///< �������pmbBlock
        pmbBlock->AddReference();

		//m_bNewPackage = FALSE;
	}

    return hResult;
}

ME_Result CMETransportOperatorPackage::PostSendData( CMEMessageBlock*& pmbBlock )
{
	ME_ASSERTE_RETURN( pmbBlock, ME_ERROR_NULL_POINTER );

	if ( pmbBlock->GetReadPos() == pmbBlock->GetWritePos() )
	{
		/* һ�������ͳɹ� */
		m_bNewPackage = TRUE;
	}
	else
	{
		ME_INFO_TRACE_THIS( "Send Fail." << SelfInfo() << " Read Pos=" << pmbBlock->GetReadPos() << "Write Pos=" << pmbBlock->GetWritePos() );
	}

    ///< ���ü���-1����Ҫ�����pmbBlock��PreSendData�����pmbBlock��ͬ
    pmbBlock->RemoveReference();

	return RETURN_VALUE_OK;
}

ME_Result CMETransportOperatorPackage::RecvOK()
{
	/* ���� */
	m_bStickPackage = FALSE;

	/* ���ҽ������ܵ�����MB������ȡ��ɺ������ */
	m_pMB->Clear();

	return RETURN_VALUE_OK;
}

ME_Result CMETransportOperatorPackage::RecvSlice()
{
	/* ���ݷ�Ƭ����û�յ�һ�������� */
	m_bStickPackage = FALSE;

	return RETURN_VALUE_FINISH;
}

ME_Result CMETransportOperatorPackage::RecvStick()
{
	/* ճ�� */
	m_bStickPackage = TRUE;

	/* Ҫ��ճʣ�µİ����� */
	UINT nLeftLength = m_pMB->GetLength();

	LPBYTE szStickLeft = new BYTE[nLeftLength];
	ME_Result hResult = m_pMB->Read( szStickLeft, nLeftLength );
	ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

	m_pMB = new CMEMessageBlock( 
		nLeftLength, 
		szStickLeft,
		nLeftLength,
		CMEMessageBlock::MESSAGEBLOCK_ATTR_DELETE_USERDATA );

	return RETURN_VALUE_AGAIN;
}

ME_Result CMETransportOperatorPackage::PreOnReceive( CMEMessageBlock*& pmbBlock )
{
	/*
	* �÷���
	* 1���ܰ�����TransprotBase�յ��İ���
	* 2����PDU Decode���ж��Ƿ���һ�������û��������û�У�˵�������˷�Ƭ����Ҫ�ȴ���һ�����ĵ���
	*	 �����һ���������û����������������һ�����û������յ��İ�һ����
	*    ��һ��������յ��İ����ȴ����û�����˵������ճ��
	* 3���ڶ���������سɹ�����Ҫ���ܵİ��ж�ȡһ���û���
	*/

	ME_ASSERTE_RETURN( pmbBlock, ME_ERROR_NULL_POINTER );
	ME_INFO_TRACE_THIS( SelfInfo() << ", Net MB=" << pmbBlock );

	ME_Result hResult = ME_OK;

	if ( FALSE == m_bStickPackage )
	{
		/* TransportBase�յ���Ϣ���MB����ջ������ģ���������Ҫ�ܰ� ��� */
		CMEMessageBlockAutoPtr pmb( pmbBlock->Clone() );

		/* 
		* �ܰ� 
		* Append�����ö�λ�ã���Ҫ���ݲ���Append������
		*/
		UINT iReadPos = m_pMB->GetReadPos();
		m_pMB->Append( *pmb );

		hResult = m_pMB->SetReadPos( iReadPos );
		ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );
	}

    ///< fix by fangyu 2014/06/25
    ///< ������ԭдλ�ô����bug
    DWORD dwWritePos = m_pMB->GetWritePos();
	CMEPDUPackage pduPackage;
	ME_Result hPDUResult = pduPackage.Decode( *m_pMB );
	if ( ME_SUCCEEDED(hPDUResult) )
	{
		/* 
		* PDU decode�ɹ���m_pMB�Ķ�дλ�ñ���һ��������
		* ������Ҫ���������
		*/
		CMEMessageBlock* pPDUMB = pduPackage.GetData();
		ME_ASSERTE_RETURN( pPDUMB, ME_ERROR_NULL_POINTER );

		UINT iMBLength = pPDUMB->GetLength();
		LPBYTE pBuffer = new BYTE[iMBLength];
		hResult = pPDUMB->Read( pBuffer, iMBLength );
		ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

		pmbBlock = new CMEMessageBlock( 
			iMBLength,
			pBuffer,
			iMBLength, 
			CMEMessageBlock::MESSAGEBLOCK_ATTR_DELETE_USERDATA );

        ///< Operator�Լ�new��MB����Ҫ�Լ����棬�����Ҫ�Լ�������
        m_pmbRecvOut = pmbBlock;

        ///< fix by fangyu 2014/06/25
        ///< ������ԭдλ�ô����bug
        ///< m_pMB��дλ���Ѿ���PDU���ã���������Ҫ��ԭ��дλ��
        ///< �ϸ�˵��������Operator��MB��дλ�ú�Capacity��һ����ͬ����������PDU
        ///< ���ǰ��¼дλ�ã�����PDU�����ԭдλ��
		hResult = pPDUMB->SetWritePos( dwWritePos );
		ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

		if ( m_pMB->GetLength() > 0 )
		{
			/* ճ�� */
			hResult = RecvStick();
		}
		else
		{
			/* ���� */
			hResult = RecvOK();
		}
	}
	else
	{
		/* ��Ƭ */
		pmbBlock = NULL;

		hResult = RecvSlice();
	}

	return hResult;
}

ME_Result CMETransportOperatorPackage::PostOnReceive( CMEMessageBlock*& pmbBlock )
{
//     ///< ��ΪOperator���п��ܻ��޸�pmbBlock��ô����Ͳ����ô����pmbBlock������
// 	ME_INFO_TRACE_THIS( SelfInfo() << "UserMB=" << pmbBlock );
// 
// 	if ( NULL != pmbBlock )
// 	{
// 		/* ���ϲ��MB��PreOnReceive new�����ģ�������Ҫ�ͷ� */
// 		pmbBlock->RemoveReference();
// 	}

    if ( m_pmbRecvOut != NULL )
    {
        m_pmbRecvOut = NULL;
    }

	return RETURN_VALUE_OK;
}

DWORD CMETransportOperatorPackage::GetType()
{
	return IMEConnectionManager::CONNECTION_TYPE_LENGTH;
}

CHAR* CMETransportOperatorPackage::SelfInfo()
{
	static CHAR selfInfo[ME_TRACE_AVAILABLE_DATA_LEN];
	::memset( 
		selfInfo, 
		0, 
		ME_TRACE_AVAILABLE_DATA_LEN );

	CMETraceFormator opFormator( selfInfo, ME_TRACE_AVAILABLE_DATA_LEN );
	opFormator 
		<< "NewPackage" << m_bNewPackage
		<< ", StickPackage=" << m_bStickPackage;

	return selfInfo;
}

ME_NAME_SPACE_END
