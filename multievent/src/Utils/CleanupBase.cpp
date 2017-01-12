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
	* 原本这个地方应该有个else并且asserte一下
	* 以确保出错的时候有个trace，但是也正因为有trace
	* 就会生成打印trace的instance，而instance在HikUtility
	* 是退出时才会delete，当然，这个函数也只会在退出时
	* 用到，但是如果之前trace的instance被删除，而这里又
	* 生成了一个(如果这里asserte的话)就会产生一些无法预料
	* 结果，所以，为了避免这种情况的发生，就把ME_ASSERTE去掉
	*/
}

CMECleanupBase::~CMECleanupBase()
{

}

void CMECleanupBase::CleanupAll()
{
	/* 这里要拿到一个全局的mutex */
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
	* 原本这个地方应该有个else并且asserte一下
	* 以确保出错的时候有个trace，但是也正因为有trace
	* 就会生成打印trace的instance，而instance在HikUtility
	* 是退出时才会delete，当然，这个函数也只会在退出时
	* 用到，但是如果之前trace的instance被删除，而这里又
	* 生成了一个(如果这里asserte的话)就会产生一些无法预料
	* 结果，所以，为了避免这种情况的发生，就把ME_ASSERTE去掉
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









