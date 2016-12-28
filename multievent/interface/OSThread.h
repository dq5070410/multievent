/**
* �߳����API
* 
*
* History
*	12/05/2016		Created
*/

#ifndef __OS_THREAD_H_
#define __OS_THREAD_H_

#include "Base.h"

NAME_SPACE_BEGIN
class CTimeValue;
NAME_SPACE_END

NAME_SPACE_OS_BEGIN

ME_Result OS_EXPORT ThreadCreate(
	ME_THREAD_FUNC pFunc,
	LPVOID pArgs,
	UINT lFlags,
	ME_THREAD_ID& htiThreadID,
	ME_THREAD_HANDLE* hThreadHandle = 0,
	LONG lPriority = 0,
	LPVOID pStack = NULL,
	DWORD dwStacksize = 0 );

/**
* win32��������������ָ���߳�
* linux���������߳���Ϊcancel״̬(�������������̶߳�ֱ����һ��cancel��)
*
* ��win32�����е�����������������ȷ����֪��Ҫ��ʲô�Լ�����������ܻ�����
* ��һЩ����Ԥ֪�ĺ��(�����ڴ�й©�����й©)������㲻ȷ������ô��Ҫ����
* �������
*
* ��linux�����У��ڿ��ܵ���ͬ���������ͬʱ������һ����Ҫע����ǣ�����߳�
* ��������ѭ������֮�������£����ܻᵼ���̳߳ٳ��޷��˳�����Ϊ��cancel��
* �޷����Ϊ�˱���������⣬����Ҫ�����ѭ�������ThreadTestDestroy����ȷ��
* ���߳��ܹ�˳���ĵ���cancel��
*/
ME_Result OS_EXPORT ThreadDestroy(
	ME_THREAD_HANDLE hThreadHandle, 
	DWORD dwExitCode );

/**
* ����һ��cancel��
*
* win32�����£�������������κδ���
* linux�����£�������һcancellation point���������̴߳���cancel״̬ʱ��
* �����ܹ�˳�����˳�(��Ȼ��Ҳ����Ҫ����һ��ʱ��)
*/
ME_Result OS_EXPORT ThreadTestDestroy();

ME_Result OS_EXPORT ThreadJoin( 
	ME_THREAD_HANDLE hThreadHandle,
	CHikTimeValue* pTimeout = NULL,
	ME_THREAD_STAT* pStat = NULL );

ME_Result OS_EXPORT ThreadSetPriority(
	ME_THREAD_HANDLE hThreadHandle,
	int iPriority );

/**
* ��ȡ��ǰ�̵߳�ID
*/
ME_THREAD_ID OS_EXPORT GetThreadID();

/**
* ��õ�ǰ�̵߳ľ��
*
* ��linux�����£�ThreadHandle()��ThreadID()�Ľ����һ����
* ��Ϊlinuxû�о��һ˵
*
* ��win32�����£����ǲ�ͬ
*/
ME_THREAD_HANDLE OS_EXPORT GetThreadHandle();

ME_NAME_SPACE_OS_END

#endif	// __ME_OS_THREAD_H_



