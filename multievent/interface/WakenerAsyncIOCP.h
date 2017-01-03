/**
* ����������IOCP Reactor��wakener
* 
*
* History
*	9/07/2016		Created
*/
#ifndef __WAKENER_IOCP_H_
#define __WAKENER_IOCP_H_

#include "Wakener.h"
#include "AsyncIoDefines.h"
#include "AsyncIoNotify.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

/**
* ����������IOCP Reactor��wakener
*/
class CMEWakenerAsyncIOCP
	: public IMEWakener
	, public IMEAsyncIoSink
{
public:
	CMEWakenerAsyncIOCP( ME_HANDLE hInterface );

	virtual ~CMEWakenerAsyncIOCP();

public:
	virtual void SetHandle( ME_HANDLE hHandle );

	virtual ME_HANDLE GetHandle() const;

	/**
     *  ֪ͨ�����������ȴ�
     *
     *  ����ֵ: 
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result Notify();

    /**
     *  ��ʼ��������
     *
     *  �������ߵ���Դ��SocketPair��Windows�¼�����ȣ���������������������ɣ�
     *  �����Ƚ����׻�֪��Щ��Դ�Ƿ񴴽��ɹ�
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result Initialize();

    /**
     *  ����ʼ��������
     *
     *  �������ߵ���Դ���ٹ����������������
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result Finalize();

public:
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

private:
	ME_HANDLE						m_hIOCP;
	CMEAsyncIoNotify				m_asyncNotify;
};



ME_NAME_SPACE_END

#endif	// ME_WIN

#endif	// __WAKENER_IOCP_H_







