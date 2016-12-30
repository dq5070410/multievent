/**
* Transport�������
*
* ������붨��һ�����ڲ�ͬ��transport������Ŀǰ��designer������Ҫ��
* Ҫô�ʹ������̳У�ʵ������Ҫ�� -- ����������ô���ˣ�Ҳ����Ӧ��
* ��ȥ����builder
* 
*
* History
*	12/20/2016		Created
*/

#ifndef __TRANSPORT_DESIGNER_H_
#define __TRANSPORT_DESIGNER_H_

#include "Base.h"

ME_NAME_SPACE_BEGIN

class IMETransportBuilder;
class IMETransport;
class CMEInetAddress;

/**
* Transport�������
*
* ������붨��һ�����ڲ�ͬ��transport������Ŀǰ��designer������Ҫ��
* Ҫô�ʹ������̳У�ʵ������Ҫ�� -- ����������ô���ˣ�Ҳ����Ӧ��
* ��ȥ����builder
*/
class IMETransportDesigner
{
public:
	virtual ~IMETransportDesigner() {}

public:
	virtual ME_Result Build( IMETransportBuilder* pBuilder ) = 0;
};

/**
* Ŀǰtransportʹ�õ�designer����������transportʹ��ʲô����ʽ
* ȥ���� -- ����ÿһ��Ĺ������ÿ�builder
*/
class CMETransportDesigner : public IMETransportDesigner
{
public:
	CMETransportDesigner(
		DWORD dwType,
		ME_HANDLE hHandle,
		CMEInetAddress* pPeerAddress = NULL );

	virtual ~CMETransportDesigner();

public:
	virtual ME_Result Build( IMETransportBuilder* pBuilder );

private:
	DWORD							m_dwType;
	ME_HANDLE						m_hHandle;
	CMEInetAddress*				m_pPeerAddress;
};

class CMETransportCrossThreadDesigner : public IMETransportDesigner
{
public:
	CMETransportCrossThreadDesigner( IMETransport* pSrcTransport );

	virtual ~CMETransportCrossThreadDesigner();

public:
	virtual ME_Result Build( IMETransportBuilder* pBuilder );

private:
	IMETransport*					m_pSrcTransport;
};

ME_NAME_SPACE_END


#endif	// __TRANSPORT_DESIGNER_H_







