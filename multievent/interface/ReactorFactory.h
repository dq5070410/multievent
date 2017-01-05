/**
 *  ����������������
 *
 *
 *  History
 *  02/19/2016 create file
 */


#ifndef __REACTOR_FACTORY_H_
#define __REACTOR_FACTORY_H_

#include "Base.h"
#include "SingletonT.h"
#include "LockGuardT.h"

ME_NAME_SPACE_BEGIN

class CMEReactor;

class CMEReactorFactory
{
public:
    /**
     *  ���ݷ��������ʹ���������
     *
     *  <iType>, ������������
     *  <pReactor>, ���������ʵ��ָ�루����ɹ����������ָ��Ӧ��ΪNULL
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    ME_Result Create(
        INT iType,
		INT iClassificationType,	// 2013.9.16 added by Τ��
        CMEReactor*& pReactor );

	CMEReactor* Create( 
		INT iType,
		DWORD dwThreadNum );

    /**
     *  ���ٷ�����
     *
     *  <pReactor>, ������ʵ��ָ��
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
	ME_Result Destroy( CMEReactor*& pReactor );
};

typedef CMESingletonT<CMEReactorFactory> CMEReactorFactorySingleton;

ME_NAME_SPACE_END

#endif // __REACTOR_FACTORY_H_

