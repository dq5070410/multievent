/**
* 网络地址类
* 
*
* History
*	12/11/2012		Created
*/

#ifndef __INET_ADDRESS_H_
#define __INET_ADDRESS_H_

#include "Base.h"

NAME_SPACE_BEGIN

/**
* 网络地址类
*
* 目前只支持IPv4
*/
class OS_EXPORT CInetAddress
{
public:
	CInetAddress();

	/**
	* 构造函数
	*
	* <pszHostNameAndPort>，地址与端口，比如"127.0.0.1:80"
	*/
	CInetAddress( const char* pszHostNameAndPort );

	/**
	* 构造函数
	*
	* <pszHostName>，地址，比如"127.0.0.1"
	* <wPort>，端口，最大不能超过65535
	*/
	CInetAddress( 
		const char* pszHostName,
		WORD wPort );

	/* 2013.9.5 added by 韦珂 */
	CInetAddress( const ME_SOCK_ADDR& hsaAddress );

public:
	/**
	* 获取第一张网卡的第一个有效IP地址
	*
	* <hiaLocalAddress>，获取到的本地地址
	* <bIPv4>，IPv6或IPv4地址，默认是IPv4类型的地址
	* <bLoopback>，是否获取回路地址
	*/
	static ME_Result GetLocalAddress( 
		CInetAddress& hiaLocalAddress,
		BOOL bIPv4 = TRUE,
		BOOL bLoopback = FALSE);

public:

	/**
	* 设置地址和端口
	*
	* <pszHostName>, 地址名
	* <wPort>, 端口
	*
	* 返回值:
	*		ME_OK为成功; 其他为失败
	*/
	ME_Result Set( 
		const char* pszHostName,
		WORD wPort );

	/**
	* 同时设置地址和端口, 如"127.0.0.1:1224"
	* 使用这个方法设置地址时, 必须确保字符串中必须有冒号
	*
	* <pszHostNameAndPort>, 地址名和端口, 如"127.0.0.1:1224"
	* <dwLength>, <pszHostNameAndPort>的长度
	*
	* 返回值:
	*		ME_OK为成功; 其他为失败
	*/
	ME_Result Set( const char* pszHostNameAndPort );

	/**
	* 设置地址
	* 
	* <dwIP>, 数字类型IP地址, 使用这个方法设置时需要确保<dwIP>已经进行高低字节转换
	*
	* 返回值:
	*		ME_OK为成功; 其他为失败	
	*/
	ME_Result Set( DWORD dwIP );

	ME_Result Set( const ME_SOCK_ADDR& hsaAddress );

	/**
	* 设置端口
	*
	* <wPort>, 端口
	*
	* 返回值:
	*		ME_OK为成功; 其他为失败
	*/
	ME_Result SetPort( WORD wPort );

	/**
	* 获取端口
	*
	* 返回值:
	*		WORD值
	*/
	WORD GetPort() const;

	/**
	* 取得当前地址
	*
	* 返回值:
	*		CHikString值
	*/
	CMEString GetIP() const;

	/**
	* 取得原生类地址类型结构大小
	*
	* 返回值:
	*		DWORD值
	*/
	DWORD GetSize() const;

	/**
	* 本地址是否是回路地址
	*
	* WARNING：目前该函数不适用于IPv6
	*
	* 返回值:
	*		TRUE为成功，其他为失败
	*/
	BOOL IsLocalHost() const;

	/**
	* 取得原生类地址指针
	*
	* 返回值:
	*		非NULL为成功; NULL为失败
	*/
	const ME_SOCK_ADDR* GetRawAddress() const;

public:

	/**
	* == 运算符
	* 判断两个CHikInetAddress对象是否相等
	*/
	BOOL operator == ( const CInetAddress& DstAddress ) const;

	/**
	* < 运算符
	* 判断当前CHikInetAddress对象小于目标对象
	* 通常用于std::map中的查找操作
	*/
	BOOL operator < ( const CInetAddress& DstAddress ) const;

public:
	/**
	* 用于hash_map
	*/
	class HashCompare
	{
	public:
		enum
		{	/** 
			* 哈希表需要的两个变量
			* 因为在_Hash这个类中会去traits出比对类 - 也就是HashCompare的<bucket_size>和<min_buckets>
			* 所以这两个变量的名字不能改
			*/
			bucket_size = 4,
			min_buckets = 8
		};

	public:
		/** 
		* 把<Src>中的长整型ip值与一个固定的值进行异或
		*
		* <Src>, 需要异或的对象
		*
		* 返回值:
		*		long允许范围之内的任何值
		*/
		LONG GetHashValue( const CInetAddress& Src ) const;

		/**  
		* 对比两个CHikInetAddress对象, 功能依赖于CHikInetAddress的operator <
		*
		* <Src>, 源对象
		* <Dst>, 目标对象
		*
		* 返回值:
		*		Src小于Dst, 返回TRUE; 否则返回FALSE
		*/
		BOOL operator () (
			const CInetAddress& Src, 
			const CInetAddress& Dst ) const;

		/**  
		* hash目标对象
		* 算法copy自<hash_map>的<hash_compare>
		*
		* <Dst>, 目标对象
		*
		* 返回值:
		*		long允许范围之内的任何值
		*/
		LONG operator() ( const CInetAddress& Dst ) const;
	};

private:
	/**
	* 设置地址
	*
	* <pszIP>, 地址, 不包括端口
	*
	* 返回值:
	*		ME_OK为成功; 其他为失败
	*/
	ME_Result SetIP( const char* pszIP );

	/**
	* 设置地址
	*
	* <dwIP>, 数字类型地址
	* <bIsNetworkOrder>, <dwIP>是否已经被转成网络字节序了?
	*
	* 返回值:
	*		ME_OK为成功; 其他为失败
	*/
	ME_Result SetIP( 
		DWORD dwIP, 
		BOOL bIsNetworkOrder = FALSE );

private:
	ME_SOCK_ADDR					m_SockAddr;	// 系统地址

};

NAME_SPACE_END

#endif	// __INET_ADDRESS_H_



