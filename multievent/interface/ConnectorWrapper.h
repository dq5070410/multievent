/**
* Connector的工厂类
*
* 把Connector的内部实现与IMEConnector进行对接
* 对于原生类connector屏蔽了transport，加入引用计数的控制
* 以及部分与连接并不直接相关的功能(如，连接超时等等)的粘合
*
*
* History
*	1/29/2016		Created
*/

#ifndef __CONNECTOR_WRAPPER_H_
#define __CONNECTOR_WRAPPER_H_

#include "ConnectionInterface.h"
#include "ConnectionDefines.h"
#include "ConnectorImpl.h"
#include "ReferenceControlT.h"
#include "Timer.h"
#include "InetAddress.h"
#include "ConnectionOperatorInterface.h"
#include "Thread.h"

ME_NAME_SPACE_BEGIN

/**
* Connector的工厂类
*
* 把Connector的内部实现与IMEConnector进行对接
* 对于原生类connector屏蔽了transport，加入引用计数的控制
* 以及部分与连接并不直接相关的功能(如，连接超时等等)的粘合
*
* TCP/UDP都wrap
*/
class CMEConnectorWrapper
	: public IConnector
	, protected IMEConnectorImplSink
	, protected ITimerSink
	, public CMEMutexTypeTraits<CMEConnDummy>
	, public CMEReferenceControlT<CMEConnDummy::MutexType>
{
public:
	CMEConnectorWrapper();

	virtual ~CMEConnectorWrapper();

public:
	/**
	* 根据不同的类型初始化一个真正可用的connector
	*
	* <dwType>, 标识该connector是什么类型的, tcp/udp等等
	*
	* 返回值:
	*		ME_OK为成功; 其他失败
	*/
	ME_Result Initialize( DWORD dwType );

    ME_Result RegisterOperator( IMEConnectionOperator* pOperator );

    ME_Result UnregisterOperator( IMEConnectionOperator* pOperator );

	/* 继承自IMEConnector */
public:
	/**
	* 打开一个Connector并且尝试连接到<peerAddress>
	* 当这个连接建立时, 会通过<pSink>的OnConnect()来通知
	*
	* <pSink>, 事件接收器
	* <peerAddress>, 需要连接的对端地址
	* <pTimeout>, 尝试连接过多长时间没响应算超时
	* <pLocalAddress>, 需要绑定的本地地址
	*
	* 返回值:
	*		ME_OK为成功; 非ME_OK为失败
	*/
	virtual ME_Result Open(
		IConnectorSink* pSink,
		const CMEInetAddress& peerAddress,
		CTimeValue* pTimeout = NULL,
		CMEInetAddress* pLocalAddress = NULL );

	
	/**
	* 关闭一个Connector并且停止连接
	* 当这个方法被调用之后, 先前绑定的接收器不会再收到任何事件
	*
	* 返回值:
	*		无
	*/
	virtual void Close();

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

	/* 继承自IMEConnectorImplSink */
protected:
	/**
	* 有连接产生
	*
	* <hResult>, 对端连接的结果, 一般都是ME_OK
	* <hHandle>, 连接句柄
	* 
	* 返回值,
	*		无
	*/
	virtual void OnConnect( 
		ME_Result hResult,
		ME_HANDLE hHandle );

	/* 继承自ITimerSink */
protected:
	/**
	* 你schedule的时钟发生了
	*
	* <pTimerID>，哪个时钟发生了
	*
	* 返回值:
	*		无
	*/
	virtual void OnTimer( CMETimer* pTimerID );

private:
	void CloseHandle();

    ME_Result PreOnConnect(
        ME_HANDLE hSocket,
        CTransportAutoPtr& pTransportAutoPtr );

private:
    typedef std::deque<IMEConnectionOperatorAutoPtr> OperatorDequeType;

private:
	DWORD							m_dwType;			// 标识该connector是什么类型的，tcp/udp...
	CMEAutoPtrT<IMEConnectorImpl>	m_pConnector;		// 原生类指针，类型与<m_dwType>对应

	IConnectorSink*			    	m_pSink;			// 上层回调对象
	CMETimer				 		m_Timer;			// 用作连接超时以及跳出函数
	BOOL							m_bConnected;		// 用于标识Open()函数调用成功与否
	ME_HANDLE						m_hHandle;			// 用于在没有连接成功(即没有产生transport)时，对已经产生的句柄进行清理，防止句柄泄漏

	CMEInetAddress					m_hiaPeerAddress;	// 对端地址

    OperatorDequeType               m_dequeOperator;

	CMEThread*						m_pTimerThread; // 投递Timer线程
};

ME_NAME_SPACE_END

#endif	// __CONNECTOR_WRAPPER_H_


