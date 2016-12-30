/**
* Transport的设计者
*
* 如果你想定制一个与众不同的transport，或者目前的designer不合你要求
* 要么就从这个里继承，实现你想要的 -- 如果你决定这么做了，也许你应该
* 再去看看builder
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
* Transport的设计者
*
* 如果你想定制一个与众不同的transport，或者目前的designer不合你要求
* 要么就从这个里继承，实现你想要的 -- 如果你决定这么做了，也许你应该
* 再去看看builder
*/
class IMETransportDesigner
{
public:
	virtual ~IMETransportDesigner() {}

public:
	virtual ME_Result Build( IMETransportBuilder* pBuilder ) = 0;
};

/**
* 目前transport使用的designer，它决定了transport使用什么的样式
* 去构建 -- 具体每一项的构建，得看builder
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







