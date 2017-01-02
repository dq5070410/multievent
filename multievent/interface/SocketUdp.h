/**
* udp socket的基装类
* 
*
* History
*	12/12/2016		Created
*/

#ifndef __SOCKET_UDP_H_
#define __SOCKET_UDP_H_

#include "Base.h"
#include "SocketBase.h"

ME_NAME_SPACE_BEGIN

/**
* UDP socket的封装类
* 
* 注意，它不是线程安全的
*/
class ME_OS_EXPORT CMESocketUdp : public CMESocketBase
{
public:
	CMESocketUdp();

	CMESocketUdp( ME_HANDLE hHandle );

	virtual ~CMESocketUdp();

public:
	/** 
	* 发送数据
	* 
	* <pszData>, 要发送的数据
	* <dwLength>, 要发送数据的长度
	* <hiaPeerAddress>, 目标地址
	* <iFlag>, 标记
	*
	* 返回值:
	*		SOCKET_ERROR是失败; 0是成功
	*/
	int SendTo(
		const char* pszData, 
		DWORD dwLength,
		const CMEInetAddress& hiaPeerAddress,
		int iFlag = 0 );

	/** 
	* 发送数据
	* 
	* <pszIov>, 要发送的数据数组
	* <dwCount>, 要发送的数据数组的个数
	* <hiaPeerAddress>, 目标地址
	*
	* 返回值:
	*		SOCKET_ERROR是失败; 0是成功
	*/
	int SendVTo(
		const iovec pszIov[], 
		DWORD dwCount,
		const CMEInetAddress& hiaPeerAddress );

	/** 
	* 收取数据
	* 
	* <pszBuf>, 收取数据的缓冲区
	* <dwLength>, 收取数据的缓冲区大小
	* <hiaPeerAddress>, 目标地址
	* <iFlag>, 标记
	*
	* 返回值:
	*		SOCKET_ERROR是失败; 0是成功
	*/
	int RecvFrom(
		char*& pszData, 
		DWORD dwLength,
		CMEInetAddress& hiaPeerAddress,
		int iFlag = 0 );
protected:
	virtual ME_Result OpenImpl();

	virtual ME_Result CloseImpl();
};

ME_NAME_SPACE_END

#endif	// __SOCKET_UDP_H_




















