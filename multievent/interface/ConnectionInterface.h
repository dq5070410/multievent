/**
* �������ӿ�ܽӿ�
* 
*
* History
*	12/13/2016		Created
*/

#ifndef __CONNECTION_INTERFACE_H_
#define __CONNECTION_INTERFACE_H_

#include "Base.h"
#include "AutoPtrT.h"
#include "ReferenceControlT.h"

ME_NAME_SPACE_BEGIN

/* Ԥ���� */
class CMEMessageBlock;
class CTimeValue;
class IAcceptor;
class IAcceptorSink;
class IConnector;
class IConnectorSink;
class ITransport;
class ITransportSink;
class CMEInetAddress;

typedef CMEAutoPtrT<IAcceptor> CAcceptorAutoPtr;
typedef CMEAutoPtrT<IConnector> CConnectorAutoPtr;
typedef CMEAutoPtrT<ITransport> CTransportAutoPtr;

class ME_OS_EXPORT IConnectionManager
{
public:
	virtual ~IConnectionManager() {}

public:
	/* ���ӵ������Լ����� */
	enum CONNECTION_TYPE
	{
		CONNECTION_TYPE_NONE = 0,

		/* ��TCP */
		CONNECTION_TYPE_TCP = (1 << 0),

		/* ��UDP */
		CONNECTION_TYPE_UDP = (1 << 1),

		/* ʹ���Ӵ���������������, ��ÿһ������ȥ�İ�����������, ֧��TCP */
		CONNECTION_TYPE_LENGTH = (1 << 16),

		/* �Զ����������Ƿ��ǿ��õ�, ֧��TCP/UDP */
		CONNECTION_TYPE_KEEP_ALIVE = (1 << 17),

        /* SSL, ֧��TCP */
        CONNECTION_TYPE_SSL = (1 << 18),
	};

public:
	static IConnectionManager* Instance();

public:

	/**
	* ����һ���ͻ���
	* 
	* <dwType>, ʲôЭ��Ŀͻ���, ���ͱ����ME_CONNECTION_TYPE�л�ķ�ʽȡֵ
	* <pConnector>, out, ���������������ָ�����
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��
	*/
	virtual ME_Result CreateClient(
		DWORD dwType,
		CConnectorAutoPtr& pConnector ) = 0;

	/**
	* ����һ����������
	* 
	* <dwType>, ʲôЭ��ķ�������, ���ͱ����ME_CONNECTION_TYPE�л�ķ�ʽȡֵ
	* <pAcceptor>, out, ���������������ָ�����
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��
	*/
	virtual ME_Result CreateServer(
		DWORD dwType,
		CAcceptorAutoPtr& pAcceptor ) = 0;
};

/**
 * ���������Ľӿ���
 * ����ʱ, ���û��ָ���󶨵ĵ�ַ, ���һ������ж�������Ļ�,
 * ��ô���ս�Ĭ��ѡ���һ�������ĵ�һ����ַ��Ϊ������ַ
*/
class ME_OS_EXPORT IAcceptor : public IMEReferenceControl
{
public:
	virtual ~IAcceptor() {}

public:

	/**
	* ��һ��Acceptor��������<hiaLocalAddr>
	* �����µ����ӽ���ʱ, ��ͨ��<pSink>��OnConnect()��֪ͨ
	*
	* <pSink>, �¼�������
	* <pTransport>, һ���²����������� -- ��Ӧһ������
	*
	* ����ֵ:
	*		��
	*/
	virtual ME_Result Open( 
		IAcceptorSink* pSink,
		const CMEInetAddress& hiaLocalAddr ) = 0;

	/**
	* �ر�һ��Acceptor����ֹͣ����
	* ���������������֮��, ��ǰ�󶨵Ľ������������յ��κ��¼�
	*
	* <hReason>, �رյ�ԭ����ʲô
	*
	* ����ֵ:
	*		��
	*/
	virtual void Close( ME_Result hReason = ME_OK ) = 0;

	/**
	* ����acceptor������
	*
	* <dwOptionType>, Ҫ����ʲô����
	* <pOptionValue>, ��Ӧ��<dwOptionType>��ֵ
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
    virtual ME_Result SetOption(
        DWORD dwOptionType,
        void* pOptionValue ) = 0;

	/**
	* ȡ��acceptor������
	*
	* <dwOptionType>, Ҫȡ��ʲô����
	* <pOptionValue>, out, ��Ӧ��<dwOptionType>��ֵ
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
    virtual ME_Result GetOption(
        WORD dwOptionType,
        void*& pOptionValue ) = 0;
};

/**
 * IMEAcceptor���¼�������
 * ��������յ���IMEAcceptor�������¼�, ����, ��������������, �ȵ�
 * �ͱ���̳������, ��ʵ����غ���
*/
class ME_OS_EXPORT IAcceptorSink
{
public:
	virtual ~IAcceptorSink() {}

public:

	/**
	* �����յ�����ص���ʱ��, ˵���������Ѿ�������
	* ����<pTransport>, ��ʹ��֮ǰ, �������<pTransport>��Open()
	* ������ָ��һ���¼�������, ֮��������<pTransport>������
	* �¼�, ���������������֪ͨ����
	*
	* <pTransport>, һ���²����������� -- ��Ӧһ������
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnConnect( CTransportAutoPtr& pTransportAutoPtr ) = 0;
};

/**
* �������ӵĽӿ���
* ����ʱ, ���û��ָ���󶨵ĵ�ַ, ���һ������ж�������Ļ�,
* ��ô���ս�Ĭ��ѡ���һ�������ĵ�һ����ַ��Ϊ������ַ
*/
class ME_OS_EXPORT IConnector : public IMEReferenceControl
{
public:
	virtual ~IConnector() {}

public:
	/**
	* ��һ��Connector���ҳ������ӵ�<peerAddress>
	* ��������ӽ���ʱ, ��ͨ��<pSink>��OnConnect()��֪ͨ
	*
	* <pSink>, �¼�������
	* <pTransport>, һ���²����������� -- ��Ӧһ������
	* <pTimeout>, �������ӹ��೤ʱ��û��Ӧ�㳬ʱ
	* <pLocalAddress>, ��Ҫ�󶨵ı��ص�ַ
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ��ME_OKΪʧ��
	*/
	virtual ME_Result Open(
		IConnectorSink* pSink,
		const CMEInetAddress& peerAddress,
		CTimeValue* pTimeout = NULL,
		CMEInetAddress* pLocalAddress = NULL ) = 0;

	
	/**
	* �ر�һ��Connector����ֹͣ����
	* ���������������֮��, ��ǰ�󶨵Ľ������������յ��κ��¼�
	*
	* ����ֵ:
	*		��
	*/
	virtual void Close() = 0;

	/**
	* ����connector������
	*
	* <dwOptionType>, Ҫ����ʲô����
	* <pOptionValue>, ��Ӧ��<dwOptionType>��ֵ
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
    virtual ME_Result SetOption(
        DWORD dwOptionType,
        void* pOptionValue ) = 0;

	/**
	* ȡ��connector������
	*
	* <dwOptionType>, Ҫȡ��ʲô����
	* <pOptionValue>, out, ��Ӧ��<dwOptionType>��ֵ
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
    virtual ME_Result GetOption(
        WORD dwOptionType,
        void*& pOptionValue ) = 0;
};

/**
* IConnector���¼�������
* ��������յ���IConnector�������¼�, ����, ��������������, �ȵ�
* �ͱ���̳������, ��ʵ����غ���
*/
class ME_OS_EXPORT IConnectorSink
{
public:
	virtual ~IConnectorSink() {}

public:
	/**
	 * �����յ�����ص���ʱ��, ˵���������Ѿ�������
	 * ����<pTransport>, ��ʹ��<pTransport>֮ǰ, �������<pTransport>��Open()
	 * ������ָ��һ���¼�������, ֮��������<pTransport>�������¼�, ���������
	 * ������֪ͨ����; ���û��ָ��������, �㽫�ղ����κ����ԶԶ˵���Ϣ
	 *
	 * <pTransport>, һ�����������ݷ��͵�������, �Ƿ�ΪNULLȡ����<hResult>
	 * <hResult>, ���ӳɹ����, ME_OKΪ�ɹ�, ����Ϊʧ��ԭ��; ��ME_OKʱ, <pTransport>��ΪNULL, ������ΪNULL
	 *
	 * ����ֵ:
	 *		��
	*/
	virtual void OnConnect( 
		CTransportAutoPtr& pTransportAutoPtr,
		ME_Result hResult ) = 0;
};

/**
* ����ӿ���
*/
class ME_OS_EXPORT ITransport : public IMEReferenceControl
{
public:
	enum OPTION_TYPE
	{
		/* ��Сֵ, ���ڱ߽��� */
		OPTION_TYPE_NONE = 100,

		/* ��ȡ�Ѿ��󶨵ı��ص�ַ */
		OPTION_TYPE_LOCAL_ADDRESS,

		/* ��ȡԶ�˵ĵ�ַ */
		OPTION_TYPE_PEER_ADDRESS,

		/* ��֪��ǰ���ڸ����ӵ�socket�Ƿ񻹿��� */
		OPTION_TYPE_SOCKET_STILL_ALIVE,

		/* ��ȡ��ǰ���ӵ����� */
		OPTION_TYPE_TRANSPORT_TYPE,

		/* Disable��Enable TCP keepalive */
		OPTION_TYPE_TCP_KEEP_AVLIE,

		/* ��ȡ��ǰ���ӷ��ͻ�������С */
		OPTION_TYPE_SEND_BUF_LENGTH,

		/* ��ȡ��ǰ���շ��ͻ�������С */
		OPTION_TYPE_RECEIVE_BUF_LENGTH,

		/* ��ȡ��ǰ������ӵ�е�socket���� */
		OPTION_TYPE_GET_SOCKET,

		/* ���Ӽ�������ڴ���Server��Clientʱ��ʹ��CONNECTION_TYPE_KEEP_ALIVEѡ� */
		OPTION_TYPE_SET_KEEPALIVE_INTERVEL,

		/* ���Ӽ��������ڴ���Server��Clientʱ��ʹ��CONNECTION_TYPE_KEEP_ALIVEѡ� */
		OPTION_TYPE_SET_KEEPALIVE_CHECKNUM,

        /* SSL�����ڲ�����������ֵ�SSL����ʱʹ�ã� */
        OPTION_TYPE_SET_SSL,
	};

public:
	virtual ~ITransport() {}

public:
	/**
	 * ��һ����������һ���¼�������
	 *
	 * <pSink>, ����󶨵��¼�������
	 *
	 * ����ֵ:
	 *		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result Open( ITransportSink* pSink ) = 0;

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
	virtual ME_Result Close( ME_Result hReason = ME_OK ) = 0;

	/**
	* ��������
	*
	* <pmbBlock>, ���͵�����
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result SendData( CMEMessageBlock* pmbBlock ) = 0;

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
		void* pOptionValue ) = 0;

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
		void*& pOptionValue ) = 0;

	/**
	* ȡ�õ�ǰ���transport�趨��sink
	*
	* ����ֵ:
	*		��NULL��ʾ�ɹ�; NULL��ʾʧ��
	*/
	virtual ITransportSink* GetSink() const = 0;
};

class ME_OS_EXPORT ITransportSink
{
public:
	virtual ~ITransportSink() {}

public:
	/**
	* �յ�����
	*
	* <pTransport>, ��һ����������һ�������յ�������
	* <pmbBlock>, �յ����ݵ�����
	*/
	virtual void OnReceive( 
		ITransport* pTransport,
		CMEMessageBlock* pmbBlock ) = 0;

	/**
	* ���ӱ��Ͽ�
	* 
	* <pTransport>, ��һ�����ӶϿ���
	* <hResult>, Ϊʲô�Ͽ���
	*/
	virtual void OnDisconnect( 
		ITransport* pTransport,
		ME_Result hResult = ME_OK ) = 0;

	/**
	* ĳ�����ӿ������·���������
	* �����ݷ����ر�Ƶ��ʱ, ����tcp/ip buffer�Ѿ�����, �޷������������ݵ�ʱ��,
	* Transport��SendData()�����᷵��ʧ��, ���ʱ��, �Ͳ�Ӧ�ü����ٷ�����,
	* (�ֵ�, �ǵü��SendData(), �����������ʧ����, �ǵñ���÷���ʧ�ܵ������, ׼���´��ٷ�)
	* ��Ϊ����ô��, ������ɹ���, ��Ψһ��Ҫ���ľ��ǵȴ�����ص�
	* �����յ�����ص���ʱ��, ��ϲ, ���ֿ��Լ�������������Ϸ���������
	*
	* <pTransport>, ��һ�����ӿ��Լ������������� 
	* <hResult>, Ϊʲô���Լ�������? (ͨ��, ���ﶼֻ�᷵��ME_OK) 
	*
	*/
	virtual void OnCanSendAgain( 
		ITransport* pTransport,
		ME_Result hReason = ME_OK ) = 0;
};

ME_NAME_SPACE_END

#endif	// __CONNECTION_INTERFACE_H_







