/**
* �첽IO������ - SendTo����(UDP)
* 
*
* History
*	9/04/2016		Created
*/

#ifndef __ASYCH_IO_SENDTO_H_
#define __ASYCH_IO_SENDTO_H_

#include "Base.h"
#include "AsyncIoResult.h"
#include "LockThreadMutex.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

class CMEMessageBlock;
class CMEInetAddress;

/**
* �첽IO������ - SendTo(UDP)
* 
* ���ض��ĵ�ַ��������
*/
class ME_OS_EXPORT CMEAsyncIoSendTo
{
public:
	/**
	* Ͷ��һ��Send����׼����������
	*
	* <pSink>, �¼�������
	* <hSendHandle>, ʹ����һ��socket����������ݷ���
	* <pmbBlock>, ׼�����͵�����
	* <pToAddress>, ��ʲô��ַ����
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�, ����Ϊʧ��
	*/
	ME_Result Post(
		IMEAsyncIoSink* pSink,
		ME_HANDLE hSendHandle,
		CMEMessageBlock* pmbBlock,
		CMEInetAddress* pToAddress );

	/**
	* Cancelĳ��������ϵ������ڶ��е��е�����
	*
	* <hHandle>, ��Ҫcancel��socket���
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�, ����Ϊʧ��
	*/
	void Cancel( ME_HANDLE hHandle );

public:
	class CMEAsyncIoSendToResult : public CMEAsyncIoResult
	{
	public:
		CMEAsyncIoSendToResult(
			IMEAsyncIoSink* pSink,
			ME_HANDLE hRecvHandle,
			CMEMessageBlock* pmbBlock,
			CMEInetAddress* pToAddress );

		virtual ~CMEAsyncIoSendToResult();

	protected:
		/**
		* ��SendTo IOִ�����֮��ᱻcall
		*
		* <bResult>, �������
		* <iActiveEvent>, ʲô�¼�������
		* <hInterface>, ˭�����IO
		*
		* ����ֵ:
		*		��
		*/
		virtual void OnCompleteImpl( 
			BOOL bResult,
			int iActiveEvent,
			ME_HANDLE hInterface );

	public:
		CMEAutoPtrT<CMEMessageBlock>	m_pmbBlock;		// ��Ҫ���͵����ݿ�
		CMEInetAddress*				m_pToAddress;	// ��ʲô�ط�����

		CMELockThreadMutex				m_Lock;			// ���ڱ�����������������
		int								m_iLastResult;	// ���������������WSASend�����ķ���ֵ
		DWORD							m_dwLastError;	// ����������ϵͳ������ʾ����997(WSA_IO_PENDING)�ȵ�
	};

	typedef CMEAsyncIoSendToResult Result;
};

ME_NAME_SPACE_END

#endif	// ME_WIN

#endif	// __ASYCH_IO_SENDTO_H_






























