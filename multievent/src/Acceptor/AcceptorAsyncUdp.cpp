#include "AcceptorAsyncUdp.h"
#include "ConnectionDefines.h"
#include "MessageBlock.h"
#include "Reactor.h"
#include "Thread.h"
#include "ThreadManager.h"

ME_NAME_SPACE_BEGIN

CMEAcceptorAsyncUdp::CMEAcceptorAsyncUdp()
#ifdef ME_LINUX
	: CMEAsyncIoResult(
			CMEReactor::EVENT_TYPE_ACCEPT,
			NULL )
#else	// ME_LINUX

	: m_pRefSink( NULL )
#endif	// ME_LINUX
{

}

CMEAcceptorAsyncUdp::~CMEAcceptorAsyncUdp()
{

}

DWORD CMEAcceptorAsyncUdp::AddReference()
{
	if ( m_pRefSink )
	{
		m_pRefSink->AddReference();
	}

	return CMEReferenceControlT<MutexType>::AddReference();
}

DWORD CMEAcceptorAsyncUdp::RemoveReference()
{
	if ( m_pRefSink )
	{
		m_pRefSink->RemoveReference();
	}

	return CMEReferenceControlT<MutexType>::RemoveReference();
}

#ifdef ME_WIN

ME_Result CMEAcceptorAsyncUdp::Open( 
	IMEAcceptorImplSink* pSink, 
	CMEReactor* pReactor, 
	const CMEInetAddress& hiaLocalAddress )
{
	ME_Result hResult = CMEAcceptorUdp::Open(
		pSink,
		pReactor,
		hiaLocalAddress );

	if ( ME_FAILED(hResult) )
	{
		return hResult;
	}
	
	CMEMessageBlock* pmbBlock = CreateAsyncIoBuf();

	return m_asyncRecvFrom.Post(
		this,
		GetHandle(),
		pmbBlock,
		&m_hiaFromAddress );
}

void CMEAcceptorAsyncUdp::Close( CMEReactor* pReactor )
{
	m_asyncRecvFrom.Cancel( GetHandle() );

	CMEAcceptorUdp::Close( pReactor );
}

CMEMessageBlock* CMEAcceptorAsyncUdp::CreateAsyncIoBuf()
{
	char* pszBuf = new char[ME_CONNECTION_SEND_BUF_SIZE];
	CMEMessageBlock* pmbBlock = new CMEMessageBlock( 
		ME_CONNECTION_SEND_BUF_SIZE, 
		pszBuf, 
		ME_CONNECTION_SEND_BUF_SIZE, 
		CMEMessageBlock::MESSAGEBLOCK_ATTR_DELETE_USERDATA );

	return pmbBlock;
}

void CMEAcceptorAsyncUdp::OnInput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	CMEAcceptorUdp::OnInput(
		hHandle,	// ֱ�Ӱ�resultָ�������GetHandle()����ȥ
		iEvent );

	/**
	* ���õ������ϱߵĻص������з���һЩ����رա�����֮��
	* ���ܻᵼ�±�����ɾ���������±ߵĵ��û�crash������
	*
	* wrapper�ǻ�㶨��
	*/

	/**
	* ��һ���첽��RecvFromIo�Ѿ����ģ�����Ҫ��Ͷ��һ��
	* ��Ϊ��һ������������������׼��
	*/
	CMEMessageBlock* pmbBlock = CreateAsyncIoBuf();

	m_asyncRecvFrom.Post(
		this,
		GetHandle(),
		pmbBlock,
		&m_hiaFromAddress );
}

void CMEAcceptorAsyncUdp::OnOutput( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	ME_ASSERTE( FALSE );
}

void CMEAcceptorAsyncUdp::OnClose( 
	ME_HANDLE hHandle, 
	int iEvent )
{
	ME_ASSERTE( FALSE );
}

#elif defined( ME_LINUX )

void CMEAcceptorAsyncUdp::OnCompleteImpl( 
	BOOL bResult,
	int iActiveEvent,
	ME_HANDLE hInterface )
{
	AddReference();

	CMEAcceptorUdp::OnInput(
		GetHandle(),
		CMEReactor::EVENT_TYPE_IN );

	CMEReactor* pReactor = ((CMEThread*)CMEThreadManagerSingleton::Instance()->GetNetworkThread())->GetReactor();
	pReactor->Register(
		this,
		this,
		CMEReactor::EVENT_TYPE_IN,
		m_iIndex );
}

void CMEAcceptorAsyncUdp::OnDestroy()
{
	RemoveReference();
}


#endif	// ME_WIN


ME_NAME_SPACE_END




