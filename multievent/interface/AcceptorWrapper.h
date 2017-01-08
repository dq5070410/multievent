/**
* Acceptor的工厂类
*
* 把acceptor的内部实现与IMEAcceptor进行对接
* 对原生类的acceptor，它隔绝了transport概念
* 对上，它引入了引用计数，以便于上下层交互使用
* 
*
* History
*	12/14/2016		Created
*/

#ifndef __ACCEPTOR_WRAPPER_H_
#define __ACCEPTOR_WRAPPER_H_

#include "Base.h"
#include "ConnectionInterface.h"
#include "ConnectionDefines.h"
#include "AcceptorImpl.h"
#include "ReferenceControlT.h"
#include "SocketUdp.h"
#include "ConnectionOperatorInterface.h"

ME_NAME_SPACE_BEGIN

class CMEInetAddress;

namespace EVENT
{
	/* 在当前线程中执行OnConnect() */
	template <class AcceptorType, class TransportType> class EventOnAcceptCurrentT;
}

/**
* Acceptor的工厂类
*
* 把acceptor的内部实现与IMEAcceptor进行对接
* 对原生类的acceptor，它隔绝了transport概念
* 对上，它引入了引用计数，以便于上下层交互使用
*
* TCP/UDP它都wrap
*/
class CMEAcceptorWrapper
	: public IMEAcceptor
	, public IMEAcceptorUdpImplSink
	, public CMEMutexTypeTraits<CMEConnDummy>
	, public CMEReferenceControlT<CMEConnDummy::MutexType>
{
public:
	typedef CMEAcceptorWrapper AcceptorType;
	typedef IMETransport TransportType;

public:
	CMEAcceptorWrapper();
	virtual ~CMEAcceptorWrapper();

public:
	/**
	* 根据不同的类型初始化一个真正可用的acceptor
	*
	* <dwType>, 标识该acceptor是什么类型的, tcp/udp等等
	*
	* 返回值:
	*		ME_OK为成功; 其他失败
	*/
	ME_Result Initialize( DWORD dwType );

    ME_Result RegisterOperator( IMEConnectionOperator* pOperator );

    ME_Result UnregisterOperator( IMEConnectionOperator* pOperator );

	/* 继承自IMEAcceptor */
public:
	/**
	* 打开一个Acceptor并且侦听<localAddress>
	* 当有新的连接建立时, 会通过<pSink>的OnConnect()来通知
	*
	* <pSink>, 事件接收器
	* <pTransport>, 一个新产生的流对象 -- 对应一个连接
	*
	* 返回值:
	*		ME_OK为成功; 其他失败
	*/
	virtual ME_Result Open( 
		IMEAcceptorSink* pSink,
		const CMEInetAddress& localAddress );

	/**
	* 关闭一个Acceptor并且停止侦听
	* 当这个方法被调用之后, 先前绑定的接收器不会再收到任何事件
	*
	* <scReason>, 关闭的原因是什么
	*
	* 返回值:
	*		无
	*/
	virtual void Close( ME_Result scReason = ME_OK );

	/**
	* 设置connector的属性
	*
	* <dwOptionType>, 要设置什么属性
	* <pOptionValue>, 对应于<dwOptionType>的值
	*
	* 返回值:
	*		ME_OK表示成功; 非ME_OK表示失败
	*/
    virtual ME_Result SetOption(
        DWORD dwOptionType,
        void* pOptionValue );

	/**
	* 取得connector的属性
	*
	* <dwOptionType>, 要取得什么属性
	* <pOptionValue>, out, 对应于<dwOptionType>的值
	*
	* 返回值:
	*		ME_OK表示成功; 非ME_OK表示失败
	*/
    virtual ME_Result GetOption(
        WORD dwOptionType,
        void*& pOptionValue );

    /* 继承自IMEReferenceControl */
public:
	/**
	* 增加引用计数1
	*
	* 返回值:
	*		递增之后的引用计数, DWORD
	*/
	virtual DWORD AddReference();

	/**
	* 减少引用计数1
	*
	* 返回值:
	*		递减之后的引用计数, DWORD
	*/
	virtual DWORD RemoveReference();

	/* 继承自IMEAcceptorImplSink */
protected:
	/**
	* 有连接产生
	*
	* <hResult>, 对端连接的结果, 一般都是ME_OK
	* <pHandle>, 新产生的socket句柄
	* <pPeerAddress>, 对端地址
	* 
	* 返回值,
	*		无
	*/
	virtual void OnConnect(
		ME_Result hResult,
		ME_HANDLE hHandle,
		CMEInetAddress* pPeerAddress );

	/**
	* 有连接产生 -- 仅仅在UDP的情况下触发
	*
	* <hResult>, 对端连接的结果, 一般都是ME_OK
	* <hHandle>, UDP acceptor自身的句柄
	* 
	* 返回值,
	*		无
	*/
	virtual void OnInput(
		ME_Result hResult,
		ME_HANDLE hHandle );

private:
	/**
	* 读取数据
	*
	* <hHandle>, socket句柄或CMEAsyncIoRecvFrom::Result*指针
	* <iRecvLen>, 从<hHandle>中获取到的数据长度
	* <pszData>, 从<hHandle>中获取到的数据
	* <hiaPeerAddr>, 对端地址
	* 
	* 返回值,
	*		无
	*/
	void ReadData(
		ME_HANDLE hHandle,
		int& iRecvLen,
		char*& pszData,
		CMEInetAddress& hiaPeerAddr );

	/**
	* 通知TransportUdpManager
	*
	* <pTransport>, 从哪一个transport对象上来的数据
	* <iRecvLen>, 获取到的数据长度
	* <pszData>, 获取到的数据
	* <hiaPeerAddr>, 对端地址
	* 
	* 返回值,
	*		无
	*/
	void NotifyUdpManager( 
		IMETransport* pTransport,
		int iRecvLen,
		char* pszData,
		const CMEInetAddress& hiaPeerAddr );

private:
	void OnConnect_c( TransportType* pTransport );

    ME_Result PreOnConnect(
        ME_HANDLE hSocket,
        CMETransportAutoPtr& pTransportAutoPtr );

	/* 事件友元类的声明 */
private:
	friend class EVENT::EventOnAcceptCurrentT<AcceptorType, TransportType>;

	/* 友元事件的再定义 */
private:
	typedef EVENT::EventOnAcceptCurrentT<AcceptorType, TransportType> OnAcceptCurrentEvent;

private:
    typedef std::deque<IMEConnectionOperatorAutoPtr> OperatorDequeType;

private:
	DWORD							m_dwType;		// 标识该acceptor是什么类型的，tcp/udp...
	CMEAutoPtrT<IMEAcceptorImpl>	m_pAcceptor;	// 2013.9.6 editedy by 韦珂 // 原生类指针，类型与<m_dwType>对应

	int								m_iReactorType;	// 用于记录在网络线程的reactor类型

	IMEAcceptorSink*				m_pSink;		// 上层回调对象

	CMESocketUdp					m_SocketUdp;	// 当且仅当在UDP时会起作用

    OperatorDequeType               m_dequeOperator;
};

ME_NAME_SPACE_END

#endif	// __ACCEPTOR_WRAPPER_H_




