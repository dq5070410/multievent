/**
* 底层socket的基础封装类
* 所有的socket类都应该从这里继承
* 
*
* History
*	12/12/2016		Created
*/

#ifndef __SOCKET_BASE_H_
#define __SOCKET_BASE_H_

#include "Base.h"
#include "IPBase.h"
#include "SocketErrorAutoSave.h"

ME_NAME_SPACE_BEGIN
class CMEInetAddress;
ME_NAME_SPACE_END

ME_NAME_SPACE_BEGIN

/**
* 底层socket的基础封装类
* 所有的socket类都应该从这里继承
*/
class ME_OS_EXPORT CMESocketBase : public CIPBase
{
public:
	virtual ~CMESocketBase();

public:
	/** 
	* 尝试打开一个socket
	* 
	* <bReuse>, 是否允许地址重用
	*
	* 返回值:
	*		SOCKET_ERROR是失败; ME_OK是成功
	*/
	ME_Result Open( BOOL bReuse = FALSE );

	/** 
	* 尝试打开一个socket, 并绑定到一个本地地址
	* 
	* <hiaLocalAddr>, 需要绑定的地址
	* <bReuse>, 是否允许地址重用
	*
	* 返回值:
	*		ME_OK是成功, 其他为失败
	*/
	ME_Result Open( 
		const CMEInetAddress& hiaLocalAddr,
		BOOL bReuse = FALSE );

	/** 
	* 进入监听状态
	* 
	* <hiaLocalAddr>, 需要监听的地址
	*
	* 返回值:
	*		ME_OK是成功, 其他为失败
	*/
	ME_Result Listen( const CMEInetAddress& hiaLocalAddr );

	/** 
	* 接受对端接入
	*
	* <hHandle>, 对端连接句柄
	* <hiaPeerAddr>, 对端的地址
	*
	* 返回值:
	*		ME_OK是成功, 其他为失败
	*/
	ME_Result Accept(
		ME_HANDLE& hHandle,
		CMEInetAddress& hiaPeerAddr );

	/**
	* 尝试关闭一个socket并把<m_hhHandle>置为ME_INVALID_HANDLE
	*
	* 返回值:
	*		ME_OK是成功, 其他为失败
	*/
	ME_Result Close();

	/** 
	* 发送数据
	* 
	* <pszData>, 要发送的数据
	* <dwLength>, 要发送数据的长度
	* <iFlag>, 标记
	*
	* 返回值:
	*		返回本次调用发送的字节数数量
	*/
	int Send(
		const char* pszData,
		DWORD dwLength,
		int iFlag = 0 );

	/** 
	* 发送数据
	* 
	* <pszIov>, 要发送的数据数组
	* <dwCount>, 要发送的数据数组的个数
	*
	* 返回值:
	*		返回本次调用发送的字节数数量
	*/
	int SendV(
		const iovec pszIov[],
		DWORD dwCount );

	/** 
	* 收取数据
	* 
	* <pszBuf>, 收取数据的缓冲区
	* <dwLength>, 收取数据的缓冲区大小
	* <iFlag>, 标记
	*
	* 返回值:
	*		返回本次收取的字节数数量
	*/
	int Recv(
		char*& pszBuf,
		DWORD dwLength,
		int iFlag = 0 );

	/** 
	* 发送数据
	* 
	* <pszIov>, 收取数据缓冲区数组
	* <dwCount>, 收取数据缓冲区数组的个数
	*
	* 返回值:
	*		返回本次收取的字节数数量
	*/
	int RecvV(
		iovec szIov[],
		DWORD dwCount );

	/** 
	* 设置socket的属性
	* 
	* <iLevel>, 要设置的这个属性被定于哪一个层次
	* <iOption>, 要设置的属性
	* <pOptval>, 设置<iOption>属性的值
	* <iOptlen>, 属性值<pOptval>的长度是多少
	*
	* 返回值:
	*		SOCKET_ERROR是失败; ME_OK是成功
	*/
	ME_Result SetOption(
		int iLevel, 
		int iOption, 
		const void* pOptval, 
		int iOptlen );

	/** 
	* 获取socket的属性
	* 
	* <iLevel>, 要获取的这个属性被定于哪一个层次
	* <iOption>, 要获取的属性
	* <pOptval>, 设置<iOption>属性的值
	* <iOptlen>, 属性值<pOptval>的长度是多少
	*
	* 返回值:
	*		SOCKET_ERROR是失败; ME_OK是成功
	*/
	ME_Result GetOption(
		int iLevel, 
		int iOption, 
		void* pOptval, 
		int* pOptlen );

	/** 
	* 获取一个已有连接中所绑定的本地地址
	* 
	* <hiaLocalAddr>, out, 被绑定的地址
	*
	* 返回值:
	*		SOCKET_ERROR是失败; ME_OK是成功
	*/
	ME_Result GetLoacalAddress( CMEInetAddress& hiaLocalAddr );

	/** 
	* 获取一个已有连接中所绑定的对端地址
	* 
	* <hiaRemoteAddress>, out, 对端地址
	*
	* 返回值:
	*		SOCKET_ERROR是失败; ME_OK是成功
	*/
	ME_Result GetRemoteAddress( CMEInetAddress& hiaRemoteAddress );

protected:
	CMESocketBase();

	CMESocketBase( ME_HANDLE hHandle );

protected:
	/**
	* 在基类已有的动作之外，你(继承类)还需要做什么
	*/
	virtual ME_Result OpenImpl() = 0;

	/**
	* 在基类已有的动作之外，你(继承类)还需要做什么
	*/
	virtual ME_Result CloseImpl() = 0;

protected:
	/**
	* 打印当前的socket错误, 在哪个函数里发生的错误, this指针, 以及重置errno
	*/
	void ShowSocketError( int iResult );
};

ME_NAME_SPACE_END

#endif	// __ME_SOCKET_BASE_H_








