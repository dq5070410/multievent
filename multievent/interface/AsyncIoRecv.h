/**
* �첽IO������ - Recv����
* 
*
* History
*	9/04/2016		Created
*/
#ifndef __ASYCH_IO_RECV_H_
#define __ASYCH_IO_RECV_H_

#include "Base.h"
#include "AsyncIoResult.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

class CMEMessageBlock;

/**
* �첽IO������ - Recv
* 
* ���ڶ�ȡһ���Ѿ���Ready�������ݿ�
*/
class ME_OS_EXPORT CMEAsyncIoRecv
{
public:
	/**
	* Ͷ��һ��Recv����Ϊ��һ�����������ݿ���׼��
	*
	* <pSink>, �¼�������
	* <hRecvHandle>, ׼������һ��socket�����ȡ
	* <pmbBlock>, Ԥ��׼���õ����ݴ洢�ռ�(�������Ǳ�new������)
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�, ����Ϊʧ��
	*/
	ME_Result Post(
		IMEAsyncIoSink* pSink,
		ME_HANDLE hRecvHandle,
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
	* �첽IO������Recv��ר�ý����
	* 
	* ��Recv�������ʱ, ��͸�������Ķ������֪ͨ
	*/
	class CMEAsyncIoRecvResult : public CMEAsyncIoResult
	{
	public:
		CMEAsyncIoRecvResult(
			IMEAsyncIoSink* pSink,
			ME_HANDLE hRecvHandle,
			CMEMessageBlock* pmbBlock );
	
		virtual ~CMEAsyncIoRecvResult();

	protected:
		/**
		* ��Recv IOִ�����֮��ᱻcall
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
		CMEAutoPtrT<CMEMessageBlock>	m_pmbBlock;	// ��ȡ�������ݿ�
	};

	typedef CMEAsyncIoRecvResult Result;
};

ME_NAME_SPACE_END

#endif	// ME_WIN

#endif	// __ASYCH_IO_RECV_H_


