/**
* Acceptor的创建工厂
*
* 各类的acceptor都由它创建
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
* Acceptor的创建工厂
*
* 各类的acceptor都由它创建
*/
class CMEAcceptorFactory
{
public:
	/**
	* 创建Acceptor
	*
	* 返回值:
	*		ME_OK为成功; 其他为失败
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

typedef CMESingletonT<CMEAcceptorFactory> CMEAcceptorFactorySingleton;	// acceptor工厂单例

ME_NAME_SPACE_END

#endif	// __ACCEPTOR_FACTORY_H_








