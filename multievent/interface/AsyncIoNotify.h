/**
* �첽IO������ - Notify����
* 
*
* History
*	9/04/2016		Created
*/

#ifndef __ASYCH_IO_NOTIFY_H_
#define __ASYCH_IO_NOTIFY_H_

#include "AsyncIoResult.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

/**
* �첽IO������ - Notify
* 
* ����Ͷ��һ������ɡ����źţ��Ա��ڻ���
* ����˵������һ��Wakenerר�õĲ�����
*/
class CMEAsyncIoNotify
{
public:
	/**
	* Ͷ��һ��Notify�����Ի��Ѵ�������״̬��IOCP����������(IOCP Reactor)
	*
	* <pSink>, �¼�������
	* <hInterface>, Ҫ������һ��IOCP����
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�, ����Ϊʧ��
	*/
	ME_Result Post(
		IMEAsyncIoSink* pSink,
		ME_HANDLE hInterface );

public:
	/**
	* �첽IO������Notify��ר�ý����
	* 
	* ��Notify�������ʱ, ��͸�������Ķ������֪ͨ
	*/
	class CMEAsyncIoNotifyResult : public CMEAsyncIoResult
	{
	public:
		CMEAsyncIoNotifyResult(
			IMEAsyncIoSink* pSink,
			ME_HANDLE hInterface );

	protected:
		/**
		* ��Notify IOִ�����֮��ᱻcall
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
	};

	typedef CMEAsyncIoNotifyResult Result;
};

ME_NAME_SPACE_END

#endif	// ME_WIN

#endif	// __ASYCH_IO_NOTIFY_H_














