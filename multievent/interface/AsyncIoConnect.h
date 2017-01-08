/**
* �첽IO������ - Connect����
* 
*
* History
*	9/04/2016		Created
*/
#ifndef __ASYCH_IO_CONNECT_H_
#define __ASYCH_IO_CONNECT_H_

#include "Base.h"
#include "AsyncIoResult.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

class CMEInetAddress;

/**
* �첽IO������ - Connect
* 
* ���ڳ�����ĳ����ַ��������
*/
class ME_OS_EXPORT CMEAsyncIoConnect
{
public:
	/**
	* Ͷ��һ��Connect���󣬳�����ĳ����ַ��������
	*
	* <pSink>, �¼�������
	* <hConnectHandle>, ʹ����һ�������������
	* <pPeerAddress>, �Զ˵�ַ
	* <bIPv4>, ��ַ����, ��IPv4����IPv6
	* <bAutoBind>, �Ƿ��Զ��󶨱��ص�ַ, �������ΪFALSE, ��ô����Ҫȷ��<hConnectHandle>�Ѿ���
	*			   �󶨵�ĳ�����ص�ַ��, ����, ���ӻ�ʧ��
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�, ����Ϊʧ��
	*/
	ME_Result Post(
		IMEAsyncIoSink* pSink,
		ME_HANDLE hConnectHandle,
		const CMEInetAddress* pPeerAddress,
		BOOL bIPv4 = TRUE,
		BOOL bAutoBind = TRUE );

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
	* �첽IO������Connect��ר�ý����
	* 
	* ��Connect�������ʱ, ��͸�������Ķ������֪ͨ
	*/
	class CMEAsyncIoConnectResult : public CMEAsyncIoResult
	{
	public:
		CMEAsyncIoConnectResult(
			IMEAsyncIoSink* pSink,
			ME_HANDLE hConnectHandle );

	protected:
		/**
		* ��Connect IOִ�����֮��ᱻcall
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

	typedef CMEAsyncIoConnectResult Result;
};

ME_NAME_SPACE_END

#endif	// ME_WINs

#endif	// __ASYCH_IO_CONNECT_H_

















