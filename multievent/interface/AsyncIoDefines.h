/**
* �첽IO�ṹ�Ķ���
* 
*
* History
*	9/04/2016		Created
*/

#ifndef _ASYCH_IO_DEFINES_H_
#define _ASYCH_IO_DEFINES_H_

#include "Base.h"
#include "IO.h"
#include "ReferenceControlT.h"
#include "ConnectionDefines.h"

ME_NAME_SPACE_BEGIN

#ifdef ME_WIN
	extern LPFN_ACCEPTEX g_lpfnAcceptEx;							// ::AcceptEx����ָ��
	extern LPFN_CONNECTEX g_lpfnConnectEx;							// ::ConnectEx����ָ��
	extern LPFN_GETACCEPTEXSOCKADDRS g_lpfnGetAcceptExSockaddrs;	// ::GetAcceptExSockaddrs����ָ��

	const DWORD ME_MS_ACCEPTEX_FUNC_REMAIN_LENGTH = 16;			// ::AcceptEx������Ҫ�ĵ�ַ�������ȣ����MSDN
#endif	// ME_WIN
/**
* HikUtility�첽IOʹ�õ����ݽṹ
* 
* �̳���OVERLAPPED����������һЩ��Ҫ��������
*/
typedef struct tagHikAsyncIoInfo : public ME_OVERLAPPED
{
	int iIoType;			// ����IO������
	ME_HANDLE hHandle;		// IO������ؾ��
} ME_ASYCH_IO_INFO;

/**
* �����첽IO��������(Ͷ��/���/֪ͨ���첽�ģ������������)
* Ϊȷ���ص�����İ�ȫ������ص���������ôһ���࣬��ԭ�е�IMEIOSink
* �Ļ����ϼ��������ü���(�̳���CMEReferenceControlT)
*
* Ҳ������Ϊ���������ü������ƣ���̳����ʹ����Ҫע�� :
* ��Ҫֱ����delete
*/
class IMEAsyncIoSink 
	: public IMEIOSink
	, public CMEMutexTypeTraits<CMEConnDummy>
	, virtual public CMEReferenceControlT<CMELockMutex>
{};

ME_NAME_SPACE_END

#endif	// _ASYCH_IO_DEFINES_H_


