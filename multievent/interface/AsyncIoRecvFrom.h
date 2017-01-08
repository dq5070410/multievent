/**
* �첽IO������ - RecvFrom����(����UDP)
* 
*
* History
*	9/04/2016		Created
*/
#ifndef __ASYCH_IO_RECVFROM_H_
#define __ASYCH_IO_RECVFROM_H_

#include "Base.h"
#include "AsyncIoResult.h"
#include "InetAddress.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

class CMEMessageBlock;
class CMEInetAddress;

/**
* �첽IO������ - RecvFrom
* 
* ���ڵȴ�����ȡһ��������
*/
class ME_OS_EXPORT CMEAsyncIoRecvFrom
{
public:
	/**
	* Ͷ��һ�����ݶ�ȡ������Ϊ��һ�����������ݰ�(UDP)��׼��
	*
	* <pSink>, �¼�������
	* <hRecvHandle>, ����һ��socket�����ȡ
	* <pmbBlock>, Ԥ��������ݿռ�(������new������)
	* <pFromAddress>, ������Դ�ĶԶ˵�ַ
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�, ����Ϊʧ��
	*/
	ME_Result Post(
		IMEAsyncIoSink* pSink,
		ME_HANDLE hRecvHandle,
		CMEMessageBlock* pmbBlock,
		CMEInetAddress* pFromAddress );

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
	* �첽IO������RecvFrom��ר�ý����
	* 
	* ��RecvFrom�������ʱ, ��͸�������Ķ������֪ͨ
	*/
	class CMEAsyncIoRecvFromResult : public CMEAsyncIoResult
	{
	public:
		CMEAsyncIoRecvFromResult(
			int iIoType,
			IMEAsyncIoSink* pSink,
			ME_HANDLE hRecvHandle,
			CMEMessageBlock* pmbBlock,
			CMEInetAddress* pFromAddress );

		virtual ~CMEAsyncIoRecvFromResult();

	protected:
		/**
		* ��RecvFrom IOִ�����֮��ᱻcall
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
		CMEAutoPtrT<CMEMessageBlock>	m_pmbBlock;				// ��ȡ�������ݿ�
		CMEInetAddress*				m_pFromAddress;			// �Զ˵�ַ
		int								m_iFromAddressSize;		// ��ַ���ȣ�Ҳ����IPv4��IPv6��ַ�ṹ�ĳ��ȣ�ȡ���ڶԶ�����ʲô���͵ĵ�ַ
	};

	typedef CMEAsyncIoRecvFromResult Result;
};

ME_NAME_SPACE_END

#endif	// ME_WIN

#endif	// __ASYCH_IO_RECVFROM_H_




