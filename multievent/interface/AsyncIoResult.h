/**
* �첽IO��������Ķ���
* ���������첽IO��������Ļ���
* 
*
* History
*	9/04/2016		Created
*/
#ifndef __ASYCH_IO_RESULT_H_
#define __ASYCH_IO_RESULT_H_

#include "Base.h"
#include "AsyncIoDefines.h"
#include "AutoPtrT.h"
#include "IO.h"

ME_NAME_SPACE_BEGIN

/**
* �����첽IO��������Ļ���
*
* ��ֻ��һ���ص�����OnComplete()�����������ָ����IOִ�����֮��ᱻcall��
* ��OnComplete()��������ᰴ˳�����OnCompleteImpl()��OnDestroy()��ǰ���Ǵ���
* �̳�����붨�壬���ߵ�Ĭ�϶�����delete this������̳�����������Ҫ������
* ����ʵ��OnDestroy()
*/
class ME_OS_EXPORT CMEAsyncIoResult : public ME_ASYCH_IO_INFO 
{
public:
	/**
	* ��ָ����IOִ�����֮��ᱻcall
	*
	* <bResult>, �������
	* <iActiveEvent>, ʲô�¼�������
	* <hInterface>, ˭�����IO
	*
	* ����ֵ:
	*		��
	*/
	void OnComplete(
		BOOL bResult,
		int iActiveEvent,
		ME_HANDLE hInterface );

protected:
	CMEAsyncIoResult(
		int iIoType,
		IMEAsyncIoSink* pSink,
		ME_HANDLE hHandle = NULL );

	virtual ~CMEAsyncIoResult();

protected:
	/**
	* ��ָ����IOִ�����֮��ᱻcall
	* �̳�����Ҫʵ������ӿ�
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
		ME_HANDLE hInterface ) = 0;

	/**
	* ��ָ����OnCompleteImpl()ִ�����֮��ᱻcall��
	* Ĭ�϶�����delete this������̳�����������Ҫ������
	* ����ʵ��OnDestroy()
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnDestroy();

protected:
	CMEAutoPtrT<IMEAsyncIoSink>	m_pAioSink;	// �ص�����
};

ME_NAME_SPACE_END

#endif	// __ASYCH_IO_RESULT_H_



