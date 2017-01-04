/**
* 专用TCP协议的连接器原生类
* 
*
* History
*	1/28/2016		Created
*/

#ifndef __CONNECTOR_TCP_H_
#define __CONNECTOR_TCP_H_

#include "Base.h"
#include "IO.h"
#include "ConnectorImpl.h"
#include "SocketTcp.h"
#include "AutoPtrT.h"

ME_NAME_SPACE_BEGIN

namespace EVENT
{
	template <class ConnectorType> class EventOnConnectCurrentT;
}

/**
* 专用TCP协议的连接器原生类
*
* 它只“生”句柄，但是管理句柄，因为它生产的句柄都
* 交付给使用者 -- 通常是wrapper
*/
class ME_OS_EXPORT CMEConnectorTcp
	: public IMEIO
	, public IMEIOSink
	, public IMEConnectorImpl
{
public:
	typedef CMEConnectorTcp ConnectorType;
	typedef IMEConnectorImplSink ConnectorSinkType;

public:
	CMEConnectorTcp();

	virtual ~CMEConnectorTcp();

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

protected:
	ME_Result Connect(
		CMESocketTcp* pSocket,
		const CMEInetAddress& peerAddr,
		CMEReactor* pReactor,
		int iIndex );

	void NotifySuccess();

	/* 继承自IMEIOSink */
protected:
	/**
	* input事件发生
	*
	* <hHandle>，哪个句柄发生了input事件
	* <iEvent>，具体是什么样的事件
	*
	* 返回值:
	*		无
	*/
	virtual void OnInput(
		ME_HANDLE hHandle,
		int iEvent );

	/**
	* output事件发生
	*
	* <hHandle>，哪个句柄发生了output事件
	* <iEvent>，具体是什么样的事件
	*
	* 返回值:
	*		无
	*/
	virtual void OnOutput(
		ME_HANDLE hHandle,
		int iEvent );

	/**
	* close事件发生
	*
	* <hHandle>，哪个句柄发生了close事件
	* <iEvent>，具体是什么样的事件
	*
	* 返回值:
	*		无
	*/
	virtual void OnClose(
		ME_HANDLE hHandle,
		int iEvent );

private:
	void OnConnect_c( ME_Result hResult );

protected:
	ConnectorSinkType*				m_pSink;		// 回调对象，一般都是connector wrapper

	CMESocketTcp					m_SocketTcp;	// 其实只是一个“套”，用于生产句柄
	CMEReactor*					m_pReactor;		// io 分离器
	int								m_iIndex;		// 注册事件时，<m_pReactor>返回的事件索引

	/* 事件友元类声明 */
private:
	friend class EVENT::EventOnConnectCurrentT<ConnectorType>;

	/* 事件友元类再定义 */
private:
	typedef EVENT::EventOnConnectCurrentT<ConnectorType> OnConnectEvent;
};

ME_NAME_SPACE_END

#endif	// __CONNECTOR_TCP_H_

