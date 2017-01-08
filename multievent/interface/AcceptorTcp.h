/**
* TCP��������ԭ����
* 
*
* History
*	12/12/2016		Created
*/

#ifndef __ACCEPTOR_TCP_H_
#define __ACCEPTOR_TCP_H_

#include "Base.h"
#include "AcceptorImpl.h"
#include "IO.h"

ME_NAME_SPACE_BEGIN

class CMESocketTcp;
class CMEReactor;

/**
* TCP��������ԭ����
*
* ������Ҫ�����ǰ�socket��reactor��ϵ����
* 
* �������̰߳�ȫ��
*/
class  ME_OS_EXPORT CMEAcceptorTcp 
	: public IMEIOSink
	, public IMEIO
	, public IMEAcceptorImpl
{
public:
	typedef CMEAcceptorTcp AcceptorType;
	typedef IMEAcceptorImplSink AcceptorSinkType;

public:
	CMEAcceptorTcp();

	virtual ~CMEAcceptorTcp();

public:
	virtual void SetHandle( ME_HANDLE hHandle );

	virtual ME_HANDLE GetHandle() const;

public:
	/**
	* ��, �������뱾��ĳ���˿�
	*
	* <pSink>, �¼�������
	* <pReactor>, �¼�����������
	* <hiaLocalAddress>, ��Ҫ�󶨵ı��ص�ַ
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�, ����Ϊʧ��
	*/
	virtual ME_Result Open( 
		IMEAcceptorImplSink* pSink,
		CMEReactor* pReactor,
		const CMEInetAddress& hiaLocalAddress );

	/**
	* �ر�, ��ֹͣ����
	*
	* ����ֵ:
	*		��
	*/
	virtual void Close( CMEReactor* pReactor );

	/* �̳���IMEIO */
protected:
	/**
	* input�¼�����
	*
	* <hHandle>���ĸ����������input�¼�
	* <iEvent>��������ʲô�����¼�
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnInput(
		ME_HANDLE hHandle,
		int iEvent );

	/**
	* output�¼�����
	*
	* <hHandle>���ĸ����������output�¼�
	* <iEvent>��������ʲô�����¼�
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnOutput(
		ME_HANDLE hHandle,
		int iEvent );

	/**
	* close�¼�����
	*
	* <hHandle>���ĸ����������close�¼�
	* <iEvent>��������ʲô�����¼�
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnClose(
		ME_HANDLE hHandle,
		int iEvent );

protected:
	AcceptorSinkType*				m_pSink;		// �ص�����
	int								m_iIndex;		// ��ʾacceptor��Ϊio������reactor�������е�λ��
	CMESocketTcp*					m_pSocketTcp;	// TCP socket
};

ME_NAME_SPACE_END

#endif	// __ME_ACCEPTOR_TCP_H_







