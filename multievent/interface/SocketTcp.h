/**
* tcp socket的封装类
* 
*
* History
*	12/12/2016		Created
*/

#ifndef __SOCKET_TCP_H_
#define __SOCKET_TCP_H_

#include "Base.h"
#include "SocketBase.h"

ME_NAME_SPACE_BEGIN

/**
* TCP socket的封装类
* 
* 注意，它不是线程安全的
*/
class ME_OS_EXPORT CMESocketTcp : public CMESocketBase
{
public:
	CMESocketTcp();

	CMESocketTcp( ME_HANDLE hHandle );

	virtual ~CMESocketTcp();

public:
	/**
	* 关闭socket的读取功能
	* 
	*
	* 返回值:
	*		ME_OK是成功; 其他是失败
	*/
	ME_Result DisableRead();

	/** 
	* 关闭socket的发送功能
	* 
	*
	* 返回值:
	*		ME_OK是成功; 其他是失败
	*/
	ME_Result DisableWrite();

protected:
	virtual ME_Result OpenImpl();

	virtual ME_Result CloseImpl();
};

ME_NAME_SPACE_END

#endif	// __SOCKET_TCP_H_







