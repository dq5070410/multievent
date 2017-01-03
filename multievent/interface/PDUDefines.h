/**
* ��PDU��ص�һЩ����
* 
*
* History
*	05/07/2016		Created
*/

#ifndef __PDU_DEFINES_H_
#define __PDU_DEFINES_H_

ME_NAME_SPACE_BEGIN

enum PDUTYPE
{
	PDUTYPE_BASE = 100,

	/* ��������PDU */
	PDUTYPE_KEEPALIVE_REQ = PDUTYPE_BASE + 1,

	/* ������ӦPDU */
	PDUTYPE_KEEPALIVE_RSP = PDUTYPE_BASE + 2,

	PDUTYPE_MAX = 200,
};

ME_NAME_SPACE_END

#endif	// __PDU_DEFINES_H_
