#include "TransportUdpManager.h"
#include "ConnectionInterface.h"
#include "ConnectionDefines.h"
#include "TransportUdp.h"
#include "TransportFactory.h"
#include "TransportWrapper.h"

ME_NAME_SPACE_BEGIN

CMETransportUdpManager::CMETransportUdpManager()
{

}

CMETransportUdpManager::~CMETransportUdpManager()
{
	if ( FALSE == m_TransportMap.empty() )
	{
		MapType::iterator iter = m_TransportMap.begin();
		for ( ; iter != m_TransportMap.end(); ++iter )
		{
			iter->second->RemoveReference();
		}

		m_TransportMap.clear();
	}
}

CMETransportUdpManager::TransportType* CMETransportUdpManager::FindTransport( const CMEInetAddress& peerAddress )
{
	CMELockGuardT<MutexType> Guard( &m_lock );

	if ( TRUE == m_TransportMap.empty() )
	{
		return NULL;
	}

	MapType::iterator iter = m_TransportMap.find( peerAddress );
	if ( iter != m_TransportMap.end() )
	{
		return iter->second;
	}

	return NULL;
}

ME_Result CMETransportUdpManager::RegisterTransport( 
	const CMEInetAddress& peerAddress, 
	TransportType* pTransport )
{
	CMELockGuardT<MutexType> Guard( &m_lock );

	MapType::iterator iter = m_TransportMap.find( peerAddress );
	if ( iter != m_TransportMap.end() )
	{
		return ME_OK;
	}

	m_TransportMap[peerAddress] = pTransport;

	return ME_OK;
}

ME_Result CMETransportUdpManager::UnRegisterTransport( 
	const CMEInetAddress& peerAddress, 
	TransportType* pTransport )
{
	CMELockGuardT<MutexType> Guard( &m_lock );

	if ( TRUE == m_TransportMap.empty() )
	{
		return ME_OK;
	}

	m_TransportMap.erase( peerAddress );

	return ME_OK;
}

void CMETransportUdpManager::OnUserConnect( 
	DWORD dwType, 
	const CMEInetAddress& peerAddress,
	TransportType*& pNewTransport )
{
	/* 根据peerAddress创建transport */
	CreateTransport(
		dwType,
		peerAddress,
		pNewTransport );

	/* 把这条transport注册 */
	RegisterTransport(
		peerAddress,
		pNewTransport );
}

void CMETransportUdpManager::OnUserDataReceive( 
	TransportType* pTransport, 
	const char* pszData, 
	UINT uLenght )
{
	ME_ASSERTE_RETURN_VOID( pTransport );

#ifdef ME_DEBUG
	CMETransportWrapper* pWrapper = dynamic_cast<CMETransportWrapper*>( pTransport );
	ME_ASSERTE( pWrapper );
#endif	// ME_DEBUG

	/* 通知这条transport，它收到数据了 -- 这样看起来就和TCP一样了 */
	((CMETransportWrapper*)pTransport)->OnReceive_c(
		pszData,
		uLenght );
}

void CMETransportUdpManager::CreateTransport( 
	DWORD dwType, 
	const CMEInetAddress& peerAddress, 
	TransportType*& pNewTransport )
{
	CMELockGuardT<MutexType> Guard( &m_lock );

	CMESocketUdp tmpSocket;
	CMEInetAddress hiaLocal;

	CMEInetAddress::GetLocalAddress( 
		hiaLocal,
		TRUE,
		peerAddress.IsLocalHost() );

	tmpSocket.Open( hiaLocal, FALSE );
	tmpSocket.Enable( CMEIPBase::NON_BLOCK );

	CMETransportFactorySingleton::Instance()->Create(
		dwType,
		tmpSocket.GetHandle(),
		pNewTransport,
		&const_cast<CMEInetAddress&>(peerAddress) );
}

ME_NAME_SPACE_END







