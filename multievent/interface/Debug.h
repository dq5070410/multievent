/**
* 各种Debug相关以及一些trace的设定
* 
*
* History
*	11/26/2016		Created
*/

#ifndef __DEBUG_H_
#define __DEBUG_H_

#include "Platform.h"
#include "UtilDefines.h"
#include "TypeDefines.h"

/**
* ASSERTE的开关
*
* 如果你需要在debug版本下工作，但是又不想开启assert
* 那么你一定会喜欢下面的宏 -- 把下面的//去掉你就能如愿了
*
* 或者，你可以在makefile里定义ME_DISABLE_ASSERTE也能达到
* 同样的效果
*/
#ifndef ME_DISABLE_ASSERTE
	#define ME_DISABLE_ASSERTE
	#define ME_DISABLE_TRACE
#endif	// ME_DISABLE_ASSERTE

#ifdef ME_WIN
	#include <crtdbg.h>
	#ifdef _DEBUG
		#define ME_DEBUG
	#endif // _DEBUG

	#if defined( ME_DEBUG ) && !defined( ME_DISABLE_ASSERTE )
		#define ME_ASSERTE _ASSERTE
	#endif // ( ME_DEBUG ) && !defined( ME_DISABLE_ASSERTE )

	/* 理论上这个检查应该是可以通过的，因为在Platform.h上已经限制过了，不过，为了安全，还是再check一下 */
	#if !defined( __FUNCTION__ )
		#error trace工具只支持vc6以上版本
	#else
		#define _ME_FUNCTION_ __FUNCTION__
	#endif	// !defined( __FUNCTION__ )
#endif	// ME_WIN

#ifdef ME_LINUX
	#include <assert.h>

	/* 想要让ME_ASSERTE生效，就在你的makefile里边定义：ME_DEBUG */
	#if defined( ME_DEBUG ) && !defined( ME_DISABLE_ASSERTE )
		#define ME_ASSERTE assert
	#endif // ME_DEBUG

	//#if !defined( __PRETTY_FUNCTION__ )
	//	#error trace工具只支持gcc 4.4以上版本
	//#else
		#define _ME_FUNCTION_ __PRETTY_FUNCTION__
	//#endif	// !defined( __PRETTY_FUNCTION__ )
#endif // ME_UNIX

#ifndef ME_ASSERTE
	#define ME_ASSERTE
#endif	// ME_ASSERTE

#ifdef ME_DISABLE_ASSERTE
	// TODO
	#ifdef ME_ASSERTE
		#undef ME_ASSERTE
	#endif

	/**
	* 当Asserte关闭的情况下, 所有的ME_ASSERTE在表达式不成立时, 打印一个错误
	*/
	#define ME_ASSERTE( expr )																	\
	do																							\
	{																							\
		if ( !(expr) )																			\
		{																						\
			ME_ERROR_TRACE( __FILE__ << " : " << __LINE__ << " Assert failed: " << #expr );	\
		}																						\
	} while( 0 )

#endif // ME_DISABLE_ASSERTE

#ifdef ME_DISABLE_ASSERTE

	/**
	* 当Asserte关闭的情况下, 当表达式不成立时, 打印一个错误并且返回<result>
	*/
	#define ME_ASSERTE_RETURN( expr, result )													\
	do																							\
	{																							\
		if ( !(expr) )																			\
		{																						\
			ME_ERROR_TRACE( __FILE__ << " : " << __LINE__ << " Assert failed: " << #expr );	\
			return result;																		\
		}																						\
	} while( 0 )

	/**
	* 当Asserte关闭的情况下, 当表达式不成立时, 打印一个错误并且返回
	*/
	#define ME_ASSERTE_RETURN_VOID( expr )														\
	do																							\
	{																							\
		if ( !(expr) )																			\
		{																						\
			ME_ERROR_TRACE( __FILE__ << " : " << __LINE__ << " Assert failed: " << #expr );	\
			return;																				\
		}																						\
	} while (0)

#else	// ME_DISABLE_ASSERTE
	/**
	* 当Asserte开启的情况下, 当表达式不成立时, asserte出来, 之后返回<result>
	*/
	#define ME_ASSERTE_RETURN( expr, result )	/*lint -save -e665 -e534*/						\
	do																							\
	{																							\
		ME_ASSERTE( (expr) );																	\
		if ( !(expr) )																			\
		{																						\
			ME_ERROR_TRACE( __FILE__ << " : " << __LINE__ << " Assert failed: " << #expr );	\
			return result;																		\
		}																						\
	} while( 0 )	/*lint -restore*/

	/**
	* 当Asserte开启的情况下, 当表达式不成立时, asserte出来, 之后返回
	*/
	#define ME_ASSERTE_RETURN_VOID( expr )	/*lint -save -e665 -e534*/							\
	do																							\
	{																							\
		ME_ASSERTE( (expr) );																	\
		if ( !(expr) )																			\
		{																						\
			ME_ERROR_TRACE( __FILE__ << " : " << __LINE__ << " Assert failed: " << #expr );	\
			return;																				\
		}																						\
	} while( 0 )	/*lint -restore*/

#endif // ME_DISABLE_ASSERTE

/**
* 如何使用trace
* 
* 例子:
*
* int i = 100;
* DWORD dw = 78;
* CObject* pObject = new CObject;
*
* ME_INFO_TRACE_THIS( "Test trace: "
*					   << ", i = " << i
*					   << ", dw = " <<　dw
*					   << ", pObject = " << pObject );
*
* 最终输出结果:
* [07/06/2010 15:36:08.812 pid=3076 tid=3196] [Info] Test trace: i = 100, dw = 78, pObject = 0x00ff4de6 this = 0x0012fb3f
* 
* 
* 其他trace宏与上同
*
* 注意:
* 在使用带"THIS"结尾的trace宏时必须确保该宏处于某个对象的成员函数当中, 在无this指针的情况下
* 打印this会带来不可预知的后果
*/

/**
* 如果是debug版本, 那么打印trace到debugview; 如果是release版本, 那么打印trace到文件
* 当<data>的长度长于允许的最大值时, <data>会被截断至允许长度范围之内
*/

#ifndef ME_DISABLE_TRACE
	#if defined( ME_DEBUG ) && !defined( ME_LINUX )
		#define ME_WRITE_TRACE_LOG( level, data )	/*lint -save -e665 -e534*/					\
		do																						\
		{																						\
			char pszFormatBuf[ME_TRACE_AVAILABLE_DATA_LEN];									\
			::memset( pszFormatBuf, 0, ME_TRACE_AVAILABLE_DATA_LEN );							\
			HIK::CMETraceFormator htfFormator( pszFormatBuf, ME_TRACE_AVAILABLE_DATA_LEN );	\
			htfFormator << _ME_FUNCTION_ << ME_TRACE_FUNCTION_EXT << data;					\
			HIK::CMETrace::Instance()->Output( level, htfFormator );							\
		} while( 0 );	/*lint -restore*/

	#else	// ME_DEBUG
		/** 
		* 由于HikUtility所有的类实现都放在HIK下，加上宏展开的特点，导致gcc无法正确识别
		* (vc编译器没有这个问题)，所以只能把下面的HIK从CMETraceFormator前去掉，否则gcc
		* 会认为CMETraceFormator是在HIK::HIK这个命名空间里(事实上是HIK::里)
		*/
		#define ME_WRITE_TRACE_LOG( level, data )	/*lint -save -e665 -e534*/					\
		do																						\
		{																						\
			char pszFormatBuf[ME_TRACE_AVAILABLE_DATA_LEN];									\
			::memset( pszFormatBuf, 0, ME_TRACE_AVAILABLE_DATA_LEN );							\
			CMETraceFormator htfFormator( pszFormatBuf, ME_TRACE_AVAILABLE_DATA_LEN );		\
			htfFormator << _ME_FUNCTION_ << ME_TRACE_FUNCTION_EXT << data;					\
			CMETrace::Instance()->WriteLog( level, htfFormator );								\
		} while( 0 );	/*lint -restore*/
	#endif	// ME_DEBUG

#else
	#define ME_WRITE_TRACE_LOG( level, data )
#endif	// ME_DISABLE_TRACE

/* 正常信息 */
#define ME_TRACE_LEVEL_INFO	0

/* 警告信息 */
#define ME_TRACE_LEVEL_WARNING	1

/* 错误信息 */
#define ME_TRACE_LEVEL_ERROR	2

/* 打印正常信息, 信息最大长度需要定义 */
#define ME_INFO_TRACE( data ) /*lint -save -e665 -e534*/ ME_WRITE_TRACE_LOG( ME_TRACE_LEVEL_INFO, data )	/*lint -restore*/

/* 打印警告信息, 信息最大长度需要定义 */
#define ME_WARNING_TRACE( data ) /*lint -save -e665 -e534*/ ME_WRITE_TRACE_LOG( ME_TRACE_LEVEL_WARNING, data )	/*lint -restore*/

/* 打印错误信息, 信息最大长度需要定义 */
#define ME_ERROR_TRACE( data ) /*lint -save -e665 -e534*/ ME_WRITE_TRACE_LOG( ME_TRACE_LEVEL_ERROR, data )	/*lint -restore*/

/** 
* 追加this指针信息的trace宏 
*/

/* 打印正常信息并在尾部加上this指针的值, 信息最大长度需要定义 */
#define ME_INFO_TRACE_THIS( data ) /*lint -save -e665 -e534*/ ME_INFO_TRACE( data << " this = " << this )	/*lint -restore*/

/* 打印警告信息并在尾部加上this指针的值, 信息最大长度需要定义 */
#define ME_WARNING_TRACE_THIS( data ) /*lint -save -e665 -e534*/ ME_WARNING_TRACE( data << " this = " << this )	/*lint -restore*/

/* 打印错误信息并在尾部加上this指针的值, 信息最大长度需要定义 */
#define ME_ERROR_TRACE_THIS( data ) /*lint -save -e665 -e534*/ ME_ERROR_TRACE( data << " this = " << this )	/*lint -restore*/

#endif	// __DEBUG_H_





