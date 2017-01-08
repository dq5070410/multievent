/**
* TCP侦听器的原生类
* 
*
* History
*	12/12/2016		Created
*/

#ifndef __ACCEPTOR_TCP_H_
#define __ACCEPTOR_TCP_H_

#include "Base.h"
#include "AcceptorImpl.h"
#include "IO.h"

ME_NAME_SPACE_BEGIN

class CMESocketTcp;
class CMEReactor;

/**
* TCP侦听器的原生类
*
* 它的主要工作是把socket与reactor联系起来
* 
* 它不是线程安全的
*/
class  ME_OS_EXPORT CMEAcceptorTcp 
	: public IMEIOSink
	, public IMEIO
	, public IMEAcceptorImpl
{
public:
	typedef CMEAcceptorTcp AcceptorType;
	typedef IMEAcceptorImplSink AcceptorSinkType;

public:
	CMEAcceptorTcp();

	virtual ~CMEAcceptorTcp();

public:
	virtual void SetHandle( ME_HANDLE hHandle );

	virtual ME_HANDLE GetHandle() const;

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

	/* 继承自IMEIO */
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

protected:
	AcceptorSinkType*				m_pSink;		// 回调对象
	int								m_iIndex;		// 表示acceptor作为io对象在reactor索引表中的位置
	CMESocketTcp*					m_pSocketTcp;	// TCP socket
};

ME_NAME_SPACE_END

#endif	// __ME_ACCEPTOR_TCP_H_







