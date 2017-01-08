/**
* Acceptor�Ĵ�������
*
* �����acceptor����������
* 
*
* History
*	12/14/2016		Created
*/

#ifndef __ACCEPTOR_FACTORY_H_
#define __ACCEPTOR_FACTORY_H_

#include "Base.h"
#include "LockThreadMutex.h"
#include "SingletonT.h"

ME_NAME_SPACE_BEGIN

class IMEAcceptorImpl;

/**
* Acceptor�Ĵ�������
*
* �����acceptor����������
*/
class CMEAcceptorFactory
{
public:
	/**
	* ����Acceptor
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�; ����Ϊʧ��
	*/
	ME_Result Create( 
		DWORD dwType,
		IMEAcceptorImpl*& pAcceptor );

private:
	ME_Result CreateSync(
		DWORD dwType,
		IMEAcceptorImpl*& pAcceptor );


	ME_Result CreateAsync(
		DWORD dwType,
		IMEAcceptorImpl*& pAcceptor );
};

typedef CMESingletonT<CMEAcceptorFactory> CMEAcceptorFactorySingleton;	// acceptor��������

ME_NAME_SPACE_END

#endif	// __ACCEPTOR_FACTORY_H_








