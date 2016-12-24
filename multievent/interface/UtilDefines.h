/**
* 一些工具宏的定义
* 
*
* History
*	12/02/2012		Created
*/

#ifndef __UTIL_DEFINES_H_
#define __UTIL_DEFINES_H_

/**
* export和import定义
*/
#ifdef ME_WIN
	#if ( defined(_LIB) || defined(ME_OS_BUILD_LIB) )
		#define ME_OS_EXPORT
	#else 
		#if ( defined(_USRDLL) || defined(ME_OS_BUILD_DLL) )
			#define ME_OS_EXPORT __declspec( dllexport )
		#else
			#define ME_OS_EXPORT __declspec( dllimport )
		#endif	// _USRDLL || ME_OS_BUILD_DLL
	#endif	// _LIB || ME_OS_BUILD_LIB
#else
	#define ME_OS_EXPORT
#endif	// ME_WIN

/**
* 分隔符定义
*/
#ifdef ME_WIN
	#define ME_OS_SEPARATE '\\'
#elif defined( ME_LINUX )
	#define ME_OS_SEPARATE '/'
#endif	// ME_WIN

/* 检查某bit有没有被设置 */
#ifndef ME_BIT_ENABLED
	#define ME_BIT_ENABLED( dword, bit ) ( ((dword) & (bit)) != 0 )
#endif	// ME_BIT_ENABLED

/* 检查某bit之外的位有没有被设置 */
#ifndef ME_BIT_ENABLED_REVERSE
	#define ME_BIT_ENABLED_REVERSE( dword, bit ) ( ME_BIT_ENABLED(dword, ~(bit)) )
#endif	// ME_BIT_ENABLED_REVERSE

/* 检查某bit有没有被屏蔽 */
#ifndef ME_BIT_DISABLED
	#define ME_BIT_DISABLED( dword, bit ) ( ((dword) & (bit)) == 0 )
#endif	// ME_BIT_DISABLED

/* 检查某bit之外的位有没有被设置 */
#ifndef ME_BIT_DISABLED_REVERSE
#define ME_BIT_DISABLED_REVERSE( dword, bit ) ( ME_BIT_ENABLED(dword, ~(bit)) == 0 )
#endif	// ME_BIT_ENABLED_REVERSE

/* 检查某bit的值是否与<mask>相等 */
#ifndef ME_BIT_CMP_MASK
	#define ME_BIT_CMP_MASK( dword, bit, mask ) ( ((dword) & (bit)) == mask )
#endif	// ME_BIT_CMP_MASK

/* 设置某bit */
#ifndef ME_SET_BITS
	#define ME_SET_BITS( dword, bits ) ( (dword) |= (bits) )
#endif	// ME_SET_BITS

/* 屏蔽某bit */
#ifndef ME_CLR_BITS
	#define ME_CLR_BITS( dword, bits ) ( (dword) &= ~(bits) )
#endif	// ME_CLR_BITS

/**
* 删除一个指针并置为NULL
*/
#ifndef ME_DELETE
	#define ME_DELETE( p )			\
	do								\
	{								\
		if ( NULL != p )			\
		{							\
			delete p;				\
			p = NULL;				\
		}							\
	} while ( 0 )
#endif	// ME_DELETE

/**
* 删除一个指针数组并置为NULL
*/
#ifndef ME_ARRAY_DELETE
	#define ME_ARRAY_DELETE( pArray )	\
	do									\
	{									\
		if ( NULL != pArray )			\
		{								\
			delete []pArray;			\
			pArray = NULL;				\
		}								\
	} while( 0 )
#endif	// ME_ARRAY_DELETE

#ifndef ME_FAILED
	/* 判断当前值是否为“失败” */
	#define ME_FAILED( nResult ) ( nResult != ME_OK )
#endif	// ME_FAILED

#ifndef ME_SUCCEEDED
	/* 判断当前值是否为“成功” */
	#define ME_SUCCEEDED( nResult ) ( nResult == ME_OK )
#endif	// ME_SUCCEEDED

/**
* MultiEvent所使用的命名空间
* 
* 同样，如果不想这个命名空间起作用，
* 只要定义ME_NO_NAME_SPACE就行
*/
#ifndef ME_NO_NAME_SPACE

	/* MultiEvent顶层命名空间HIK的预声明 */
	namespace HIK {}

	/* MultiEvent次级命名空间OS的预声明 */
	namespace HIK { namespace OS {} }

    /* MultiEvent次级命名空间CRYPTO的预声明 */
    namespace HIK { namespace CRYPTO {} }

	/* MultiEvent所用的顶层命名空间 HIK */
	#define ME_NAME_SPACE_BEGIN namespace HIK {
	#define ME_NAME_SPACE_END }

	/* MultiEvent封装与系统相关的C API使用的命名空间 */
	#define ME_NAME_SPACE_OS_BEGIN namespace HIK { namespace OS {
	#define ME_NAME_SPACE_OS_END } }

    /* MultiEvent封装与SSL，加解密以及摘要相关的模块使用的命名空间 */
    #define ME_NAME_SPACE_CRYPTO_BEGIN namespace HIK { namespace CRYPTO {
    #define ME_NAME_SPACE_CRYPTO_END } }

	#define ME_USING_HIK using namespace HIK;
	#define ME_USING_ME_OS using namespace HIK::OS;
    #define ME_USING_ME_CRYPTO using namespace HIK::CRYPTO;

#else
	#define ME_NAME_SPACE_BEGIN
	#define ME_NAME_SPACE_END

	#define ME_NAME_SPACE_OS_BEGIN
	#define ME_NAME_SPACE_OS_END

    #define ME_NAME_SPACE_CRYPTO_BEGIN
    #define ME_NAME_SPACE_CRYPTO_END

	#define ME_USING_HIK
	#define ME_USING_ME_OS
    #define ME_USING_ME_CRYPTO
#endif	// ME_NO_NAME_SPACE

#endif	// __UTIL_DEFINES_H_


