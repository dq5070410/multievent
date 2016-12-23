/**
* 一个自动保存socket错误的类
* 
* 在linux下，socket错误会保存在公共的errno里
* 为了避免socket error被其他error覆盖，就搞了
* 这么个类
* 
*
* History
*	12/11/2012		Created
*/

#ifndef __SOCKET_ERROR_AUTO_SAVE_H_
#define __SOCKET_ERROR_AUTO_SAVE_H_

#include "Base.h"

/**
* 一个自动保存socket错误的类
* 
* 在linux下，socket错误会保存在公共的errno里
* 为了避免socket error被其他error覆盖，就搞了
* 这么个类
*/
class CSocketErrorAutoSave
{
public:
	CSocketErrorAutoSave();
	~CSocketErrorAutoSave();

public:
	/**
	* 获取系统错误
	*/
	int GetLastError() const;

private:
	int								m_iError;
};

#endif	// __SOCKET_ERROR_AUTO_SAVE_H_



