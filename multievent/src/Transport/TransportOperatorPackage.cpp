/**
* 用作TCP整理包的处理类
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
	* 该方法：
	* 1，在用户包前加一个4字节头，标明用户包长度
	* 2，如果用户包大于64k，移动写位置，保证GetLength小于等于64k
	*/

	ME_ASSERTE_RETURN( pmbBlock, ME_ERROR_NULL_POINTER );
	ME_INFO_TRACE_THIS( SelfInfo() << ", Parameter MB=" << pmbBlock << ", Parameter MB WritePos" << pmbBlock->GetWritePos() );

	ME_Result hResult = ME_OK;

	if ( TRUE == m_bNewPackage )
	{
		pmbBlock->AddReference();

		ME_ASSERTE_RETURN( 0 == pmbBlock->GetReadPos(), ME_ERROR_INVALID_ARG );

		pmbBlock->RemoveReference();

		/* 加头 */
		CMEPDUPackage pduPackage( 
			pmbBlock->GetLength(),
			pmbBlock );

        ///< 因为追加时不能够修改用户MB，所以必须用头部MB来Append用户的MB，这就
        ///< 需要头部的MB要保持到发送完成
        ///< 目前通过引用计数的方式解决了Append导致用户MB被增加4个字节的问题，也
        ///< 解决了前面的修改用成员变量存储导致的不支持并发调用的问题，目前这种
        ///< 方式的缺点是不能够有其他的Operator在Package之后修改pmbBlock指针所指
        ///< 向的MB，就目前来讲，还没有这样的Operator，所以现在这样做是安全的
        ///< fix by fangyu 2014/07/11
        CMEMessageBlock* pmbHead = new CMEMessageBlock( pduPackage.GetLength() );
		pduPackage.Encode( *pmbHead );

		pmbBlock = pmbHead;

        ///< 引用计数+1，在PostSendData中再-1，但这要求PostSendData输入的mb是此
        ///< 处输出的pmbBlock
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
		/* 一个包发送成功 */
		m_bNewPackage = TRUE;
	}
	else
	{
		ME_INFO_TRACE_THIS( "Send Fail." << SelfInfo() << " Read Pos=" << pmbBlock->GetReadPos() << "Write Pos=" << pmbBlock->GetWritePos() );
	}

    ///< 引用计数-1，需要输入的pmbBlock与PreSendData输出的pmbBlock相同
    pmbBlock->RemoveReference();

	return RETURN_VALUE_OK;
}

ME_Result CMETransportOperatorPackage::RecvOK()
{
	/* 正好 */
	m_bStickPackage = FALSE;

	/* 当且仅当已攒的所有MB都被读取完成后才清理 */
	m_pMB->Clear();

	return RETURN_VALUE_OK;
}

ME_Result CMETransportOperatorPackage::RecvSlice()
{
	/* 数据分片，还没收到一个完整包 */
	m_bStickPackage = FALSE;

	return RETURN_VALUE_FINISH;
}

ME_Result CMETransportOperatorPackage::RecvStick()
{
	/* 粘包 */
	m_bStickPackage = TRUE;

	/* 要将粘剩下的包保存 */
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
	* 该方法
	* 1，攒包（从TransprotBase收到的包）
	* 2，用PDU Decode来判断是否有一个完整用户包，如果没有，说明发生了分片，需要等待下一个包的到来
	*	 如果有一个完整的用户包，有两种情况，一种是用户包和收到的包一样，
	*    另一种情况是收到的包长度大于用户包，说明发生粘包
	* 3，第二步如果返回成功，则要从攒的包中读取一个用户包
	*/

	ME_ASSERTE_RETURN( pmbBlock, ME_ERROR_NULL_POINTER );
	ME_INFO_TRACE_THIS( SelfInfo() << ", Net MB=" << pmbBlock );

	ME_Result hResult = ME_OK;

	if ( FALSE == m_bStickPackage )
	{
		/* TransportBase收到消息后的MB是在栈上申请的，而这里需要攒包 深拷贝 */
		CMEMessageBlockAutoPtr pmb( pmbBlock->Clone() );

		/* 
		* 攒包 
		* Append会重置读位置，需要备份并在Append后重设
		*/
		UINT iReadPos = m_pMB->GetReadPos();
		m_pMB->Append( *pmb );

		hResult = m_pMB->SetReadPos( iReadPos );
		ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );
	}

    ///< fix by fangyu 2014/06/25
    ///< 修正还原写位置错误的bug
    DWORD dwWritePos = m_pMB->GetWritePos();
	CMEPDUPackage pduPackage;
	ME_Result hPDUResult = pduPackage.Decode( *m_pMB );
	if ( ME_SUCCEEDED(hPDUResult) )
	{
		/* 
		* PDU decode成功，m_pMB的读写位置标明一个整包，
		* 这里需要读出这个包
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

        ///< Operator自己new的MB，需要自己保存，最后需要自己来清理
        m_pmbRecvOut = pmbBlock;

        ///< fix by fangyu 2014/06/25
        ///< 修正还原写位置错误的bug
        ///< m_pMB的写位置已经被PDU设置，所以这里要还原下写位置
        ///< 严格说来，进入Operator的MB，写位置和Capacity不一定相同，所以在做PDU
        ///< 解包前记录写位置，做完PDU解包后还原写位置
		hResult = pPDUMB->SetWritePos( dwWritePos );
		ME_ASSERTE_RETURN( ME_SUCCEEDED(hResult), hResult );

		if ( m_pMB->GetLength() > 0 )
		{
			/* 粘包 */
			hResult = RecvStick();
		}
		else
		{
			/* 正好 */
			hResult = RecvOK();
		}
	}
	else
	{
		/* 分片 */
		pmbBlock = NULL;

		hResult = RecvSlice();
	}

	return hResult;
}

ME_Result CMETransportOperatorPackage::PostOnReceive( CMEMessageBlock*& pmbBlock )
{
//     ///< 因为Operator都有可能会修改pmbBlock那么这里就不能用传入的pmbBlock来回收
// 	ME_INFO_TRACE_THIS( SelfInfo() << "UserMB=" << pmbBlock );
// 
// 	if ( NULL != pmbBlock )
// 	{
// 		/* 给上层的MB是PreOnReceive new出来的，这里需要释放 */
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
