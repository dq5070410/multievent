/**
* UDP侦听器的原生类
* 
*
* History
*	3/12/2016		Created
*/

#ifndef __ME_ACCEPTOR_UDP_H_
#define __ME_ACCEPTOR_UDP_H_

#include "Base.h"
#include "AcceptorImpl.h"
#include "IO.h"

ME_NAME_SPACE_BEGIN

class CMESocketUdp;
class CMEReactor;

/**
* UD侦听器的原生类
*
* 它的主要工作室把socket与reactor联系起来
* 
* 它不是线程安全的
*/
class CMEAcceptorUdp 
	: public IMEIO
	, public IMEIOSink
	, public IMEAcceptorImpl
{
public:
	typedef CMEAcceptorUdp AcceptorType;
	typedef IMEAcceptorUdpImplSink AcceptorSinkType;

public:
	CMEAcceptorUdp();

	virtual ~CMEAcceptorUdp();

	/* 继承自IMEIO */
public:
	virtual void SetHandle( ME_HANDLE hHandle );

	virtual ME_HANDLE GetHandle() const;

	/* 继承自IMEAcceptorImpl */
public:
	/**
	* 打开, 即侦听与本地某个端口
	*
	* <pSink>, 事件接收器
	* <pReactor>, 事件分离器对象
	* <hiaLocalAddress>, 需要绑定的本地地址
	*
	* 返回值:
	*		ME_OK为成功, 其他为失败
	*/
	virtual ME_Result Open( 
		IMEAcceptorImplSink* pSink,
		CMEReactor* pReactor,
		const CMEInetAddress& hiaLocalAddress );

	/**
	* 关闭, 即停止侦听
	*
	* 返回值:
	*		无
	*/
	virtual void Close( CMEReactor* pReactor );

	/* 继承自IMEIOSink */
protected:
	virtual void OnInput(
		ME_HANDLE hHandle,
		int iEvent );

	virtual void OnOutput(
		ME_HANDLE hHandle,
		int iEvent );

	virtual void OnClose(
		ME_HANDLE hHandle,
		int iEvent );

protected:
	AcceptorSinkType*				m_pSink;		// 回调对象
	int								m_iIndex;		// 表示acceptor作为io对象在reactor索引表中的位置
	CMESocketUdp*					m_pSocketUdp;	// UDP socket
};


ME_NAME_SPACE_END


#endif	// __ACCEPTOR_UDP_H_




