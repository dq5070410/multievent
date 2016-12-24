/**
* һЩ���ߺ�Ķ���
* 
*
* History
*	12/02/2012		Created
*/

#ifndef __UTIL_DEFINES_H_
#define __UTIL_DEFINES_H_

/**
* export��import����
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
* �ָ�������
*/
#ifdef ME_WIN
	#define ME_OS_SEPARATE '\\'
#elif defined( ME_LINUX )
	#define ME_OS_SEPARATE '/'
#endif	// ME_WIN

/* ���ĳbit��û�б����� */
#ifndef ME_BIT_ENABLED
	#define ME_BIT_ENABLED( dword, bit ) ( ((dword) & (bit)) != 0 )
#endif	// ME_BIT_ENABLED

/* ���ĳbit֮���λ��û�б����� */
#ifndef ME_BIT_ENABLED_REVERSE
	#define ME_BIT_ENABLED_REVERSE( dword, bit ) ( ME_BIT_ENABLED(dword, ~(bit)) )
#endif	// ME_BIT_ENABLED_REVERSE

/* ���ĳbit��û�б����� */
#ifndef ME_BIT_DISABLED
	#define ME_BIT_DISABLED( dword, bit ) ( ((dword) & (bit)) == 0 )
#endif	// ME_BIT_DISABLED

/* ���ĳbit֮���λ��û�б����� */
#ifndef ME_BIT_DISABLED_REVERSE
#define ME_BIT_DISABLED_REVERSE( dword, bit ) ( ME_BIT_ENABLED(dword, ~(bit)) == 0 )
#endif	// ME_BIT_ENABLED_REVERSE

/* ���ĳbit��ֵ�Ƿ���<mask>��� */
#ifndef ME_BIT_CMP_MASK
	#define ME_BIT_CMP_MASK( dword, bit, mask ) ( ((dword) & (bit)) == mask )
#endif	// ME_BIT_CMP_MASK

/* ����ĳbit */
#ifndef ME_SET_BITS
	#define ME_SET_BITS( dword, bits ) ( (dword) |= (bits) )
#endif	// ME_SET_BITS

/* ����ĳbit */
#ifndef ME_CLR_BITS
	#define ME_CLR_BITS( dword, bits ) ( (dword) &= ~(bits) )
#endif	// ME_CLR_BITS

/**
* ɾ��һ��ָ�벢��ΪNULL
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
* ɾ��һ��ָ�����鲢��ΪNULL
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
	/* �жϵ�ǰֵ�Ƿ�Ϊ��ʧ�ܡ� */
	#define ME_FAILED( nResult ) ( nResult != ME_OK )
#endif	// ME_FAILED

#ifndef ME_SUCCEEDED
	/* �жϵ�ǰֵ�Ƿ�Ϊ���ɹ��� */
	#define ME_SUCCEEDED( nResult ) ( nResult == ME_OK )
#endif	// ME_SUCCEEDED

/**
* MultiEvent��ʹ�õ������ռ�
* 
* ͬ�������������������ռ������ã�
* ֻҪ����ME_NO_NAME_SPACE����
*/
#ifndef ME_NO_NAME_SPACE

	/* MultiEvent���������ռ�HIK��Ԥ���� */
	namespace HIK {}

	/* MultiEvent�μ������ռ�OS��Ԥ���� */
	namespace HIK { namespace OS {} }

    /* MultiEvent�μ������ռ�CRYPTO��Ԥ���� */
    namespace HIK { namespace CRYPTO {} }

	/* MultiEvent���õĶ��������ռ� HIK */
	#define ME_NAME_SPACE_BEGIN namespace HIK {
	#define ME_NAME_SPACE_END }

	/* MultiEvent��װ��ϵͳ��ص�C APIʹ�õ������ռ� */
	#define ME_NAME_SPACE_OS_BEGIN namespace HIK { namespace OS {
	#define ME_NAME_SPACE_OS_END } }

    /* MultiEvent��װ��SSL���ӽ����Լ�ժҪ��ص�ģ��ʹ�õ������ռ� */
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


