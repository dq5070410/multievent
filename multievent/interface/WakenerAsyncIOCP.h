/**
* 仅仅适用于IOCP Reactor的wakener
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
* 仅仅适用于IOCP Reactor的wakener
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
     *  通知分离器跳出等待
     *
     *  返回值: 
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result Notify();

    /**
     *  初始化唤醒者
     *
     *  将唤醒者的资源（SocketPair或Windows事件对象等）创建工作集中在这里完成，
     *  这样比较容易获知这些资源是否创建成功
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result Initialize();

    /**
     *  反初始化唤醒者
     *
     *  将唤醒者的资源销毁工作集中在这里完成
     *
     *  返回值:
     *      ME_OK表示成功; 非ME_OK表示失败
     */
    virtual ME_Result Finalize();

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

private:
	ME_HANDLE						m_hIOCP;
	CMEAsyncIoNotify				m_asyncNotify;
};



ME_NAME_SPACE_END

#endif	// ME_WIN

#endif	// __WAKENER_IOCP_H_







