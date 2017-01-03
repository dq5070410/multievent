/**
* trace������ĸ��ֶ���
* 
*
* History
*	03/20/2016		Created
*/

#ifndef __TRACE_DEFINES_H_
#define __TRACE_DEFINES_H_

#include "Platform.h"
#include "UtilDefines.h"
#include "TypeDefines.h"

ME_NAME_SPACE_BEGIN

/* trace���ݵ���󳤶� */
const DWORD ME_TRACE_MAX_TRACE_LEN				= 1024;

/* trace��ʽ�����ֱ������� */
const DWORD ME_TRACE_RESERVE_DATA_LEN			= 64;

/* ʵ������trace����󳤶� */
const DWORD ME_TRACE_AVAILABLE_DATA_LEN		= ME_TRACE_MAX_TRACE_LEN - ME_TRACE_RESERVE_DATA_LEN;

/* �ַ�������󳤶�*/
const DWORD ME_TRACE_CHAR_MAX_LEN				= 255;

#ifdef ME_WIN
const char ME_TRACE_FUNCTION_EXT[]				= "(), ";
#else
const char ME_TRACE_FUNCTION_EXT[]				= ", ";
#endif

const char ME_TRACE_CURRENT_LINE_MARK[]		= "************************* Current trace line *************************\n\n";
const char ME_TRACE_FORMAT[]					= "[%02d/%02d/%04d %02d:%02d:%02d.%03ld pid=%d tid=%d] %s %s\n";

ME_NAME_SPACE_END

#endif	// __TRACE_DEFINES_H_

