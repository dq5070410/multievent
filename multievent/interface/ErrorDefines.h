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
* 为了避免32/64位的困扰，错误码最好不用long类型的
* 推荐使用int
*/

/* 基本的错误类型，范围10000 ~ 20000 */
#define ME_ERROR_BASE					10000

/* 网络错误类型，范围20000 ~ 30000 */
#define ME_ERROR_NETWORK				20000

/* 其他增加的错误类型可以在下边定义，从30000开始 */

enum
{
	/* 成功 */
	ME_OK							    = (int)0,

	/* 失败 */
	ME_ERROR_FAILURE					= (int)(ME_ERROR_BASE + 1),

	/* 没有初始化 */
	ME_ERROR_NOT_INITIALIZED			= (int)(ME_ERROR_BASE + 2),

	/* 已经初始化 */
	ME_ERROR_ALREADY_INITIALIZED		= (int)(ME_ERROR_BASE + 3),

	/* 没有实现 */
	ME_ERROR_NOT_IMPLEMENTED			= (int)(ME_ERROR_BASE + 4),

	/* 空指针 */
	ME_ERROR_NULL_POINTER			    = (int)(ME_ERROR_BASE + 5),

	/* 不可预期的错误 */
	ME_ERROR_UNEXPECTED				    = (int)(ME_ERROR_BASE + 6),

	/* 内存不足 */
	ME_ERROR_OUT_OF_MEMORY			    = (int)(ME_ERROR_BASE + 7),

	/* 非法参数 */
	ME_ERROR_INVALID_ARG				= (int)(ME_ERROR_BASE + 8),

	/* 当前操作不允许 */
	ME_ERROR_NOT_AVAILABLE			    = (int)(ME_ERROR_BASE + 9),

	/* 结果无法立即返回 */
	ME_ERROR_WOULD_BLOCK				= (int)(ME_ERROR_BASE + 10),

	/* 没有找到或不存在 */
	ME_ERROR_NOT_FOUND				    = (int)(ME_ERROR_BASE + 11),

	/* 已经存在 */
	ME_ERROR_FOUND					    = (int)(ME_ERROR_BASE + 12),

	/* 超时 */
	ME_ERROR_TIMEOUT					= (int)(ME_ERROR_BASE + 13),

    /* 达到最大 */
    ME_ERROR_REACH_MAX                  = (int)(ME_ERROR_BASE + 14),

	/* 进入队列但无法立即成功 */
	ME_ERROR_IO_PENDING				    = (int)(ME_ERROR_BASE + 15),
};


#endif	// __ERROR_DEFINES_H_




