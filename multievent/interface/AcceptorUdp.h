/**
* UDP��������ԭ����
* 
*
* History
*	3/12/2016		Created
*/

#ifndef __ME_ACCEPTOR_UDP_H_
#define __ME_ACCEPTOR_UDP_H_

#include "Base.h"
#include "AcceptorImpl.h"
#include "IO.h"

ME_NAME_SPACE_BEGIN

class CMESocketUdp;
class CMEReactor;

/**
* UD��������ԭ����
*
* ������Ҫ�����Ұ�socket��reactor��ϵ����
* 
* �������̰߳�ȫ��
*/
class CMEAcceptorUdp 
	: public IMEIO
	, public IMEIOSink
	, public IMEAcceptorImpl
{
public:
	typedef CMEAcceptorUdp AcceptorType;
	typedef IMEAcceptorUdpImplSink AcceptorSinkType;

public:
	CMEAcceptorUdp();

	virtual ~CMEAcceptorUdp();

	/* �̳���IMEIO */
public:
	virtual void SetHandle( ME_HANDLE hHandle );

	virtual ME_HANDLE GetHandle() const;

	/* �̳���IMEAcceptorImpl */
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

	/* �̳���IMEIOSink */
protected:
	virtual void OnInput(
		ME_HANDLE hHandle,
		int iEvent );

	virtual void OnOutput(
		ME_HANDLE hHandle,
		int iEvent );

	virtual void OnClose(
		ME_HANDLE hHandle,
		int iEvent );

protected:
	AcceptorSinkType*				m_pSink;		// �ص�����
	int								m_iIndex;		// ��ʾacceptor��Ϊio������reactor�������е�λ��
	CMESocketUdp*					m_pSocketUdp;	// UDP socket
};


ME_NAME_SPACE_END


#endif	// __ACCEPTOR_UDP_H_




