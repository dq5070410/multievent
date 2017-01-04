/**
* 专用UDP协议的连接器原生类
* 
*
* History
*	3/12/2016		Created
*/

#ifndef __CONNECTOR_UDP_H_
#define __CONNECTOR_UDP_H_

#include "Base.h"
#include "IO.h"
#include "ConnectorImpl.h"
#include "SocketUdp.h"
#include "AutoPtrT.h"

ME_NAME_SPACE_BEGIN

/* 所涉及事件的预声明 */
namespace EVENT
{
	/* 在当前线程执行OnConnect() */
	template <class ConnectorType> class EventOnConnectCurrentT;
}

/**
* 专用UDP协议的连接器原生类
*/
class ME_OS_EXPORT CMEConnectorUdp 
	: public IMEIO
	, public IMEConnectorImpl
{
public:
	typedef CMEConnectorUdp ConnectorType;
	typedef IMEConnectorImplSink ConnectorSinkType;

public:
	CMEConnectorUdp();

	virtual ~CMEConnectorUdp();

	/* 继承自IMEIO */
public:
	virtual void SetHandle( ME_HANDLE hHandle );

	virtual ME_HANDLE GetHandle() const;

	/* 继承自IMEConnectorImpl */
public:

	/**
	* 打开, 即连接到某一个合法地址
	*
	* <pSink>, 事件接收器
	* <pReactor>, 事件分离器对象
	* <hHandle>, 连接成功后的句柄
	* <peerAddress>, 连接到哪里
	* <localAddress>, 需要绑定的本地地址
	*
	* 返回值:
	*		ME_OK为成功, 其他为失败
	*/
	virtual ME_Result Open( 
		IMEConnectorImplSink* pSink,
		const CMEInetAddress& peerAddress,
		CMEReactor* pReactor,
		ME_HANDLE& hHandle,
		CMEInetAddress* pLocalAddress = NULL );

	/**
	* 关闭, 即停止连接
	*
	* 返回值:
	*		无
	*/
	virtual void Close();

private:
	void NotifySuccess();

private:
	void OnConnect_c( ME_Result hResult );

protected:
	IMEConnectorImplSink*			m_pSink;		// 回调对象，一般是wrapper

	CMESocketUdp					m_SocketUdp;	// udp socket

	/* 事件友元类声明 */
protected:
	friend class EVENT::EventOnConnectCurrentT<ConnectorType>;

	/* 事件友元类再定义 */
protected:
	typedef EVENT::EventOnConnectCurrentT<ConnectorType> OnConnectEvent;
};

ME_NAME_SPACE_END

#endif	// __CONNECTOR_UDP_H_




