/**
* Transport�Ļ���ʵ����
*
* ���û���������������������͵�transport��ö�
* ������̳�
* 
*
* History
*	12/18/2016		Created
*/

#ifndef __TRANSPORT_BASE_H_
#define __TRANSPORT_BASE_H_

#include "Base.h"
#include "TransportImpl.h"
#include "IO.h"
#include "AutoPtrT.h"

ME_NAME_SPACE_BEGIN

class CMESocketBase;
class CMEReactor;

/**
* Transport�Ļ���ʵ����
*
* ���û���������������������͵�transport��ö�
* ������̳�
*/
class ME_OS_EXPORT CTransportBase 
	: public ITransportImpl
	, public IIOSink
	, public IIO
{
public:
	typedef ITransportImplSink TransportSinkType;

public:
	virtual ~CTransportBase();

public:
	/**
	 * ��һ����������һ���¼�������
	 *
	 * <pReactor>, ʹ���ĸ�reactor
	 * <pSink>, ����󶨵��¼�������
	 *
	 * ����ֵ:
	 *		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result Open( 
		CReactor* pReactor,
		ITransportImplSink* pSink );

	/**
	* �ر�һ��������
	* һ���������������, ��ô�����������Ҳ���ܱ�ʹ����, ֮ǰ��֮
	* �󶨵Ľ�����Ҳ�����ٽ��յ��κ��¼�, ����㻹��Ҫһ��Transport,
	* ��ô��ֻ��ͨ��Connector����Acceptor��Open()����, Ȼ��ȴ�
	* Sink��OnConnect()�����
	*
	* <pReactor>, ʹ���ĸ�reactor
	* <hReason>, ��ΪʲôҪ�ر��������
	*
	* ����ֵ:
	*		ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
	*/
	virtual ME_Result Close( 
		CReactor* pReactor,
		ME_Result hReason = ME_OK );

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

public:
	virtual void SetHandle( ME_HANDLE hHandle );

	ME_HANDLE GetHandle() const;

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
	CTransportBase( 
		DWORD dwType, 
		ME_HANDLE hSocketHandle );

protected:
	void SetBuffSize();

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
	virtual DWORD CheckUnReadDataImpl() = 0;

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
		DWORD dwDataSize ) = 0;

protected:
	TransportSinkType*				m_pSink;
	
	/**
	* ����������ڱ��浱ǰ����ʹ�õ�reactorʵ��
	* 
	* ���Ĵ��ڣ�ʹ��ÿһ�������������ڲ�ͬ��reactorע��
	* ����reactor���̵߳Ĺ�ϵ�����൱�ڼ�ӵ���transport
	* �Ĳ��������ڲ�ͬ���߳��н��У���������ΪHikUtility
	* ��̵Ĳ�����ܶ�׼���ģ���������������ֵ�ȷ������
	* �����Ĳ������ܣ����Ⲣ����ζ���������ô����
	*
	* ͬ������������ص㣬��ʹ�ö��̼߳���������л����
	* �ǳ��ĸ��ӣ�����㲻���ر����ϤHikUtility�Լ����߳�
	* �����������˳�ɫ���ҽ����㲻Ҫ���׵ĳ������ַ�ʽ
	* 
	* �ȫ�ķ����ǣ�����ͬһ��transportʵ����ÿһ������
	* ������ͬһ��reactor
	*/
	CMEReactor*					m_pReactor;
	int								m_iIndex;

	CMESocketBase*					m_pSocket;
};

ME_NAME_SPACE_END

#endif	// __TRANSPORT_BASE_H_








