/**
*
* 
* History
*	12/02/2016		Created
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
#define ME_ERROR_BASE					10000

/* ����������ͣ���Χ20000 ~ 30000 */
#define ME_ERROR_NETWORK				20000

/* �������ӵĴ������Ϳ������±߶��壬��30000��ʼ */

enum
{
	/* �ɹ� */
	ME_OK							    = (int)0,

	/* ʧ�� */
	ME_ERROR_FAILURE					= (int)(ME_ERROR_BASE + 1),

	/* û�г�ʼ�� */
	ME_ERROR_NOT_INITIALIZED			= (int)(ME_ERROR_BASE + 2),

	/* �Ѿ���ʼ�� */
	ME_ERROR_ALREADY_INITIALIZED		= (int)(ME_ERROR_BASE + 3),

	/* û��ʵ�� */
	ME_ERROR_NOT_IMPLEMENTED			= (int)(ME_ERROR_BASE + 4),

	/* ��ָ�� */
	ME_ERROR_NULL_POINTER			    = (int)(ME_ERROR_BASE + 5),

	/* ����Ԥ�ڵĴ��� */
	ME_ERROR_UNEXPECTED				    = (int)(ME_ERROR_BASE + 6),

	/* �ڴ治�� */
	ME_ERROR_OUT_OF_MEMORY			    = (int)(ME_ERROR_BASE + 7),

	/* �Ƿ����� */
	ME_ERROR_INVALID_ARG				= (int)(ME_ERROR_BASE + 8),

	/* ��ǰ���������� */
	ME_ERROR_NOT_AVAILABLE			    = (int)(ME_ERROR_BASE + 9),

	/* ����޷��������� */
	ME_ERROR_WOULD_BLOCK				= (int)(ME_ERROR_BASE + 10),

	/* û���ҵ��򲻴��� */
	ME_ERROR_NOT_FOUND				    = (int)(ME_ERROR_BASE + 11),

	/* �Ѿ����� */
	ME_ERROR_FOUND					    = (int)(ME_ERROR_BASE + 12),

	/* ��ʱ */
	ME_ERROR_TIMEOUT					= (int)(ME_ERROR_BASE + 13),

    /* �ﵽ��� */
    ME_ERROR_REACH_MAX                  = (int)(ME_ERROR_BASE + 14),

	/* ������е��޷������ɹ� */
	ME_ERROR_IO_PENDING				    = (int)(ME_ERROR_BASE + 15),
};


#endif	// __ERROR_DEFINES_H_




