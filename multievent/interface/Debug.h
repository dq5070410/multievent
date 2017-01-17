/**
* ����Debug����Լ�һЩtrace���趨
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
* ASSERTE�Ŀ���
*
* �������Ҫ��debug�汾�¹����������ֲ��뿪��assert
* ��ô��һ����ϲ������ĺ� -- �������//ȥ���������Ը��
*
* ���ߣ��������makefile�ﶨ��ME_DISABLE_ASSERTEҲ�ܴﵽ
* ͬ����Ч��
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

	/* ������������Ӧ���ǿ���ͨ���ģ���Ϊ��Platform.h���Ѿ����ƹ��ˣ�������Ϊ�˰�ȫ��������checkһ�� */
	#if !defined( __FUNCTION__ )
		#error trace����ֻ֧��vc6���ϰ汾
	#else
		#define _ME_FUNCTION_ __FUNCTION__
	#endif	// !defined( __FUNCTION__ )
#endif	// ME_WIN

#ifdef ME_LINUX
	#include <assert.h>

	/* ��Ҫ��ME_ASSERTE��Ч���������makefile��߶��壺ME_DEBUG */
	#if defined( ME_DEBUG ) && !defined( ME_DISABLE_ASSERTE )
		#define ME_ASSERTE assert
	#endif // ME_DEBUG

	//#if !defined( __PRETTY_FUNCTION__ )
	//	#error trace����ֻ֧��gcc 4.4���ϰ汾
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
	* ��Asserte�رյ������, ���е�ME_ASSERTE�ڱ��ʽ������ʱ, ��ӡһ������
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
	* ��Asserte�رյ������, �����ʽ������ʱ, ��ӡһ�������ҷ���<result>
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
	* ��Asserte�رյ������, �����ʽ������ʱ, ��ӡһ�������ҷ���
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
	* ��Asserte�����������, �����ʽ������ʱ, asserte����, ֮�󷵻�<result>
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
	* ��Asserte�����������, �����ʽ������ʱ, asserte����, ֮�󷵻�
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
* ���ʹ��trace
* 
* ����:
*
* int i = 100;
* DWORD dw = 78;
* CObject* pObject = new CObject;
*
* ME_INFO_TRACE_THIS( "Test trace: "
*					   << ", i = " << i
*					   << ", dw = " <<��dw
*					   << ", pObject = " << pObject );
*
* ����������:
* [07/06/2010 15:36:08.812 pid=3076 tid=3196] [Info] Test trace: i = 100, dw = 78, pObject = 0x00ff4de6 this = 0x0012fb3f
* 
* 
* ����trace������ͬ
*
* ע��:
* ��ʹ�ô�"THIS"��β��trace��ʱ����ȷ���ú괦��ĳ������ĳ�Ա��������, ����thisָ��������
* ��ӡthis���������Ԥ֪�ĺ��
*/

/**
* �����debug�汾, ��ô��ӡtrace��debugview; �����release�汾, ��ô��ӡtrace���ļ�
* ��<data>�ĳ��ȳ�����������ֵʱ, <data>�ᱻ�ض��������ȷ�Χ֮��
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
		* ����HikUtility���е���ʵ�ֶ�����HIK�£����Ϻ�չ�����ص㣬����gcc�޷���ȷʶ��
		* (vc������û���������)������ֻ�ܰ������HIK��CMETraceFormatorǰȥ��������gcc
		* ����ΪCMETraceFormator����HIK::HIK��������ռ���(��ʵ����HIK::��)
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

/* ������Ϣ */
#define ME_TRACE_LEVEL_INFO	0

/* ������Ϣ */
#define ME_TRACE_LEVEL_WARNING	1

/* ������Ϣ */
#define ME_TRACE_LEVEL_ERROR	2

/* ��ӡ������Ϣ, ��Ϣ��󳤶���Ҫ���� */
#define ME_INFO_TRACE( data ) /*lint -save -e665 -e534*/ ME_WRITE_TRACE_LOG( ME_TRACE_LEVEL_INFO, data )	/*lint -restore*/

/* ��ӡ������Ϣ, ��Ϣ��󳤶���Ҫ���� */
#define ME_WARNING_TRACE( data ) /*lint -save -e665 -e534*/ ME_WRITE_TRACE_LOG( ME_TRACE_LEVEL_WARNING, data )	/*lint -restore*/

/* ��ӡ������Ϣ, ��Ϣ��󳤶���Ҫ���� */
#define ME_ERROR_TRACE( data ) /*lint -save -e665 -e534*/ ME_WRITE_TRACE_LOG( ME_TRACE_LEVEL_ERROR, data )	/*lint -restore*/

/** 
* ׷��thisָ����Ϣ��trace�� 
*/

/* ��ӡ������Ϣ����β������thisָ���ֵ, ��Ϣ��󳤶���Ҫ���� */
#define ME_INFO_TRACE_THIS( data ) /*lint -save -e665 -e534*/ ME_INFO_TRACE( data << " this = " << this )	/*lint -restore*/

/* ��ӡ������Ϣ����β������thisָ���ֵ, ��Ϣ��󳤶���Ҫ���� */
#define ME_WARNING_TRACE_THIS( data ) /*lint -save -e665 -e534*/ ME_WARNING_TRACE( data << " this = " << this )	/*lint -restore*/

/* ��ӡ������Ϣ����β������thisָ���ֵ, ��Ϣ��󳤶���Ҫ���� */
#define ME_ERROR_TRACE_THIS( data ) /*lint -save -e665 -e534*/ ME_ERROR_TRACE( data << " this = " << this )	/*lint -restore*/

#endif	// __DEBUG_H_





