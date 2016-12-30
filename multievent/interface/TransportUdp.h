/**
* Transport��UDPʵ����
* 
*
* History
*	12/18/2016		Created
*/

#ifndef __TRANSPORT_UDP_H_
#define __TRANSPORT_UDP_H_

#include "Base.h"
#include "TransportBase.h"
#include "InetAddress.h"

ME_NAME_SPACE_BEGIN

/**
* Transport��TCPʵ����
*/
class CMETransportUdp : public CMETransportBase
{
public:
	CMETransportUdp( ME_HANDLE hHandle );

	virtual ~CMETransportUdp();

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

protected:
	CMEInetAddress					m_PeerAddress;	// �Զ˵�ַ
};

ME_NAME_SPACE_END

#endif	// __TRANSPORT_UDP_H_