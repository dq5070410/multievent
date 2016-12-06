/**
*
* 
* History
*	12/02/2012		Created
*/

#ifndef __ERROR_DEFINES_H_
#define __ERROR_DEFINES_H_

/**
* this file is user self define error,you can see sys error at
* "HikErrorSysDefines.h"
*
* Ϊ�˱���32/64λ�����ţ���������ò���long���͵�
* �Ƽ�ʹ��int
*/

/* �����Ĵ������ͣ���Χ10000 ~ 20000 */
#define ERROR_BASE					10000

/* ����������ͣ���Χ20000 ~ 30000 */
#define ERROR_NETWORK				20000

/* �������ӵĴ������Ϳ������±߶��壬��30000��ʼ */

enum
{
	/* �ɹ� */
	R_OK							= (int)0,

	/* ʧ�� */
	R_ERROR_FAILURE					= (int)(R_ERROR_BASE + 1),

	/* û�г�ʼ�� */
	R_ERROR_NOT_INITIALIZED			= (int)(R_ERROR_BASE + 2),

	/* �Ѿ���ʼ�� */
	R_ERROR_ALREADY_INITIALIZED		= (int)(R_ERROR_BASE + 3),

	/* û��ʵ�� */
	R_ERROR_NOT_IMPLEMENTED			= (int)(R_ERROR_BASE + 4),

	/* ��ָ�� */
	R_ERROR_NULL_POINTER			= (int)(R_ERROR_BASE + 5),

	/* ����Ԥ�ڵĴ��� */
	R_ERROR_UNEXPECTED				= (int)(R_ERROR_BASE + 6),

	/* �ڴ治�� */
	R_ERROR_OUT_OF_MEMORY			= (int)(R_ERROR_BASE + 7),

	/* �Ƿ����� */
	R_ERROR_INVALID_ARG				= (int)(R_ERROR_BASE + 8),

	/* ��ǰ���������� */
	R_ERROR_NOT_AVAILABLE			= (int)(R_ERROR_BASE + 9),

	/* ����޷��������� */
	R_ERROR_WOULD_BLOCK				= (int)(R_ERROR_BASE + 10),

	/* û���ҵ��򲻴��� */
	R_ERROR_NOT_FOUND				= (int)(R_ERROR_BASE + 11),

	/* �Ѿ����� */
	R_ERROR_FOUND					= (int)(R_ERROR_BASE + 12),

	/* ��ʱ */
	R_ERROR_TIMEOUT					= (int)(R_ERROR_BASE + 13),

    /* �ﵽ��� */
    R_ERROR_REACH_MAX               = (int)(R_ERROR_BASE + 14),

	/* ������е��޷������ɹ� */
	R_ERROR_IO_PENDING				= (int)(R_ERROR_BASE + 15),
};


#endif	// __ERROR_DEFINES_H_




