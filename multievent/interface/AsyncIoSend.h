/**
* �첽IO������ - Send����
* 
*
* History
*	9/04/2016		Created
*/
#ifndef __ASYCH_IO_SEND_H_
#define __ASYCH_IO_SEND_H_

#include "Base.h"
#include "AsyncIoResult.h"
#include "LockThreadMutex.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

class CMEMessageBlock;

/**
* �첽IO������ - Send
* 
* ʹ���ض������������
*/
class ME_OS_EXPORT CMEAsyncIoSend
{
public:
	/**
	* Ͷ��һ��Send����׼����������
	*
	* <pSink>, �¼�������
	* <hSendHandle>, ʹ����һ��socket����������ݷ���
	* <pmbBlock>, ׼�����͵�����
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�, ����Ϊʧ��
	*/
	ME_Result Post(
		IMEAsyncIoSink* pSink,
		ME_HANDLE hSendHandle,
		CMEMessageBlock* pmbBlock );

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
	/**
	* �첽IO������Send��ר�ý����
	* 
	* ��Send�������ʱ, ��͸�������Ķ������֪ͨ
	*/
	class CMEAsyncIoSendResult : public CMEAsyncIoResult
	{
	public:
		CMEAsyncIoSendResult(
			IMEAsyncIoSink* pSink,
			ME_HANDLE hRecvHandle,
			CMEMessageBlock* pmbBlock );

		virtual ~CMEAsyncIoSendResult();

	protected:
		/**
		* ��Send IOִ�����֮��ᱻcall
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

		CMELockThreadMutex				m_Lock;			// ���ڱ������������������
		int								m_iLastResult;	// ���������������WSASend�����ķ���ֵ
		DWORD							m_dwLastError;	// ����������ϵͳ������ʾ����997(WSA_IO_PENDING)�ȵ�
	};

	typedef CMEAsyncIoSendResult Result;
};

ME_NAME_SPACE_END

#endif	// ME_WIN

#endif	// __ASYCH_IO_SEND_H_










