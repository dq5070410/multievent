/**
* tcp socket�ķ�װ��
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
* TCP socket�ķ�װ��
* 
* ע�⣬�������̰߳�ȫ��
*/
class ME_OS_EXPORT CMESocketTcp : public CMESocketBase
{
public:
	CMESocketTcp();

	CMESocketTcp( ME_HANDLE hHandle );

	virtual ~CMESocketTcp();

public:
	/**
	* �ر�socket�Ķ�ȡ����
	* 
	*
	* ����ֵ:
	*		ME_OK�ǳɹ�; ������ʧ��
	*/
	ME_Result DisableRead();

	/** 
	* �ر�socket�ķ��͹���
	* 
	*
	* ����ֵ:
	*		ME_OK�ǳɹ�; ������ʧ��
	*/
	ME_Result DisableWrite();

protected:
	virtual ME_Result OpenImpl();

	virtual ME_Result CloseImpl();
};

ME_NAME_SPACE_END

#endif	// __SOCKET_TCP_H_







