/**
* IO封装接口类
* 
* History
*	12/11/2016		Created
*/

#ifndef __IO_H_
#define __IO_H_

#include "Base.h"

NAME_SPACE_BEGIN

/**
* IO封装接口类
*/
class IIO
{
public:
	virtual ~IIO() {}

public:
	virtual void SetHandle( ME_HANDLE hHandle ) = 0;

	virtual ME_HANDLE GetHandle() const = 0;
};

/**
* IO封装接口回调类
*/
class IIOSink
{
public:
	virtual ~IIOSink() {}

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
		int iEvent ) = 0;

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
		int iEvent ) = 0;

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
		int iEvent ) = 0;
};

NAME_SPACE_END

#endif	// __IO_H_








