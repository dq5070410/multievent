/**
* 异步IO类型的udp connector
* 
*
* History
*	9/07/2016		Created
*/
#ifndef __CONNECTOR_ASYCH_UDP_H_
#define __CONNECTOR_ASYCH_UDP_H_

#include "ConnectorUdp.h"
#include "AsyncIoDefines.h"
#include "AsyncIoConnect.h"

ME_NAME_SPACE_BEGIN

/**
* 它存在的意义在是：
* 1. 设计上保持一致，但凡TCP/UDP系列的东西都有普通和Async两种
* 2. 清晰原有的实现，不然factory在构造connector时无视输入参数是否带有Async容易让人困惑
*	 (尽管从功能上说它们没什么不同，因为udp不存在连接的过程，所以普通或异步都一样)
*/
class ME_OS_EXPORT CMEConnectorAsyncUdp
	: public CMEConnectorUdp
	, public IMEAsyncIoSink
{
public:
	virtual ~CMEConnectorAsyncUdp();

public:
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
};

ME_NAME_SPACE_END

#endif	// __CONNECTOR_ASYCH_UDP_H_

















