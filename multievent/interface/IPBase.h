/**
* IP基础类，所有的socket类都应该从这里继承
* 
*
* History
*	12/08/2016		Created
*/

#ifndef __IP_BASE_H_
#define __IP_BASE_H_

#include "Base.h"
#include "IO.h"

NAME_SPACE_BEGIN

/**
* IP基础类，所有的socket应该类都从这里继承
*/
class ME_OS_EXPORT CIPBase : public IMEIO
{
public:
	/* 非阻塞选项 */
	enum
	{
		/**
		* 其他option接着往下写
		*
		* 目前，有NON_BLOCK就够了，其他以后再补
		*/
		NON_BLOCK,
	};

public:
	virtual ~CIPBase();

public:
	/**
	* 设置句柄
	*
	* <hHandle>, 要设置的句柄
	*
	* 返回值:
	*		ME_OK为成功
	*/
	virtual void SetHandle( ME_HANDLE hHandle );

	/**
	* 获取句柄
	*
	* 返回值:
	*		不为NULL且不为ME_INVALID_HANDLE为成功
	*/
	virtual ME_HANDLE GetHandle() const;

public:

	/**
	* 开启某个I/O模式, 目前, 只能设置阻塞/非阻塞两种
	*
	* <iValue>, 要开启什么模式
	*
	* 返回值:
	*		ME_OK为成功
	*/
	ME_Result Enable( int iValue );

	/**
	* 关闭的某个I/O模式, 目前, 只能设置阻塞/非阻塞两种
	*
	* <iValue>, 要关闭什么模式
	*
	* 返回值:
	*		ME_OK为成功
	*/
	ME_Result Disable( int iValue );

	/**
	* 设置某个I/O模式
	*
	* <iCmd>, 要设置什么模式
	* <pArg>, 对应<iCmd>的值
	*
	* 返回值:
	*		ME_OK为成功
	*/
	ME_Result Control( 
		int iCmd,
		void* pArg );

protected:
	CIPBase();

	CIPBase( ME_HANDLE hHandle );

protected:
	ME_HANDLE						m_hHandle;
};

NAME_SPACE_END

#endif	// __ME_IP_BASE_H_








