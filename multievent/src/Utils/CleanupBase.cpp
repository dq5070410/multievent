#include "CleanupBase.h"
#include "LockThreadMutex.h"
#include "LockGuardT.h"

ME_NAME_SPACE_BEGIN

CMECleanupBase* CMECleanupBase::s_pHeader = NULL;
CMECleanupBase::MutexType* CMECleanupBase::s_pMutex = NULL;

CMECleanupBase::CMECleanupBase()
	: m_pNext( NULL )
{
	MutexType* pMutex = GetCleanupMutex();

	if ( pMutex )
	{
		CMELockGuardT<MutexType> Guard( pMutex );
		m_pNext = s_pHeader;
		s_pHeader = this;
	}
	/**
	* ԭ������ط�Ӧ���и�else����asserteһ��
	* ��ȷ�������ʱ���и�trace������Ҳ����Ϊ��trace
	* �ͻ����ɴ�ӡtrace��instance����instance��HikUtility
	* ���˳�ʱ�Ż�delete����Ȼ���������Ҳֻ�����˳�ʱ
	* �õ����������֮ǰtrace��instance��ɾ������������
	* ������һ��(�������asserte�Ļ�)�ͻ����һЩ�޷�Ԥ��
	* ��������ԣ�Ϊ�˱�����������ķ������Ͱ�ME_ASSERTEȥ��
	*/
}

CMECleanupBase::~CMECleanupBase()
{

}

void CMECleanupBase::CleanupAll()
{
	/* ����Ҫ�õ�һ��ȫ�ֵ�mutex */
	MutexType* pMutex = GetCleanupMutex();

	if ( pMutex )
	{
		{
			CMELockGuardT<MutexType> Guard( pMutex );
			while ( s_pHeader )
			{
				CMECleanupBase* pTmp = s_pHeader->m_pNext;
				s_pHeader->Cleanup();
				s_pHeader = pTmp;
			}
		}

		ReleaseCleanupMutex();
	}
	/**
	* ԭ������ط�Ӧ���и�else����asserteһ��
	* ��ȷ�������ʱ���и�trace������Ҳ����Ϊ��trace
	* �ͻ����ɴ�ӡtrace��instance����instance��HikUtility
	* ���˳�ʱ�Ż�delete����Ȼ���������Ҳֻ�����˳�ʱ
	* �õ����������֮ǰtrace��instance��ɾ������������
	* ������һ��(�������asserte�Ļ�)�ͻ����һЩ�޷�Ԥ��
	* ��������ԣ�Ϊ�˱�����������ķ������Ͱ�ME_ASSERTEȥ��
	*/
}

void CMECleanupBase::Cleanup()
{
	delete this;
}

CMECleanupBase::MutexType* CMECleanupBase::GetCleanupMutex()
{
	if ( s_pMutex )
	{
		return s_pMutex;
	}

	s_pMutex = new CMELockThreadMutex;

	return s_pMutex;
}

void CMECleanupBase::ReleaseCleanupMutex()
{
	if ( s_pMutex )
	{
		ME_DELETE( s_pMutex );
	}
}

ME_NAME_SPACE_END









