/**
* Transport��TCPʵ����
* 
*
* History
*	12/18/2016		Created
*/

#ifndef __TRANSPORT_TCP_H_
#define __TRANSPORT_TCP_H_

#include "Base.h"
#include "TransportBase.h"

ME_NAME_SPACE_BEGIN

/* ʹ�õ��¼�Ԥ���� */
namespace EVENT
{
	/* �ڵ�ǰ���߳���callback OnDisconnect */
	template <class TransportType, class TransportSinkType> class EventOnDisconnectCurrentT;
}

/**
* Transport��TCPʵ����
*/
class ME_OS_EXPORT CMETransportTcp : public CTransportBase
{
public:
	typedef CMETransportTcp TransportType;

public:
	CMETransportTcp( ME_HANDLE hHandle );

	virtual ~CMETransportTcp();

public:
	/**
	* ��������
	*
	* <pReactor>, ʹ���ĸ�reactor
	* <pmbBlock>, ���͵�����
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result SendData( 
		CMEReactor* pReactor,
		CMEMessageBlock* pmbBlock );

	/**
	* ����transport������
	*
	* <dwOptionType>, Ҫ����ʲô����
	* <pOptionValue>, ��Ӧ��<dwOptionType>��ֵ
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result SetOption(
		DWORD dwOptionType,
		void* pOptionValue );

	/**
	* ȡ��transport������
	*
	* <dwOptionType>, Ҫȡ��ʲô����
	* <pOptionValue>, out, ��Ӧ��<dwOptionType>��ֵ
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result GetOption(
		DWORD dwOptionType,
		void*& pOptionValue );

protected:
	/**
	* ���tcp/ipջ�п��ܴ��ڵĻ���(��û�����ü����͵�����)
	*
	* <dwOptionType>, Ҫȡ��ʲô����
	* <pOptionValue>, out, ��Ӧ��<dwOptionType>��ֵ
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual DWORD CheckUnReadDataImpl();

	/**
	* ��socket��ȡ����
	*
	* <pszData>, ����ȡ������
	* <dwDataSize>, <pszData>�ĳ���
	*
	* ����ֵ:
	*		ʵ�ʶ�ȡ�����ݳ���; -1Ϊ��ȡʧ��
	*/
	virtual int ReceiveImpl( 
		char*& pszData, 
		DWORD dwDataSize );

public:
	void OnDisconnect_c( 
		TransportSinkType* pSink,
		ME_Result hReason );

	/* �¼���Ԫ������� */
protected:
	friend class EVENT::EventOnDisconnectCurrentT<TransportType, TransportSinkType>;

	/* ��Ԫ�¼����ٶ��� */
protected:
	typedef class EVENT::EventOnDisconnectCurrentT<TransportType, TransportSinkType> OnDisconnectUserEvent;
};

ME_NAME_SPACE_END

#endif	// __TRANSPORT_TCP_H_



