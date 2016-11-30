/**
* platform(system call and complier) header
* 
* Platform.h
*
*
* History
*	12/02/2012		Created
*/

#ifndef __PLATFORM_H_
#define __PLATFORM_H_

/**
* windows 
*
* only support winxp and above
*/
#if (defined(WIN32) || defined(WIN64)) && !defined(_WINDOWS)
	#define _WINDOWS
#endif	// (defined(WIN32) || defined(WIN64)) && !defined(_WINDOWS)

#ifdef _WINDOWS
	#define WIN
#endif	// _WINDOWS

/**
* linux
*/
#ifdef __linux
	#ifndef LINUX
		#define LINUX
	#endif	// LINUX

	#if defined( __i386__ )
		#define LINUX32
	#elif defined( __x86_64__ )
		#define LINUX64
	#endif	// __i386__
#endif	// __linux

/**
* only support win and linux
*/
#if ( !defined(WIN) && !defined(LINUX) )
	#error only support win and linux
#endif	// ( !defined(WIN) && !defined(LINUX) )

/**
* win32 complier only support VC,not Boland C++ and so on
*/
#ifdef WIN
	#if ( !defined(_MSC_VER) || (_MSC_VER < 1400) )
		#error only support VC2005 and beyond
	#endif	// _MSC_VER

	/*need multithread runtime lib */
	#ifndef _MT
		#error need multithread runtime lib
	#endif

	#pragma warning( disable: 4786 )	// 超过255个字符(字节)被截断的警告(identifier was truncated to '255' characters in the browser information)
	#pragma warning( disable: 4355 )	// 屏蔽在类的构造函数中使用this的警告(Disable 'this' used in base member initializer list)
	#pragma warning( disable: 4275 )	// 输出类里包含非输出类
	#pragma warning( disable: 4251 )	// 在输出类中使用非输出类的东西
	#pragma warning( disable: 4005 )	// 忽略可能存在重定义
#endif	// HIK_WIN

/**
* default use epoll on linux
* if you not need,define NO_EPOLL to disable check
*/
#ifndef NO_EPOOL

	/* if use epoll,kernel is 2.6.x and above */
	#ifdef LINUX
		#include <linux/version.h>
		#if ( !defined(LINUX_VERSION_CODE) || !defined(KERNEL_VERSION) )
			#error not find the file.
		#endif	//  ( !defined(LINUX_VERSION_CODE) || !defined(KERNEL_VERSION) )

		/* limit kernel 2.6.9 and above */
		#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 9) )
			#error kernel above 2.6.9
		#endif	// ( LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 9) )
	#endif	// LINUX
#endif	// NO_EPOOL


/**
* linux gcc 4.4 and above
* 
*/
#ifdef LINUX
	#if ( !defined(__GNUC__) || !defined(__GNUC_MINOR__) )
		#error need gcc  
	#endif	//__GNUC__

	#if( (__GNUC__ < 4) || ((__GNUC__ <= 4) && (__GNUC_MINOR__ < 4)) )
		#error gcc above 4.4
	#endif	// ( (__GNUC__ < 4) || ((__GNUC__ <= 4) && (__GNUC_MINOR__ < 4)) )
#endif	// LINUX

#endif	// __PLATFORM_H_





