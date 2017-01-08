/**
* �첽IO������ - Accept����
* 
*
* History
*	9/04/2016		Created
*/

#ifndef __ASYCH_IO_ACCEPT_H_
#define __ASYCH_IO_ACCEPT_H_

#include "Base.h"
#include "AsyncIoResult.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

/**
* �첽IO������ - Accept
* 
* ���ڵȴ�����ȡһ��������
*/
class ME_OS_EXPORT CMEAsyncIoAccept
{
public:
	/**
	* Ͷ��һ��Accept����Ϊ��һ�����ӵ�����׼��
	*
	* <pSink>, �¼�������
	* <hListenHandle>, �Ѿ���������״̬��socket���
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�, ����Ϊʧ��
	*/
	ME_Result Post(
		IMEAsyncIoSink* pSink,
		ME_HANDLE hListenHandle );

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
	* �첽IO������Accept��ר�ý����
	* 
	* ��Accept�������ʱ, ��͸�������Ķ������֪ͨ
	*/
	class CMEAsyncIoAcceptResult : public CMEAsyncIoResult
	{
	public:
		CMEAsyncIoAcceptResult( 
			IMEAsyncIoSink* pSink,
			DWORD dwLocalAddrLen,
			DWORD dwRemoteAddrLen );

	protected:
		/**
		* ��Accept IOִ�����֮��ᱻcall
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
		static char					s_cBuf;				// �������::AcceptEx��<lpOutputBuffer>�������ò�������ΪNULL
		DWORD						m_dwLocalAddrLen;	// ����::AcceptEx��<dwLocalAddressLength>����
		DWORD						m_dwRemoteAddrLen;	// ����::AcceptEx��<dwRemoteAddressLength>����
        BOOL                        m_bResult;          // ����OnComplete�Ľ�� 01/26/2015 by fangyu
	};

	typedef CMEAsyncIoAcceptResult Result;
};

ME_NAME_SPACE_END

#endif	// ME_WIN

#endif	// __ASYCH_IO_ACCEPT_H_











