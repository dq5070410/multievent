/**
* Transport��wrapper��
* 
* ��ԭ�����transport��reactor�Խӣ��Լ��ѽ������ܴ���
* �ĸ���operator(�磬�����ȵ�TCP����keep-alive��tcp�ȵ�) 
* plug-in��transport����Ȼ��������ΪIMETransport��adapter����
*
*
* History
*	2/22/2016		Created
*   3/22/2016		��operator��ָ���Ϊ����ָ�룬operator�����Ϊѭ������
*/

#ifndef __TRANSPORT_WRAPPER_H_
#define __TRANSPORT_WRAPPER_H_

#include "Base.h"
#include "ConnectionInterface.h"
#include "ConnectionDefines.h"
#include "TransportImpl.h"
#include "TransportOperator.h"
#include "LockThreadMutex.h"

ME_NAME_SPACE_BEGIN

/* ʹ�õ����¼�Ԥ���� */
namespace EVENT
{
	/* �����߳�call DeliveryData() */
	template <class TransportType> class EventUdpDeliveryDataT;
}

/**
* Transport��wrapper��
* 
* ��ԭ�����transport��reactor�Խӣ��Լ��ѽ������ܴ���
* �ĸ���operator(�磬�����ȵ�TCP����keep-alive��tcp�ȵ�) 
* plug-in��transport����Ȼ��������ΪIMETransport��adapter����
*/
class CMETransportWrapper
	: public ITransport
	, protected IMETransportImplSink
	, public CMEMutexTypeTraits<CMEConnDummy>
	, public CMEReferenceControlT<CMEConnDummy::MutexType>
{
public:
	typedef CMETransportWrapper TransportType;

public:
	CMETransportWrapper();

	virtual ~CMETransportWrapper();

public:
	/**
	* ���ݲ�ͬ�����ͳ�ʼ��һ���������õ�acceptor
	*
	* <dwType>, ��ʶ��acceptor��ʲô���͵�, tcp/udp�ȵ�
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����ʧ��
	*/
	ME_Result Initialize( 
		DWORD dwType,
		ME_HANDLE hHandle );

	ME_Result RegisterOperator( IMETransportOperator* pOperator );

	ME_Result UnRegisterOperator( IMETransportOperator* pOperator );

	/* �̳���IMETransport */
public:
	/**
	 * ��һ����������һ���¼�������
	 *
	 * <pSink>, ����󶨵��¼�������
	 *
	 * ����ֵ:
	 *		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result Open( ITransportSink* pSink );

	/**
	* �ر�һ��������
	* һ���������������, ��ô�����������Ҳ���ܱ�ʹ����, ֮ǰ��֮
	* �󶨵Ľ�����Ҳ�����ٽ��յ��κ��¼�, ����㻹��Ҫһ��Transport,
	* ��ô��ֻ��ͨ��Connector����Acceptor��Open()����, Ȼ��ȴ�
	* Sink��OnConnect()�����
	*
	* <hReason>, ��ΪʲôҪ�ر��������
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result Close( ME_Result hReason = ME_OK );

	/**
	* ��������
	*
	* <pmbBlock>, ���͵�����
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result SendData( CMEMessageBlock* pmbBlock );

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

	/**
	* ȡ�õ�ǰ���transport�趨��sink
	*
	* ����ֵ:
	*		��NULL��ʾ�ɹ�; NULL��ʾʧ��
	*/
	virtual ITransportSink* GetSink() const;

	/* �̳���IMEReferenceControl */
public:
	/**
	* �������ü���1
	*
	* ����ֵ:
	*		����֮������ü���, DWORD
	*/
	virtual DWORD AddReference();

	/**
	* �������ü���1
	*
	* ����ֵ:
	*		�ݼ�֮������ü���, DWORD
	*/
	virtual DWORD RemoveReference();

public:
	/**
	* ������UDP���������������ᱻcall
	*/
	void OnReceive_c( 
		const char* pszData,
		UINT uLength );

	/* �̳���IMETranpsortImplSink */
protected:
	/**
	* �յ�����
	*
	* <pTransport>���ĸ�transport�յ�����
	* <pmbData>���յ�������
	*
	* ����ֵ��
	*		��
	*/
	virtual void OnReceive( 
		IMETransportImpl* pTransport,
		CMEMessageBlock* pmbData );

	/**
	* ���ӶϿ�
	*
	* <pTransport>���ĸ�transport�Ͽ�
	* <hReason>���Ͽ���ԭ����ʲô
	*
	* ����ֵ��
	*		��
	*/
	virtual void OnDisconnect( 
		IMETransportImpl* pTransport,
		ME_Result hReason );

	/**
	* �յ�����
	*
	* <pTransport>���ĸ�transport�ܹ����·���������
	* <hResult>��ԭ����ʲô
	*
	* ����ֵ��
	*		��
	*/
	virtual void OnCanSendAgain(
		IMETransportImpl* pTransport,
		ME_Result hResult = ME_OK );

private:
	/**
	* ������UDP���������������ᱻcall
	* 
	* ���ϲ�û�е���Open()��֮ǰ�Ѿ��յ����������������Żᱻ����
	* ���ң������ϲ����յ�����Open()֮�����Żᷢ��
	*/
	void DeliveryData();

	ME_Result OperatorPreSend( 
		CMEMessageBlock*& pmbBlock );

	ME_Result OperatorPostSend( 
		CMEMessageBlock*& pmbBlock );

	ME_Result OperatorPreRecv( 
		CMEMessageBlock*& pmbBlock );

	ME_Result OperatorPostRecv( 
		CMEMessageBlock*& pmbBlock );

private:
	ME_Result CreateSync( 
		DWORD dwType,
		ME_HANDLE hHandle );

	ME_Result CreateAsync(
		DWORD dwType,
		ME_HANDLE hHandle );

private:
	DWORD									m_dwType;		// ԭ��transport������
	CMEAutoPtrT<IMETransportImpl>			m_pTransport;   // ԭ��transport

	ITransportSink*						m_pSink;		// �ϲ�ص�����һ����transport cross thread�����ϲ�ʹ����

// 	typedef std::map<DWORD, IMETransportOperatorAutoPtr> MapType;
// 	MapType									m_mapOperator;	// �������洢�ṹ

    typedef std::deque<IMETransportOperatorAutoPtr> OperatorDequeType;
    OperatorDequeType                       m_dequeOperator;

	/**
	*
	* ����ȷ���Ƿ�close�Ա�����OnReceive() callback�п��ܴ��ڵ� 
	* ʹ����ɾ��transport����Ϊ
	*/
	CMEAtomicOperationT<CMELockMutex>		m_bClosed;

private:
	typedef CMEMessageBlock PayloadType;

private:
	typedef CMEAutoPtrT<PayloadType> ListItemType;
	typedef std::list<ListItemType> DataBufferType;

	/**
	* ���ҽ������������ʹ��
	*
	* udp������transport��������֮���ϲ�û��open
	*
	* ����������£����еİ����ᱻ��ŵ�<m_UdpDataBuf>������Open֮��
	* ����İ��ᱻһ������callback��ȥ
	*/
	DataBufferType							m_UdpDataBuf;
	CMELockThreadMutex						m_DataBufLock;

	/* �¼���Ԫ������� */
private:
	friend class EVENT::EventUdpDeliveryDataT<TransportType>;

	/* ��Ԫ�¼����ٶ��� */
private:
	typedef EVENT::EventUdpDeliveryDataT<TransportType> DeliveryDataEvent;
};

ME_NAME_SPACE_END

#endif	// __TRANSPORT_WRAPPER_H_






